// Parallel code of the second SPM Assignment a.a. 23/24
// Compile using:
// g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o wc Word-Count-ff-file.cpp -pthread
//
// Execute with:
// ./wc /opt/SPMcode/A2/filelist.txt 6 0 5 1
//
// Schema:
//                   |────> Worker ──|
//         Emitter ──|────> Worker ──├───|
//           ^       |────> Worker ──|   |
//			 |                           |
//           └───────────────────────────|
//

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <set>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
using namespace ff;

using umap=std::unordered_map<std::string, uint64_t>;
using pair=std::pair<std::string, uint64_t>;
struct Comp {
	bool operator ()(const pair& p1, const pair& p2) const {
		if (p1.second == p2.second)
			return p1.first < p2.first;
		return p1.second > p2.second;
	}
};
using ranking=std::multiset<pair, Comp>;

// ------ globals --------
uint64_t total_words{0};
volatile uint64_t extraworkXline{0};
// ----------------------

void tokenize_line(const std::string& line, umap& UM) {
	char *tmpstr;
	char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
	while(token) {
		++UM[std::string(token)];
		token = strtok_r(NULL, " \r\n", &tmpstr);
		++total_words;
	}
	for(volatile uint64_t j{0}; j<extraworkXline; j++);
}

void compute_file(const std::string& filename, umap& UM) {
	std::ifstream file(filename, std::ios_base::in);
	if (file.is_open()) {
		std::string line;
		while(std::getline(file, line)) {
			if (!line.empty()) {
				tokenize_line(line, UM);
			}
		}
	} 
	file.close();
}

struct Task_t {
    Task_t(std::string file):file(file),local_words(0) {}
	const std::string file;
	uint64_t local_words;
    umap local_UM;
};

struct Emitter: ff_monode_t<Task_t> {

    Emitter(const std::vector<std::string>& filenames)
        :filenames(filenames) {}

    Task_t *svc(Task_t *in) {
        if (in == nullptr) { // first call
			const int nw = get_num_outchannels(); // gets the total number of workers added to the farm

			uint64_t i = 0;
			for (auto fn : filenames) {
				Task_t *task = new Task_t(fn);
				ff_send_out_to(task, i % nw);
				++i;
			}
			broadcast_task(EOS);
			return GO_ON;  // keep me alive
        }
		// someting coming back from Workers (Reduction)
		auto &lUM = in->local_UM;
		for (const auto& pair : lUM) {
			UM[pair.first] += pair.second;
		}
		total_words += in->local_words;
		delete in;
        return GO_ON;  // keep me alive
    }
    // these are the values in the Emitter
    std::vector<std::string> filenames;
    // this is where we store the results coming back from the Workers
    umap UM;

};

struct Worker: ff_node_t<Task_t> {
    Task_t *svc(Task_t *in) {
		auto& UM   = in->local_UM;
		auto& words   = in->local_words;

		std::ifstream file(in->file, std::ios_base::in);
		if (file.is_open()) {
			std::string line;
			std::vector<std::string> V;
			while(std::getline(file, line)) {
				if (!line.empty()) {
					char *tmpstr;
					char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
					while(token) {
						++UM[std::string(token)];
						token = strtok_r(NULL, " \r\n", &tmpstr);
						++words;
					}
					for(volatile uint64_t j{0}; j<extraworkXline; j++);
				}
			}
		} 
		file.close();

        return in;
    }
};

int main(int argc, char *argv[]) {

	auto usage_and_exit = [argv]() {
		std::printf("use: %s filelist.txt nworkers [extraworkXline] [topk] [showresults]\n", argv[0]);
		std::printf("   - filelist.txt    contains one txt filename per line\n");
		std::printf("   - nworkers        in the number of threads as 1 Emitter + (nw-1) Workers\n");
		std::printf("   - extraworkXline  is the extra work done for each line, it is an integer value whose default is 0\n");
		std::printf("   - topk            is an integer number, its default value is 10 (top 10 words)\n");
		std::printf("   - showresults     is 0 or 1, if 1 the output is shown on the standard output\n\n");
		exit(-1);
	};

	std::vector<std::string> filenames;
	size_t topk = 10;
	bool showresults = false;
	if (argc < 3 || argc > 6) {
		usage_and_exit();
	}

	if (argc > 3) {
		try { extraworkXline=std::stoul(argv[3]);
		} catch(std::invalid_argument const& ex) {
			std::printf("%s is an invalid number (%s)\n", argv[3], ex.what());
			return -1;
		}
		if (argc > 4) {
			try { topk=std::stoul(argv[4]);
			} catch(std::invalid_argument const& ex) {
				std::printf("%s is an invalid number (%s)\n", argv[4], ex.what());
				return -1;
			}
			if (topk==0) {
				std::printf("%s must be a positive integer\n", argv[4]);
				return -1;
			}
			if (argc == 6) {
				int tmp;
				try { tmp=std::stol(argv[5]);
				} catch(std::invalid_argument const& ex) {
					std::printf("%s is an invalid number (%s)\n", argv[5], ex.what());
					return -1;
				}
				if (tmp == 1) showresults = true;
			}
		}
	}

	if (std::filesystem::is_regular_file(argv[1])) {
		std::ifstream file(argv[1], std::ios_base::in);
		if (file.is_open()) {
			std::string line;
			while(std::getline(file, line)) {
				if (std::filesystem::is_regular_file(line))
					filenames.push_back(line);
				else
					std::cout << line << " is not a regular file, skip it\n";
			}
		} else {
			std::printf("ERROR: opening file %s\n", argv[1]);
			return -1;
		}
		file.close();
	} else {
		std::printf("%s is not a regular file\n", argv[1]);
		usage_and_exit();
	}

	const size_t nw = std::stoul(argv[2]);
	if (nw == 1)
		std::printf("Sequential execution\n");
	else if (nw >= 2)
		std::printf("%ld Worker(s) + 1 Emitter\n", nw-1);
	else {
		std::printf("%s is an invalid number\n", argv[2]);
		return -1;
	}

	umap UM;	// used for storing results
	if (nw != 1) { // Parallel execution
		ffTime(START_TIME);
		ff_Farm<> farm([&]() {
			std::vector<std::unique_ptr<ff_node> > W;
			for(size_t i=0;i<(nw-1);++i)
				W.push_back(make_unique<Worker>());
			return W;
	    } () ); // by default it has both an emitter and a collector
		
		Emitter E(filenames);        // creating the Emitter
		farm.add_emitter(E);      // replacing the default emitter
		farm.remove_collector();  // removing the default collector
		farm.wrap_around();       // adding feedback channels between Workers and the Emitter
		
		if (farm.run_and_wait_end() < 0) {
			error("running farm\n");
			return -1;
		}
		UM = E.UM;
		ffTime(STOP_TIME);
		auto computeTime = ff::ffTime(ff::GET_TIME);

		ffTime(START_TIME);
		ranking rank(UM.begin(), UM.end());		// sorting in descending order
		ffTime(STOP_TIME);

		std::cout << "Compute time: " << computeTime / 1000 << " (s)\n";
		std::cout << "Sorting time: " << ff::ffTime(ff::GET_TIME) / 1000 << " (s)\n";

		if (showresults) {		// show the results
			std::cout << "Unique words " << rank.size() << "\n";
			std::cout << "Total words  " << total_words << "\n";
			std::cout << "Top " << topk << " words:\n";
			auto top = rank.begin();
			for (size_t i=0; i < std::clamp(topk, 1ul, rank.size()); ++i)
				std::cout << top->first << '\t' << top++->second << '\n';
		}
	} else { // Sequential execution
		ffTime(START_TIME);
		for (auto f : filenames) {
			compute_file(f, UM);
		}
		ffTime(STOP_TIME);
		auto computeTime = ff::ffTime(ff::GET_TIME);

		ffTime(START_TIME);
		ranking rank(UM.begin(), UM.end());		// sorting in descending order
		ffTime(STOP_TIME);

		std::cout << "Compute time: " << computeTime / 1000 << " (s)\n";
		std::cout << "Sorting time: " << ff::ffTime(ff::GET_TIME) / 1000 << " (s)\n";

		if (showresults) {		// show the results
			std::cout << "Unique words " << rank.size() << "\n";
			std::cout << "Total words  " << total_words << "\n";
			std::cout << "Top " << topk << " words:\n";
			auto top = rank.begin();
			for (size_t i=0; i < std::clamp(topk, 1ul, rank.size()); ++i)
				std::cout << top->first << '\t' << top++->second << '\n';
		}
	}
	return 0;
}
