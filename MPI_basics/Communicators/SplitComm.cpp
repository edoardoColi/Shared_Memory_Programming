#include <mpi.h>
#include <iostream>

/*
Creates new communicators by "splitting" a communicator into a group of sub-communicators based on the input values color and key.
MPI_Comm_split(
        MPI_Comm comm,      //communicator to split
        int color,          //determines to which new communicator each process will belong. Same color same communicator
        int key,            //determ the order(rank) within each new communicator. The smallest key value is assigned to 0, next to 1 and so on 
        MPI_Comm* newcomm   //new communicator
        )
*/
int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    //In the following code we suppose to have 10 processes and we want to split them in sub groups "rows" as they are a 2x5 Matrix

    int color = world_rank / 2;         //Determine color based on row

    MPI_Comm row_comm;          //To free
    //MPI_Comm_split(comm, color, key, newcomm)
    MPI_Comm_split(MPI_COMM_WORLD, color, -world_rank, &row_comm);

    int row_size;
    MPI_Comm_size(row_comm, &row_size);    //Get the number of processes related to the communicator
    int row_rank;
    MPI_Comm_rank(row_comm, &row_rank);    //Get the number of process related to the communicator

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") is splitted to sub-group " << row_rank+1 << "/" << row_size << "-" << color << " (row/size-color)" << std::endl;

    MPI_Comm_free(&row_comm);           //We have to free the communicator that we created above

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}