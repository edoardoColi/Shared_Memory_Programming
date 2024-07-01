#include <iostream>
#include <mpi.h>
#include <cmath>
#include <vector>
// Compile using the code:
// make
// Run using the code:
// sbatch launcher.sh


void usage(char* name) {
    std::printf("use: %s N\n", name);
    std::printf("    N size of the square matrix\n");
}




int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);  



    uint64_t N = 512;           // Default size of the matrix (NxN)
   
    if (argc != 1 && argc != 2) {
        if(rank==0){ 
            usage(argv[0]);
        }
        MPI_Finalize();
        return -1;
    }

    if (argc > 1) {
                N = std::stol(argv[1]);
    } else {
        if(rank==0){
            std::printf("Using default values: matrix size=%ld\n", N);
        }
    }




    // Each process initializes its matrix independently
    std::vector<double> M(N*N, 0); // Allocate the matrix, default value 0


    // init function
    auto init=[&]() {
        for(uint64_t i = 0; i< N; ++i) { // For each element in the major diagonal
            double t = (i+1);
            t= t/N;
            M[i*N + i] = t;
        }
    };
    init();



    //Start timer
    double start_time= MPI_Wtime();

    // Iterate over each diagonal, starting with the (major diagonal +1)
    for (uint64_t diag = 1; diag < N; ++diag) {
        // Number of elements in the current diagonal
        uint64_t num_elements = N - diag;

        // Calculate the number of elements per process. Over estimation
        uint64_t elements_per_proc = (num_elements + size - 1) / size;

        // Define the start and end indices for this process' range
        uint64_t start_index = rank * elements_per_proc;
        uint64_t end_index = std::min((rank + 1) * elements_per_proc, num_elements);
        // std::cout << "DIAG " << diag << " Sono " << rank << " con " << start_index << " e " << end_index << " e " << elements_per_proc << std::endl;

        // Create a local buffer for the elements that this process will compute
        std::vector<double> local_elements(num_elements, 0);
        // uint64_t local_index = 0;

        if (start_index <= end_index){
            // Process each element in the assigned part of the diagonal
            for (uint64_t ind = start_index; ind < end_index; ++ind) {
                uint64_t current_pos= ind*(N+1) + diag;
                double result=0;

                for(uint64_t i=0;i<diag;++i){
                    result=result+( M[current_pos -i-1]* M[current_pos +((i+1)*N)] );
                }
                result=std::cbrt(result);
                //M[current_pos]=result; //in teoria non dovrebbe servire, tanto dopo aggiorno tutta la (diagonale della) matrice

                local_elements[ind] = result;
            }
        }
        // Print befor the sum for each process
        // std::cout << "Process " << rank << " global sum(PRIMA): ";
        // for (double elem : local_elements) {
        //     std::cout << elem << " ";
        // }
        // std::cout << std::endl;
        // Perform the all-reduce operation, using the same vector for input and output
        MPI_Allreduce(MPI_IN_PLACE, local_elements.data(), num_elements, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        // Print after the sum for each process
        // std::cout << "Process " << rank << " global sum: ";
        // for (double elem : local_elements) {
        //     std::cout << elem << " ";
        // }
        // std::cout << std::endl;

        // // Gather all the computed elements from all processes
        // std::vector<double> all_gathered_elements(num_elements, 0);
        // MPI_Allgather(local_elements.data(), elements_per_proc, MPI_DOUBLE,
        //               all_gathered_elements.data(), elements_per_proc, MPI_DOUBLE, MPI_COMM_WORLD);

        // Each process updates its local matrix with the gathered results
        for (uint64_t ind = 0; ind < num_elements; ++ind) {
            uint64_t current_pos = ind*(N+1) + diag;
            // M[current_pos] = all_gathered_elements[ind];
            M[current_pos] = local_elements[ind];
        }
    }

    //End timer
    double end_time= MPI_Wtime();
    double elapsed_time= end_time - start_time;

    if(rank==0){
        std::printf("Computation time: %f seconds\n",elapsed_time);
    }




    if(rank==0){

    #if 1 //stampa la matrice 
        for(uint64_t i=0;i<N;++i){
            for(uint64_t j=0; j<N;++j){
                std::printf("%f ",M[i*N+j]);
            }
        std::printf("\n");
        }
    #endif
    }

    MPI_Finalize();


    return 0;
}