#include <mpi.h>
#include <iostream>

/*

    MPI_Barrier(MPI_COMM_WORLD);
    while (pingpong_count < PINGPONG_MAX)
    {
        if (world_rank%2 == pingpong_count%2)
        {
            pingpong_count++;
            // MPI_Send(data, count, datatype, dest, tag, communicator)
            MPI_Send(&pingpong_count, 1, MPI_LONG_LONG, partner_rank, 0, MPI_COMM_WORLD);
            std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") sent '" << pingpong_count << "' to " << partner_rank+1 << std::endl;
        }
        else
        {
            // MPI_Recv(data, count, datatype, source, tag, communicator, status)
            MPI_Recv(&pingpong_count, 1, MPI_LONG_LONG, partner_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") received '" << pingpong_count << "' from " << partner_rank+1 << std::endl;
        }
    }

*/
int main(int argc, char* argv[])
{
    const long long PINGPONG_MAX = 3;

    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << "): my partner is " << partner_rank+1 << std::endl;

    if (world_rank == 0)
        std::cout << "Passed Barrier" << std::endl;


    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}