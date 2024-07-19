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
        std::cout << "Sequential <MatrixSize>,<Workers>" << std::endl << "Wavefront(" << N << "," << mpiSize << ") --> ";

    std::vector<double> M(N*N, 0);  //Filled with 0

    auto init=[&]() {
        for(uint64_t i=0; i < N; i++) {    //For each element in the major diagonal
            double t = (i+1.)/N;
            M[i*N + i] = t;
        }
    };
    init();

    double startGlobal, endGlobal;
	startGlobal = MPI_Wtime();

    // // Iterate over each diagonal, starting with the (major diagonal +1)
    // for (uint64_t diag = 1; diag < N; ++diag) {
    //     // Number of elements in the current diagonal
    //     uint64_t num_elements = N - diag;

    //     // Calculate the number of elements per process. Over estimation
    //     uint64_t elements_per_proc = (num_elements + size -1 )/ size;

    //     // Define the start and end indices for this process' range
    //     uint64_t start_index = rank * elements_per_proc;
    //     uint64_t end_index = std::min((rank + 1) * elements_per_proc, num_elements);

    //     //std::cout << "DIAG " << diag << " Sono " << rank << " start " << start_index << " end " << end_index << " cioe quanti? " << elements_per_proc << std::endl;



    //     // Create a local buffer for the elements that this process will compute
    //     std::vector<double> local_elements(elements_per_proc, 0);
    //     uint64_t local_index = 0;

    //     // Process each element in the assigned part of the diagonal
    //     for (uint64_t ind = start_index; ind < end_index; ++ind) {
    //         uint64_t current_pos= ind*(N+1) + diag;
    //         double result=0;

    //         for(uint64_t i=0;i<diag;++i){
    //             result=result+( M[current_pos -i-1]* M[current_pos +((i+1)*N)] );
    //         }
    //         result=std::cbrt(result);
    //         //M[current_pos]=result; //in teoria non dovrebbe servire, tanto dopo aggiorno tutta la (diagonale della) matrice

    //         local_elements[local_index++] = result;


    //     }

    //     // Gather all the computed elements from all processes
    //     std::vector<double> all_gathered_elements(elements_per_proc * size, 0);
    //     MPI_Allgather(local_elements.data(), elements_per_proc, MPI_DOUBLE,
    //                   all_gathered_elements.data(), elements_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);

    //     // Each process updates its local matrix with the gathered results
    //     for (uint64_t ind = 0; ind < num_elements; ++ind) {
    //         uint64_t current_pos = ind*(N+1) + diag;
    //         M[current_pos] = all_gathered_elements[ind];
    //     }
    // }

    endGlobal = MPI_Wtime();
	double singleTime = endGlobal-startGlobal;
	double avgTime;
	MPI_Reduce(&singleTime, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (mpiRank == 0) {
		std::cout << (avgTime / mpiSize) << "s" <<  std::endl;
    }

    #if 0   //Print matrix
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
