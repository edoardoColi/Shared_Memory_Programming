#include <mpi.h>
#include <iostream>

/*
MPI_Send(
        void* data,              //data buffer
        int count,               //count of elements
        MPI_Datatype datatype,   //type of element
        int destination,         //rank of receiver
        int tag,                 //tag of message
        MPI_Comm communicator    //communicator
        )
MPI_Recv(
        void* data,              //data buffer
        int count,               //count of elements
        MPI_Datatype datatype,   //type of element
        int source,              //rank of receiver
        int tag,                 //tag of message
        MPI_Comm communicator,   //communicator
        MPI_Status* status       //about received message
        )
*/
int main(int argc, char* argv[])
{
    const long long PINGPONG_MAX = 3;

    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    if (world_size % 2 != 0)
    {
        std::cerr << "World size must be pair, actually is " << world_rank+1 << " (odd)" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    long long pingpong_count = 0;
    int partner_rank;
    if (world_rank % 2 == 0)
        partner_rank = (world_rank + 1);
    else
        partner_rank = (world_rank - 1);
    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << "): my partner is " << partner_rank+1 << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0)
        std::cout << "Passed Barrier" << std::endl;

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

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}