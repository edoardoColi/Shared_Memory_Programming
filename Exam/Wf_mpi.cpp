/* Distributed code of the SPM project a.a. 23/24
Compile using:
mpicxx -std=c++20 -I. -Wall  -O3 -o wfm Wf_mpi.cpp 
Run using:
sbatch -N <value> laucher.sh
*/

#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv); //Initialize the MPI environment

    if (argc != 1 && argc != 2) {
        std::cout << "use " << argv[0] << " <MatrixSize>" << std::endl;
        MPI_Finalize();
        return -1;
    }

    uint64_t N = 512;   //Default value
    if (argc > 1) {
        N = std::stol(argv[1]);
    }

    int mpiSize, mpiRank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);  
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

    if (mpiRank == 0)
        std::cout << "Distributed <MatrixSize>,<Workers>" << std::endl << "Wavefront(" << N << "," << mpiSize << ") --> ";

    std::vector<double> M(N*N, 0);  //Filled with 0

    auto init=[&]() {
        for(uint64_t i=0; i < N; i++) {    //For each element in the major diagonal
            double m = (i + 1.) / N;
            M[i*N + i] = m;
        }
    };
    init();

    double startGlobal, endGlobal;
	startGlobal = MPI_Wtime();

    uint64_t diag_size = N - 1;
    uint64_t threshold = N - (mpiSize - 1);
    for(uint64_t i=1; i < threshold; i++) { //For each diagonal before the threshold aka do first threshold diagonals

        uint64_t task_size = (diag_size / mpiSize) + 1;
        std::vector<double> diag_values(task_size * mpiSize, 0);
        std::vector<double> local_values(task_size, 0);

        uint64_t from = mpiRank * task_size;
        uint64_t to = ((mpiRank + 1) * task_size < diag_size ? (mpiRank + 1) * task_size : diag_size);
        for(uint64_t j=from; j < to; j++) {  //For the assigned elements of the diagonal
            uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position
            double dp = 0.0;    //Dot product

// #pragma omp parallel for reduction(+:dp)
            for(uint64_t k=0; k < i; k++) {
                dp += (M[vect_pos - k - 1] * M[vect_pos + ((k + 1) * N)]);
            }

            dp = std::cbrt(dp);
            local_values[j-from] = dp;
        }

        MPI_Allgather(
                        local_values.data(),    //
                        task_size,              //
                        MPI_DOUBLE,             //
                        diag_values.data(),     //
                        task_size,              //
                        MPI_DOUBLE,             //
                        MPI_COMM_WORLD          //
                    );

        for(uint64_t j=0; j < diag_size; j++) {  //For each element in the diagonal
            uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position
            M[vect_pos] = diag_values[j];
        }
        diag_size--;
    }

    if(mpiRank==0){
        uint64_t diag_size = N - threshold;
        for(uint64_t i=threshold; i < N; i++) {  //For each diagonal after threshold
            for(uint64_t j=0; j < diag_size; j++) {  //For each element in the diagonal
                uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position
                double dp = 0.0;

// #pragma omp parallel for reduction(+:dp)
                for(uint64_t k=0; k < i; k++) {
                    dp += (M[vect_pos - k - 1] * M[vect_pos + ((k + 1) * N)]);
                }
                dp = std::cbrt(dp);
                M[vect_pos] = dp;
            }
            diag_size--;
        }
    }

    endGlobal = MPI_Wtime();
    if (mpiRank == 0) {
		std::cout << (endGlobal-startGlobal) << "s" <<  std::endl;
    }

    #if 1   //Print matrix
    if(mpiRank==0){
                std::printf("\n");
                for(uint64_t i=0; i < N; i++){
                    for(uint64_t j=0; j < N; j++){
                        std::printf("%f ",M[i*N + j]);
                    }
                std::printf("\n");
                }
    }
    #endif

    MPI_Finalize();
    return 0;
}
