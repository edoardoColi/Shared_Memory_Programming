#include <mpi.h>
#include <iostream>

/*
MPI_Gather(
    const void* sendbuf,    //the element to send
    int sendcount,          //number of element to send
    MPI_Datatype sendtype,  //type of element
    void* recvbuf,          //buffer to receive
    int recvcount,          //number of elements to receive from each process
    MPI_Datatype recvtype,  //type of element
    int root,               //rank of the process that have to receive
    MPI_Comm comm           //communicator
    )
*/
int main(int argc, char* argv[])
{
    int sendbuf[4] = {1,2,3,4};

    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    int recvbuf[4*world_size];

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    //MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)
    MPI_Gather(sendbuf, 3, MPI_INT, recvbuf, 4, MPI_INT, 1, MPI_COMM_WORLD);
        /*Il sendcount non puo esere maggiore della dimensione del sendbuffer ma puo essere minore, tuttavia
        il processo root si aspetta di ricevere tutto il buffer di partenza. Per questo motivo oltre a dimensionare
        il recvbuf in modo appropriato bisogna ricordare che il processo root disporra in questo modo i dati che
        riceve: [expectedFrom0-0, expectedFrom0-1, expectedFrom0-2, expectedFrom0-3, expectedFrom1-0, ...] supponendo un sendbuf da 4.
        Se vengono inviati meno dati dell'originale, supponiamo 3, allora la parte expectedFrom*-3 risultera' casualmente riempita.
        Il processo root riceve anche i dati che si manda da solo come se facesse parte dei senders.*/

    if(world_rank == 1)
    {
        std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") recived:" << std::endl;
        for (int i=0; i<4*world_size; i++)
        {
            std::cout << " " << recvbuf[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}