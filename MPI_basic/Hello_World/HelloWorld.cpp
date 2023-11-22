#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);   //Get the name of the processor

    std::cout << "Hello World from '" << processor_name << "'(lenght:" << name_len << ") (rank " << world_rank+1 << " out of " << world_size << " process)" << std::endl;
    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}