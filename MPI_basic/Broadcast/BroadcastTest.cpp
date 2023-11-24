#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

/*
MPI_Bcast(
    void* buffer,           //the element to send
    int count,              //number of element to send
    MPI_Datatype datatype,  //type of element
    int root,               //rank of the process that have to sand value to others
    MPI_Comm comm           //communicator
    )
*/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int recvbuf[4*world_size];

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    long long offset;
    
    std::srand(std::time(0) + world_rank);
    offset = std::rand() % 100;                     //Give a random value to initialize element of each process 

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") recived: " << offset << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);                    //Used to not mess with the prints

    //MPI_Bcast(buffer, count, datatype, root, comm)
    MPI_Bcast(&offset, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    /*Posso utilizzare BroadCast anche con un solo nodo, invia a se stesso. Non posso usilizzare come root un nodo con rank maggiore della size.*/

    std::cout <<"Process (rank " << world_rank+1 << "/" << world_size << ") recived: " << offset << std::endl;

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}