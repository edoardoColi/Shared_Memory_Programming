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
    uint64_t threshold = N - mpiSize + 1;
    for(uint64_t i=1; i < threshold; i++) {  //For each diagonal before the threshold

        uint64_t task_size = diag_size / mpiSize;
        uint64_t remainder = diag_size % mpiSize;

        uint64_t from = mpiRank * task_size + std::min(mpiRank, remainder);
        uint64_t to = from + task_size + (mpiRank < remainder ? 1 : 0);

        for(uint64_t j=from; j < to; j++) {  //For the elements of the diagonal
            uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position

            double dp = 0.0;
            for(uint64_t k=0; k < i; k++) {
                dp = dp + (M[vect_pos - k - 1] * M[vect_pos + ((k + 1) * N)]);
            }
            dp = std::cbrt(dp);
            M[vect_pos] = dp;
        }
        diag_size--;
    }





    endGlobal = MPI_Wtime();
	double singleTime = endGlobal-startGlobal;
	double avgTime;
	MPI_Reduce(&singleTime, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (mpiRank == 0) {
		std::cout << (avgTime / mpiSize) << "s" <<  std::endl;
    }

    #if 1   //Print matrix
    if(rank==0){
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
