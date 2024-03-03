#include <mpi.h>
#include <iostream>

/*
MPI_Scatter(
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
    int sendbuf[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    int recvbuf[4];

    MPI_Init(&argc, &argv);   //Initialize the MPI environment //TODO perche qui non (NULL, NULL) ??

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    //Get the number of processes

    if (world_size != 4)
    {
        std::cerr << "World size must be 4 for this example, actually is " << world_size << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);    //Get the number of process

    //MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)
    MPI_Scatter(sendbuf, 5, MPI_INT, recvbuf, 6, MPI_INT, 0, MPI_COMM_WORLD);
    /*Il nodo di root comprende se stesso per dividere i dati e se ne invia una perte.Il counter degli
    elementi ricevuti deve essere almeno della dimensione di quelli mandati per funzionare. Quando vado a decidere quanti elementi mandare
    posso mettere qualsiasi numero, anche se non stanno nel recvbuf, vengono salvati solo quelli per cui c'e spazio. Poiche facendo
    cosi finiranno prima i valori, se rimane da mandare solo un numero e ne devono essere inviati N ne viene inviato
    solo 1 (solo fino a svuotare il sendbuf)*/

    std::cout << "Process (rank " << world_rank+1 << "/" << world_size << ") recived: " << recvbuf[0] << ", " << recvbuf[1] << ", " << recvbuf[2] << ", " << recvbuf[3] <<std::endl;

    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}