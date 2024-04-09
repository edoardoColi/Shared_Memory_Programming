// Sequential code of the first SPM Assignment a.a. 23/24
// Compile using:
// g++ -std=c++20 -O3 -march=native -I. UTWavefront.cpp -o utw

#include <iostream>
#include <vector>
#include <thread>
#include <barrier>
#include <random>
#include <cassert>
#include <algorithm>        // for std::max
#include <hpc_helpers.hpp>  // for TIMER* macro
#include <threadPool.hpp>  // for ThreadPool implementation

int random(const int &min, const int &max) {
    static std::mt19937 generator(117);
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
};

// Emulate some work, just "waste of time"
void work(std::chrono::microseconds w) {
    std::cout << "I have " << w.count() << std::endl;
    auto end = std::chrono::steady_clock::now() + w;
    while(std::chrono::steady_clock::now() < end);  // Active waste
}

void usage(char* name) {
    std::printf("use: %s N [min max] t\n", name);
    std::printf("    N size of the square matrix\n");
    std::printf("    min waiting time (us)\n");
    std::printf("    max waiting time (us)\n");
    std::printf("    t threads spawned\n");
}

void blockWavefront(const std::vector<int> &M, const uint64_t &N, const uint64_t &diag, const uint64_t &from, const uint64_t &to) {
    uint64_t d = diag;
    uint64_t f = from;
    uint64_t t = to > (N-d)? N-d : to;

    std::cout << "Blocco di lavoro" << std::endl;
    for(uint64_t i = f; i < t; ++i) {        // For each element in the block of the scoped diagonal
        work(std::chrono::microseconds(M[i*N+(i+d)]));
    }
}

void parallelWavefront(const std::vector<int> &M, const uint64_t &N, const uint64_t &t) {

    ThreadPool TP(t);
    uint64_t blockSize = 1; //TODO magari mettere una dimenzione condizionata. non m/p perche senno perdo in prestazioni in caso di workload non bilanciati

    for(uint64_t k = 0; k < N; ++k) {                // For each upper diagonal
        
        // std::barrier bar(t);                          //TODO dovrebbero essere t spawnti piu il corrente
        
        for(uint64_t i = 0; i < (N-k); i += blockSize) {        // For each element in the diagonal
            // TP.enqueue(blockWavefront, M, N, k, i, i + blockSize);
            blockWavefront(M, N, k, i, i + blockSize);
        }
        // std::cout << "Thread MAIN waiting for barrier" << std::endl;
        // bar.arrive_and_wait();
        // std::cout << "Thread MAIN crossed for barrier" << std::endl;
        std::cout << "Barrier Cambio diagonale ------------" << std::endl;  //TODO madari da bettere come esecuzione in barrier
    }
}

int main(int argc, char *argv[]) {
    int min    = 0;                                 // Default minimum time (in microseconds)
    int max    = 1000;                              // Default maximum time (in microseconds)
    uint64_t t = std::thread::hardware_concurrency();   // Default maximum threads
    uint64_t N = 512;                               // Default size of the matrix (NxN)

    if (argc != 1 && argc != 2 && argc != 4 && argc != 5) {
        usage(argv[0]);
        return -1;
    }
    if (argc > 1) {
        N = std::stol(argv[1]);
        if (argc > 2) {
            min = std::stol(argv[2]);               // Suppose to have values bigger-equal than 0
            max = std::stol(argv[3]);               // Suppose to have values bigger-equal than 0
            if (min > max) {
                usage(argv[0]);
                return -1;
            }
            if (argc > 4)
                t = std::stol(argv[4]);               // Suppose to have values bigger than 0
        }
    } else {
        std::printf("Using default values\n");
    }

    std::vector<int> M(N*N, -1);                    // Allocate the matrix, default value -1
    uint64_t expected_sequentialtime=0;
    uint64_t minimum_paralleltime=0;

    // init function
    auto init=[&]() {
        for(uint64_t k = 0; k< N; ++k) {            // For each upper diagonal
            int maxVal=-1;
            for(uint64_t i = 0; i< (N-k); ++i) {    // For each element in the diagonal
                int t = random(min,max);
                M[i*N+(i+k)] = t;
                expected_sequentialtime +=t;
                maxVal = std::max(maxVal, M[i*N+(i+k)]);
            }
            minimum_paralleltime += maxVal;
        }
    };
    init();
    std::printf("Expected Sequential compute time   (p:1 s:1) ~ %f (s)\n", expected_sequentialtime/1000000.0);
    std::printf("Minimum Parallel compute time    (p:inf s:1) ~ %f (s)\n", minimum_paralleltime/1000000.0);
    #if 1
        for(uint64_t i=0;i<N;++i){
            for(uint64_t j=0; j<N;++j){
                std::printf("%4d ",M[i*N+j]);
            }
        std::printf("\n");
        }
    #endif

    TIMERSTART(wavefront);
    parallelWavefront(M, N, t);
    TIMERSTOP(wavefront);

    return 0;
}