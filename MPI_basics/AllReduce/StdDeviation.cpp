#include <mpi.h>
#include <cmath>
#include <iostream>

/*
MPI_Allreduce(
    void* send_data,        //the element to apply the reduce
    void* recv_data,        //the reduce result
    int count,              //size of result in recv_data is sizeof(datatype)*count
    MPI_Datatype datatype,  //type of element
    MPI_Op op,              //operation to apply. Can also be defined custom operation
    MPI_Comm comm           //communicator
    )
*/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);     //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);     //Get the number of process

    float rand_nums[10] = {1,2,3,42,55,66,71,82,93,190};
    int size = 10;                                  //Related to the correct dize of rand_nums array

    float local_sum = 0;

    for(int i=0; i<size; i++)       //each process compute the sum (locally) 
        local_sum += rand_nums[i];

    std::cout << "STEP1-Process (rank " << world_rank+1 << "/" << world_size << ") sum is " << local_sum << std::endl;

    //Reduce all the local sums into the global sum in order to calculate the mean

    float global_sum;
    //MPI_Allreduce(send_data, recv_data, count, datatype, op, comm)
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    /*Come la Reduce ma senza il processo root perche' il risultato viene distribuito a tutti i processi*/
    float mean = global_sum/(size * world_size);

    float local_sq_diff = 0;
    for(int i=0; i<size; i++)       //each process compute the sum of the squared differences from the mean (locally) 
        local_sq_diff += (rand_nums[i] - mean) * (rand_nums[i] - mean);

    std::cout << "STEP2-Process (rank " << world_rank+1 << "/" << world_size << ") sum of the squared differences from the mean is " << local_sq_diff << std::endl;

    float global_sq_diff;
    MPI_Reduce(&local_sq_diff, &global_sq_diff, 1, MPI_FLOAT, MPI_SUM, 1, MPI_COMM_WORLD);

    if(world_rank == 1)
        std::cout << " FINAL RESULT:" << std::endl << "Process (rank " << world_rank+1 << "/" << world_size << ") mean is " << mean << ", standard deviation is " << sqrt(global_sq_diff/(size*world_size)) << std::endl;

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}