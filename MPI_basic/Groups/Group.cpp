#include <mpi.h>
#include <iostream>

/*
A group is just the set of all processes in the communicator.
A remote operation involves communication with other ranks whereas a local
operation does not. Creating a new communicator is a remote operation
because all processes need to decide on the same context and group, whereas
creating a group is local because it isn’t used for communication and
therefore doesn’t need to have the same context for each process. You can
manipulate a group all you like without performing any communication at all.

MPI_Comm_group(
        MPI_Comm comm,      //context communicator
        MPI_Comm* group     //the group created
        )
MPI_Comm_create_group(
        MPI_Comm comm,      //context communicator
        MPI_Group group,    //group, which is a subset of the group of comm
        int tag,            // ???
        MPI_Comm* newcomm   //the group created (?something like a sub-group?)
        )
MPI_Group_union(
        MPI_Group group1,
        MPI_Group group2,
        MPI_Group* newgroup     //the group created by the union
        )
MPI_Group_intersection(
        MPI_Group group1,
        MPI_Group group2,
        MPI_Group* newgroup     //the group created by the intersection
        )
MPI_Group_incl(
        MPI_Group group,        //group of processes where ranks value are contained, at least
        int n,                  //size of ranks
        const int ranks[],      //ranks to chose
        MPI_Group* newgroup     //group with only the specific ranks from the other group
        )
*/
int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);   //Initialize the MPI environment

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    MPI_Group world_group;          //To free
    //MPI_Comm_group(comm, group)
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    const int ranks[5] = {1,3,5,7,9};               //Just use ranks that are in the group

    MPI_Group spare_group;          //To free
    //MPI_Group_incl(group, n, ranks[], newgroup)
    MPI_Group_incl(world_group, 5, ranks, &spare_group);

    MPI_Comm spare_comm;            //To free
    //MPI_Comm_create_group(comm, group, tag, newcomm)
    MPI_Comm_create_group(MPI_COMM_WORLD, spare_group, 0, &spare_comm);

    int spare_rank = -1, spare_size = -1;           //If this rank isn't in the new communicator, it will be MPI_COMM_NULL. Using MPI_COMM_NULL for MPI_Comm_rank or MPI_Comm_size is erroneous
    if(MPI_COMM_NULL != spare_comm)
    {
        MPI_Comm_size(spare_comm, &spare_size);     //Get the number of processes related to the communicator
        MPI_Comm_rank(spare_comm, &spare_rank);     //Get the number of process related to the communicator
    }

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") is splitted to sub-group " << spare_rank+1 << "/" << spare_size << " (row/size)" << std::endl;

    MPI_Group_free(&world_group);           //We have to free the group that we created above
    MPI_Group_free(&spare_group);           //We have to free the group that we created above
    MPI_Comm_free(&spare_comm);             //We have to free the communicator that we created above

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}