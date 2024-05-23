// Parallel code of the second SPM Assignment a.a. 23/24
// Compile using:
// g++ -std=c++20 -I. -I. -Wall -O3 -o wc Word-Count-omp-line.cpp -pthread -fopenmp
//
// Execute with:
// ./wc /opt/SPMcode/A2/filelist.txt 6 0 5 1

#include <omp.h>  // used for omp_get_wtime()
#include <cstring>
#include <vector>
#include <set>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>

#include <mutex>
#include <thread>

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
std::mutex mutex;
// ----------------------

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

	ssize_t nw = std::stoul(argv[2]);
	if (nw > omp_get_max_threads()) {
		nw = omp_get_max_threads();
		std::printf("omp with %ld(capped to max) threads\n", nw);
	}
	else if (nw >= 1)
		std::printf("omp with %ld threads\n",nw);
	else {
		std::printf("Invalid number (%s)\n", argv[2]);
		return -1;
	}

	// used for storing results of critical
	umap UMfinal;

	// start the time
	auto start = omp_get_wtime();
	
#pragma omp parallel num_threads(nw)
	{
	#pragma omp single
		for (auto f : filenames) {

				std::ifstream file(f, std::ios_base::in);
				if (file.is_open()) {
					std::string line;
					std::vector<std::string> V;
					while(std::getline(file, line)) {
						if (!line.empty()) {

							uint64_t local_words = 0;
							umap local_UM;
							#pragma omp task firstprivate(local_words,local_UM) shared(UMfinal) untied // Global variable total_words is shared by default
							{
								char *tmpstr;
								char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
								while(token) {
									++local_UM[std::string(token)];
									token = strtok_r(NULL, " \r\n", &tmpstr);
									++local_words;
								}
								for(volatile uint64_t j{0}; j<extraworkXline; j++);
								
								#pragma omp critical
								{
								total_words += local_words;
								for (const auto& pair : local_UM)
									UMfinal[pair.first] += pair.second;
								}
							}
						}
					}
				}
				file.close();



		}
	}
	auto stop = omp_get_wtime();

	auto start_s = omp_get_wtime();
	// sorting in descending order
	ranking rank(UMfinal.begin(), UMfinal.end());
	auto stop_s = omp_get_wtime();
	
	std::printf("Compute time (s) %f\n",
							stop - start);
	std::printf("Sorting time (s) %f\n",
							stop_s - start_s);

	if (showresults) {
		// show the results
		std::cout << "Unique words " << rank.size() << "\n";
		std::cout << "Total words  " << total_words << "\n";
		std::cout << "Top " << topk << " words:\n";
		auto top = rank.begin();
		for (size_t i=0; i < std::clamp(topk, 1ul, rank.size()); ++i)
			std::cout << top->first << '\t' << top++->second << '\n';
	}
}

