#include <mpi.h>
#include <iostream>

/*
MPI_Reduce(
    void* send_data,        //the element to apply the reduce
    void* recv_data,        //the reduce result
    int count,              //size of result in recv_data is sizeof(datatype)*count
    MPI_Datatype datatype,  //type of element
    MPI_Op op,              //operation to apply. Can also be defined custom operation
    int root,               //rank of the process that receive result
    MPI_Comm comm           //communicator
    )
*/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    float rand_nums[4] = {1,2,3,42};
    int size = 4;                                   //Related to the correct dize of rand_nums array

    float local_sum = 0;

    for(int i=0; i<size; i++)       //each process compute the sum locally 
        local_sum += rand_nums[i];

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") sum is " << local_sum << " avg is " << local_sum/size << std::endl;

    if(world_rank%2 == 0)       //explicative example part
        local_sum = 1;

    float global_sum;
    //MPI_Reduce(send_data, recv_data, count, datatype, op, root, comm)
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 2, MPI_COMM_WORLD);
    /*Il processo root(suppongo), esegue l'operazione specificata su ogni elemento send_data che gli altri processi hanno riempito*/

    if(world_rank == 2)
        std::cout << " USING REDUCE:" << std::endl << "Process (rank " << world_rank+1 << "/" << world_size << ") sum is " << global_sum << " avg of averages is " << global_sum/(world_size*size) << std::endl;

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}