// Compile using:
// g++ -std=c++20 -Wall -O3 -DNDEBUG -o pseq Project_seq.cpp


#include <iostream>
#include <vector>
#include <cmath>

#include <thread> //used only for the timer

void usage(char* name) {
    std::printf("use: %s N\n", name);
    std::printf("    N size of the square matrix\n");
}







int main(int argc, char *argv[]) {
    
    uint64_t N = 512;           // Default size of the matrix (NxN)
   
    if (argc != 1 && argc != 2) {
        usage(argv[0]);
        return -1;
    }

    if (argc > 1) {
                N = std::stol(argv[1]);
    } else {
        std::printf("Using default values: matrix size=%ld\n", N);
    }


    std::vector<double> M(N*N, 0); // Allocate the matrix, default value 0

    // init function
    auto init=[&]() {
            for(uint64_t i = 0; i< N; ++i) { // For each element in the major diagonal
                double t = (i+1);
                // t= t/N;
                M[i*N + i] = t;
            }
        };
    init();

   
#if 1 //stampa la matrice
        for(uint64_t i=0;i<N;++i){
            for(uint64_t j=0; j<N;++j){
                std::printf("%f ",M[i*N+j]); //TODO gli elementi sono double, ok cosi?
            }
        std::printf("\n");
        }
#endif









    auto start = std::chrono::high_resolution_clock::now(); //TODO il timer va bene questo?




    uint64_t size=N-1;


    for(uint64_t i=0;i<(N-1);++i){
        for(uint64_t k=0;k<size;++k){
            uint64_t diag= N-size;
            uint64_t current_pos= (k*(N +1)) +diag;
            double result=0;

            for(uint64_t j=0; j<diag;++j){
                result=result + (M[current_pos -j-1]* M[current_pos +((j+1)*N)]);
            }
            // result=std::cbrt(result);
            M[current_pos]=result;
        }
        size--;
    }








    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "tempo sequenziale: " << elapsed_seconds.count() << std::endl;

#if 1 //stampa la matrice 
    for(uint64_t i=0;i<N;++i){
        for(uint64_t j=0; j<N;++j){
            std::printf("%f ",M[i*N+j]); //TODO gli elementi sono double, ok cosi?
        }
        std::printf("\n");
    }
#endif
    
    return 0;
}