/* Parallel code of the SPM project a.a. 23/24
Compile using:
g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o wff Wf_ff.cpp -pthread
*/

#include <iostream>
#include <vector>
#include <cmath>

#include <ff/ff.hpp>
#include <ff/farm.hpp>

using namespace ff;

struct Task_t {
    Task_t(uint64_t d, uint64_t f, uint64_t t): diagonal(d), from_elem(f), to_elem(t), completed(false) {}
    const uint64_t diagonal;
    const uint64_t from_elem;
    const uint64_t to_elem;
    bool completed;
};

struct Emitter: ff_monode_t<Task_t> {
    Emitter(uint64_t sizeMatrix) : sizeM(sizeMatrix) {}

    Task_t *svc(Task_t *in) {
        if (in != nullptr) { //Not first call
            if (in->completed){
                done++;
            } else {
                ff_send_out(in);
            }
        }

        if (send == done) {
            send = 0;
            done = 0;
            diag++;

            if (diag < sizeM){
                uint64_t diag_size = sizeM - diag;
                uint64_t from = 0; //mpiRank * task_size;
                uint64_t to = diag_size; //((mpiRank + 1) * task_size < diag_size ? (mpiRank + 1) * task_size : diag_size);
                Task_t *task = new Task_t(diag, from, to);
                send++;
                ff_send_out(task);
            } else {
                broadcast_task(EOS);
                return EOS; //TODO sicuro non GO_ON
            }
        }
        if (in != nullptr)
            delete in;
        return GO_ON;   //Keep me alive 
    }

    uint64_t sizeM;
    const uint64_t nw = get_num_outchannels(); //Gets the total number of workers added to the farm;
    uint64_t send = 0;
    uint64_t done = 0;
    uint64_t diag = 0;
}

struct Worker: ff_node_t<Task_t> {
    Worker(std::vector<double> &M, uint64_t sizeMatrix) : M(M), sizeM(sizeMatrix) {}

    Task_t *svc(Task_t *in) {
        auto& done   = in->completed;
        uint64_t i = in->diagonal;
        uint64_t from = in->from_elem;
        uint64_t to = in->to_elem;

        for(uint64_t j=from; j < to; j++) {  //For the assigned elements of the diagonal
            uint64_t vect_pos = (j * (N + 1)) + i;  //Absolute position
            double dp = 0.0;    //Dot product

            for(uint64_t k=0; k < i; k++) {
                dp += (M[vect_pos - k - 1] * M[vect_pos + ((k + 1) * N)]);
            }

            dp = std::cbrt(dp);
            M[vect_pos] = dp;
        }
        
        done = true;
        return in;
    }

    std::vector<double> &M;
    uint64_t sizeM;
};

int main(int argc, char *argv[]) {

    if (argc != 1 && argc != 2 && argc != 3) {
        std::cout << "use " << argv[0] << " <MatrixSize> <NumWorkers>" << std::endl;
        return -1;
    }

    uint64_t N = 512;   //Default value
    if (argc > 1) {
        N = std::stol(argv[1]);
    }
    uint64_t Nw = 1;    //Default value
    if (argc > 2) {
        Nw = std::stol(argv[2]);
    }
    std::cout << "Parallel <MatrixSize>,<Workers>" << std::endl << "Wavefront(" << N << "," << Nw << ") --> ";

    std::vector<double> M(N*N, 0);  //Filled with 0

    auto init=[&]() {
        for(uint64_t i=0; i < N; i++) {    //For each element in the major diagonal
            double m = (i + 1.);// / N;
            M[i*N + i] = m;
        }
    };
    init();

    ffTime(START_TIME);
    ff_Farm<> farm([&]() {
        std::vector<std::unique_ptr<ff_node> > W;
        for(size_t i=0;i<(Nw);++i)
            W.push_back(make_unique<Worker>(M,N));
        return W;
    } () );
    
    Emitter E(N);               //Creating the Emitter
    farm.add_emitter(E);        //Replacing the default emitter
    farm.remove_collector();    //Removing the default collector
    farm.wrap_around();         //Adding feedback channels between Workers and the Emitter
    
    if (farm.run_and_wait_end() < 0) {
        error("running farm\n");
        return -1;
    }
    ffTime(STOP_TIME);
    std::cout << ffTime(GET_TIME)/1000 << "s" << std::endl;

    #if 1   //Print matrix
    std::printf("\n");
    for(uint64_t i=0; i < N; i++){
        for(uint64_t j=0; j < N; j++){
            std::printf("%f ",M[i*N + j]);
        }
    std::printf("\n");
    }
    #endif

    return 0;
}
