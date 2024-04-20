// Parallel code of the second SPM Assignment a.a. 23/24
// Compile using:
// g++ -std=c++20 -I. -I. -Wall -O3 -o wc Word-Count-par.cpp -pthread -fopenmp
//
// Execute with:
// ./wc /opt/SPMcode/A2/filelist.txt 0 5 1

#include <omp.h>  // used here just for omp_get_wtime()
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

void tokenize_line(const std::string& line, umap& UMlocal, uint64_t *tw) {
	char *tmpstr;
	char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
	while(token) {
		++UMlocal[std::string(token)];
		token = strtok_r(NULL, " \r\n", &tmpstr);
		++(*tw);
	}
	for(volatile uint64_t j{0}; j<extraworkXline; j++);
}

void compute_file_critical(const std::string& filename, umap& UM, uint64_t *tw, umap& UMlocal) {
	std::ifstream file(filename, std::ios_base::in);
	if (file.is_open()) {
		std::string line;
		std::vector<std::string> V;
		while(std::getline(file, line)) {
			if (!line.empty()) {
				tokenize_line(line, UMlocal, tw);
			}
		}
	} 
	file.close();
	// Update the external values of UM within the critical region by the local copie of UM
#pragma omp critical
	{
		for (const auto& pair : UMlocal)
			UM[pair.first] += pair.second;
	}
}

void tokenize_line_atomic(const std::string& line, umap& UM, uint64_t *tw) {
	char *tmpstr;
	char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
	while(token) {
		std::lock_guard<std::mutex> lock_guard(mutex);
			++UM[std::string(token)];
		token = strtok_r(NULL, " \r\n", &tmpstr);
		++(*tw);
	}
	for(volatile uint64_t j{0}; j<extraworkXline; j++);
}

void compute_file(const std::string& filename, umap& UM, uint64_t *tw) {
	std::ifstream file(filename, std::ios_base::in);
	if (file.is_open()) {
		std::string line;
		std::vector<std::string> V;
		while(std::getline(file, line)) {
			if (!line.empty()) {
				tokenize_line_atomic(line, UM, tw);
			}
		}
	} 
	file.close();
}

void tokenize_line_critical(const std::string& line, umap& UM, uint64_t *tw) {
	char *tmpstr;
	char *token = strtok_r(const_cast<char*>(line.c_str()), " \r\n", &tmpstr);
	while(token) {
#pragma omp critical
{
			++UM[std::string(token)];
}
			token = strtok_r(NULL, " \r\n", &tmpstr);
			++(*tw);
	}
	for(volatile uint64_t j{0}; j<extraworkXline; j++);
}

void compute_file2(const std::string& filename, umap& UM, uint64_t *tw) {
	std::ifstream file(filename, std::ios_base::in);
	if (file.is_open()) {
			std::string line;
			std::vector<std::string> V;
			while(std::getline(file, line)) {
					if (!line.empty()) {
							tokenize_line_critical(line, UM, tw);
					}
			}
	}
file.close();
}

int main(int argc, char *argv[]) {

	auto usage_and_exit = [argv]() {
		std::printf("use: %s filelist.txt [extraworkXline] [topk] [showresults]\n", argv[0]);
		std::printf("     filelist.txt contains one txt filename per line\n");
		std::printf("     extraworkXline is the extra work done for each line, it is an integer value whose default is 0\n");
		std::printf("     topk is an integer number, its default value is 10 (top 10 words)\n");
		std::printf("     showresults is 0 or 1, if 1 the output is shown on the standard output\n\n");
		exit(-1);
	};

	std::vector<std::string> filenames;
	size_t topk = 10;
	bool showresults=false;
	if (argc < 2 || argc > 5) {
		usage_and_exit();
	}

	if (argc > 2) {
		try { extraworkXline=std::stoul(argv[2]);
		} catch(std::invalid_argument const& ex) {
			std::printf("%s is an invalid number (%s)\n", argv[2], ex.what());
			return -1;
		}
		if (argc > 3) {
			try { topk=std::stoul(argv[3]);
			} catch(std::invalid_argument const& ex) {
				std::printf("%s is an invalid number (%s)\n", argv[3], ex.what());
				return -1;
			}
			if (topk==0) {
				std::printf("%s must be a positive integer\n", argv[3]);
				return -1;
			}
			if (argc == 5) {
				int tmp;
				try { tmp=std::stol(argv[4]);
				} catch(std::invalid_argument const& ex) {
					std::printf("%s is an invalid number (%s)\n", argv[4], ex.what());
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
					std::cout << line << " is not a regular file, skipt it\n";
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

	// used for storing results of critical
	umap UMlocal;
	umap UMfinal;

	// start the time
	auto start_c = omp_get_wtime();

#pragma omp parallel for shared(UMfinal) firstprivate(UMlocal) reduction(+:total_words)
	for (auto f : filenames) {
		compute_file_critical(f, UMfinal, &total_words, UMlocal);
	}

	auto stop_c = omp_get_wtime();

	// used for storing results of tokenizer critical
	umap UMcritical;

	// start the time
	auto start_tc = omp_get_wtime();

#pragma omp parallel for shared(UMcritical) reduction(+:total_words)
	for (auto f : filenames) {
			compute_file2(f, UMcritical, &total_words);
	}

	auto stop_tc = omp_get_wtime();

	// used for storing results of atomic
	umap UMatomic;
	
	// start the time
	auto start_a = omp_get_wtime();

#pragma omp parallel for shared(UMatomic) reduction(+:total_words)
	for (auto f : filenames) {
		compute_file(f, UMatomic, &total_words);
	}

	auto stop_a = omp_get_wtime();

	auto start_s = omp_get_wtime();

	// sorting in descending order
	ranking rank(UMfinal.begin(), UMfinal.end());

        auto stop_s = omp_get_wtime();
        std::printf("Compute time critical (s)           %f\n",
                                stop_c - start_c);
        std::printf("Compute time tokenizer critical (s) %f\n",
                                stop_tc - start_tc);
        std::printf("Compute time guard_lock (s)         %f\n",
                                stop_a - start_a);
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

