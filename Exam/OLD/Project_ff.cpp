// Compile using:
// g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o pff Project_ff.cpp -pthread

// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 3 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 5 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 9 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 17 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 22 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 27 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 32 >> esecuzioni ;done
// echo >> esecuzioni ;for i in {1..10};do ./pff 2084 40 >> esecuzioni ;done

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>

#include <ff/ff.hpp>
#include <ff/farm.hpp>


void usage(char* name) {
    std::printf("use: %s N num_workers\n", name);
    std::printf("    N size of the square matrix\n");
    std::printf("    num_workers number of workers spawned\n");
}


using namespace ff;

struct Position {
    uint64_t diagonal;
    uint64_t index;
};



struct Worker: ff_node_t<Position, int> {
    Worker(std::vector<double> &M) : M(M) {}

//quando gli arriva un messaggio fa il dotprod e ritorna uno per far contare the ha finio
    int *svc(Position *pos) { // Process the task

        uint64_t diag= pos->diagonal;
        uint64_t ind= pos->index;
        uint64_t N = std::sqrt(M.size());
        uint64_t current_pos= (ind*(N+1))+diag;
        double result=0;


        for(uint64_t i=0; i< diag; ++i){
            result=result+ ( M[current_pos -i-1]* M[current_pos +((i+1)*N)] ); //sinistra * sotto //TODO dotprod dovrebbe essere questo, giusto?
        }
        // result=std::cbrt(result);
        M[current_pos]=result;



        delete pos;
        ff_send_out(new int(1)); //TODO perche new int? perche ffsendout utilizza puntatori per essere piu flessibile,
                                 //quindi la new alloca un nuovo spazio di memoria per l'intero 1 e restituisce il puntatore ad esso
        return GO_ON;
    }

    std::vector<double> &M; // Reference to the shared matrix
};




struct Source: ff_monode_t<int, Position> {
    uint64_t num_workers;
    uint64_t tasks_sent = 0;
    uint64_t tasks_done = 0;
    uint64_t batches_sent = 0;
    uint64_t max_batches;



    Source(uint64_t num_workers, uint64_t max_batches) : num_workers(num_workers), max_batches(max_batches) {}

    uint64_t N= max_batches;
//questa cosa parte ogni volta che ricevo qualcosa
    Position *svc(int *task) { // Handle the result from the worker

        if(task != nullptr){ //TODO non so perche altrimenti non funziona, comunque ci sta
            tasks_done++;
            delete task;
        }
        

        // Check if all workers have completed their tasks
        if (tasks_done == tasks_sent) {
            // Reset counters
            tasks_sent = 0;
            tasks_done = 0;
            batches_sent++;
            

            if (batches_sent < max_batches) {
                // Send new tasks
                send_tasks();
            } else {
                // Send EOS to workers
                // for (uint64_t i = 0; i < num_workers; ++i) {
                //     ff_send_out(EOS); //TODO come cazzo si fa la broadcast? guardare nei codici del prof? ho guardato anche in assignment 3,
                //                         // non gli piace, non capisco. sara mica quello che dice a inizio slide? di lanciare il file bash
                // }
                broadcast_task(EOS);
                return EOS;
            }
        }
        return GO_ON;
    }

    int svc_init() {
        send_tasks();
        return 0;
    }

    void send_tasks() {
        for (uint64_t i = 0; i < N; ++i) {
            Position * pos = new Position;
            pos->diagonal=(max_batches-N) +1; //numero della diagonale: (dimensione della matrice -1) meno (numero elem della seconda diag) +1
            pos->index=i;
            ff_send_out(pos);
            tasks_sent++;
        }
        N--;
    }
};


//TODO gli int vanno bene o devono essere messi tutti a uint64?






















int main(int argc, char *argv[]) {
    
    uint64_t N = 512;           // Default size of the matrix (NxN)
    uint64_t num_workers = 4;   // Default number of workers
   
    if (argc != 1 && argc != 2 && argc != 3) {
        usage(argv[0]);
        return -1;
    }

    if (argc > 1) {
                N = std::stol(argv[1]);
        if (argc > 2) {
            uint64_t max_workers = std::thread::hardware_concurrency()-1; //-1 perche c'e il Source node
            // std::printf("Maximum number of workers are %ld\n",max_workers); //TODO giusto?
            num_workers = std::stol(argv[2]);
            if(num_workers>max_workers) //TODO qui lo limito, va bene o dobbiamo sforare nei test?
                num_workers=max_workers;
            std::printf("%ld workers will be used\n",num_workers);
        } else{
           std::printf("%ld workers will be used\n",num_workers);
        }
    } else {
        std::printf("Using default values: matrix size=%ld, number of workers=%ld\n", N, num_workers);
    }




    std::vector<double> M(N*N, 0); // Allocate the matrix, default value 0

    // init function
    auto init=[&]() {
            for(uint64_t i = 0; i< N; ++i) { // For each element in the major diagonal
                double t = (i+1);
                // t= t/N;
                M[i*N + i] = t;
            }
        };
    init();

   
#if 0 //stampa la matrice
        for(uint64_t i=0;i<N;++i){
            for(uint64_t j=0; j<N;++j){
                std::printf("%f ",M[i*N+j]); //TODO gli elementi sono double, ok cosi?
            }
        std::printf("\n");
        }
#endif






    Source source(num_workers, (N-1));
    std::vector<std::unique_ptr<ff_node> > workers;
    for (uint64_t i = 0; i < num_workers; ++i) {
        workers.push_back(make_unique<Worker>(M));
    }

    ff_Farm<int> farm(std::move(workers), source);
    farm.remove_collector();
    farm.wrap_around();
    
    // Configure the farm's load balancer to have a fixed-size queue of 1
    // farm.set_scheduling_ondemand(); // Set on-demand scheduling policy
    // farm.setFixedSize(8); // Set the worker's task queue size to 1

    //TODO magari cambiare la policy di scheduling e fare test? i task in teoria sono bilanciati quindi non c'e bisogno di schedule dinamico?

auto start = std::chrono::high_resolution_clock::now(); //TODO il timer va bene questo? lo start va messo sopra riga 205?
    if (farm.run_and_wait_end() < 0) {
        error("running farm");
        return -1;
    }
auto end = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed_seconds = end-start;
std::cout << "tempo usando fastflow: " << elapsed_seconds.count() << std::endl;

#if 0 //stampa la matrice 
        for(uint64_t i=0;i<N;++i){
            for(uint64_t j=0; j<N;++j){
                std::printf("%f ",M[i*N+j]); //TODO gli elementi sono double, ok cosi?
            }
        std::printf("\n");
        }
#endif
    
    return 0;
}
