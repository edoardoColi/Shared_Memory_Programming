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
    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    long long token;

    if (world_rank != 0)
    {
            // MPI_Recv(data, count, datatype, source, tag, communicator, status)
            MPI_Recv(&token, 1, MPI_LONG_LONG, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);                     //We use MPI_Recv to force not 0 process to wait the token, this because the function is blocking
            std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") received '" << token << "' from " << world_rank << std::endl;
    }
    else
        token = 19;                 //Setup the token value if im processor 0

    // MPI_Send(data, count, datatype, dest, tag, communicator)
    MPI_Send(&token, 1, MPI_LONG_LONG, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);                                   //As first time for process 0, each process after getting the token, send it to the next(+1) process

    //Make process 0 finally get able to retrieve his token after passing it
    if (world_rank == 0)
    {
            // MPI_Recv(data, count, datatype, source, tag, communicator, status)
            MPI_Recv(&token, 1, MPI_LONG_LONG, (world_rank-1)%world_size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);        //To recive the last Send token before ending
            std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") received '" << token << "' from " << (world_rank)%world_size << std::endl;
    }

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}