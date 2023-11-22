#include <mpi.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    double start, end;

    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();                            //Microsecond precision. Can't use time(), because each process will have a different "zero" time

    sleep(30);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if(world_rank == 0)
        std::cout << "Process (rank " << world_rank+1 << "/" << world_size << "): time of work is " << end-start << "seconds" << std::endl;

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}