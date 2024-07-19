/* Sequential code of the SPM project a.a. 23/24
Compile using:
g++ -std=c++20 -O3 -march=native -I. Wf_seq.cpp -o wfs
*/

#include <iostream>
#include <vector>
#include <cmath>

#include <hpc_helpers.hpp>  //For TIMER* macro

int main(int argc, char *argv[]) {

    if (argc != 1 && argc != 2) {
        std::cout << "use " << argv[0] << " <MatrixSize>" << std::endl;
        return -1;
    }

    uint64_t N = 512;   //Default value
    if (argc > 1) {
        N = std::stol(argv[1]);
    }
    std::cout << "Sequential <MatrixSize>,<Workers>" << std::endl << "Wavefront(" << N << ",1) --> ";

    std::vector<double> M(N*N, 0);  //Filled with 0

    auto init=[&]() {
        for(uint64_t i=0; i < N; i++) {    //For each element in the major diagonal
            double m = (i + 1.) / N;
            M[i*N + i] = m;
        }
    };
    init();

    TIMERSTART(wavefront);
    uint64_t diag_size = N-1;
    for(uint64_t i=1; i < N; i++) {  //For each diagonal
        for(uint64_t j=0; j < diag_size; j++) {  //For each element in the diagonal
            uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position

            double dp = 0.0;
            for(uint64_t k=0; k < i; k++) {
                dp = dp + (M[vect_pos - k - 1]* M[vect_pos + ((k + 1) * N)]);
            }
            dp = std::cbrt(dp);
            M[vect_pos] = dp;
        }
        diag_size--;
    }
    TIMERSTOP(wavefront);

    #if 0   //Print matrix
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
