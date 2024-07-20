/* Parallel code of the SPM project a.a. 23/24
Compile using:
g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o wff Wf_ff.cpp -pthread
*/

#include <iostream>
#include <vector>
#include <cmath>

#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/all2all.hpp>

using namespace ff;

int main(int argc, char *argv[]) {

    if (argc != 1 && argc != 2 && argc != 3 && argc != 4) {
        std::cout << "use " << argv[0] << " <MatrixSize> <NumLWorkers> <NumRWorkers>" << std::endl;
        return -1;
    }

    uint64_t N = 512;   //Default value
    if (argc > 1) {
        N = std::stol(argv[1]);
    }
    uint64_t Lw = 1;    //Default value
    if (argc > 2) {
        Lw = std::stol(argv[2]);
    }
    uint64_t Rw = 15;   //Default value
    if (argc > 3) {
        Rw = std::stol(argv[3]);
    }
    std::cout << "Parallel <MatrixSize>,<LeftWorkers>,<RightWorkers>" << std::endl << "Wavefront(" << N << "," << Lw << "," << Rw << ") --> ";

    std::vector<double> M(N*N, 0);  //Filled with 0

    auto init=[&]() {
        for(uint64_t i=0; i < N; i++) {    //For each element in the major diagonal
            double m = (i + 1.) / N;
            M[i*N + i] = m;
        }
    };
    init();

    ffTime(START_TIME);
	ffTime(STOP_TIME);
    std::cout << ffTime(GET_TIME) << std::endl;


    #if 1   //Print matrix
    std::printf("\n");
    for(uint64_t i=0; i < N; i++){
        for(uint64_t j=0; j < N; j++){
            std::printf("%f ",M[i*N + j]);
        }
    std::printf("\n");
    }
    #endif

    return 0;
}
