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
    Task_t(uint64_t d, uint64_t f, uint64_t t): diagonal(d), from_elem(f), to_elem(t), completed(false) {
        // std::cout << "I'm task with diag=" << d << " from=" << f << " to=" << t << std::endl;
    }
    const uint64_t diagonal;
    const uint64_t from_elem;
    const uint64_t to_elem;
    bool completed;
};

struct Emitter: ff_monode_t<Task_t> {
    Emitter(uint64_t sizeMatrix, uint64_t workers) : sizeM(sizeMatrix), Nw(workers) {}

    Task_t *svc(Task_t *in) {
        if (in != nullptr) { //Not first call
            if (in->completed){
                done++;
            } else {
                ff_send_out(in);
            }
            delete in;
        }

        if (send == done) {
            send = 0;
            done = 0;
            diag++;

            if (diag < sizeM){
                uint64_t diag_size = sizeM - diag;

                // This is the schedule loop for schedule(static)
                uint64_t task_size = diag_size / Nw;
                uint64_t remainder = diag_size % Nw;                                
                for (uint64_t i=0 ; i < Nw; i++){
                    uint64_t from = i * task_size + std::min(i, remainder);
                    uint64_t to = from + task_size + (i < remainder ? 1 : 0);
                    if (from == to) break;

                    Task_t *task = new Task_t(diag, from, to);  //The variable 'to' must be in (0,diag_size]
                    send++;
                    // ff_send_out(task);
                    ff_send_out_to(task, i);
                }
                // // This is the schedule loop for schedule(static,task_size) aka block-cyclic with c=task_size
                // uint64_t task_size = 10;
                // for (uint64_t i=0 ; i < diag_size; i++){
                //     uint64_t from = std::min(i * task_size,diag_size);
                //     uint64_t to = std::min(from + task_size,diag_size);
                //     if (from == to) break;

                //     Task_t *task = new Task_t(diag, from, to);  //The variable 'from' and 'to' must be in (0,diag_size]
                //     send++;
                //     // ff_send_out(task);
                //     ff_send_out_to(task, i % Nw);
                // }

            } else {
                broadcast_task(EOS);
            }
        }
        return GO_ON;   //Keep me alive 
    }

    uint64_t sizeM;
    uint64_t Nw;
    uint64_t send = 0;
    uint64_t done = 0;
    uint64_t diag = 0;
};

struct Worker: ff_node_t<Task_t> {
    Worker(std::vector<double> &M, uint64_t sizeMatrix) : M(M), sizeM(sizeMatrix) {}

    Task_t *svc(Task_t *in) {
        auto& done   = in->completed;
        uint64_t i = in->diagonal;
        uint64_t from = in->from_elem;
        uint64_t to = in->to_elem;

        for(uint64_t j=from; j < to; j++) {  //For the assigned elements of the diagonal
            uint64_t vect_pos = (j * (sizeM + 1)) + i;  //Absolute position
            double dp = 0.0;    //Dot product

            for(uint64_t k=0; k < i; k++) {
                dp += (M[vect_pos - k - 1] * M[vect_pos + ((k + 1) * sizeM)]);
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
            double m = (i + 1.) / N;
            M[i*N + i] = m;
        }
    };
    init();

    ffTime(START_TIME);
    ff_Farm<> farm([&]() {
        std::vector<std::unique_ptr<ff_node> > W;
        for(uint64_t i=0;i<(Nw);++i)
            W.push_back(make_unique<Worker>(M,N));
        return W;
    } () );
    
    Emitter E(N, Nw);           //Creating the Emitter
    farm.add_emitter(E);        //Replacing the default emitter
    farm.remove_collector();    //Removing the default collector
    farm.wrap_around();         //Adding feedback channels between Workers and the Emitter
    // farm.set_scheduling_ondemand();  //On-demand scheduling

    if (farm.run_and_wait_end() < 0) {
        error("running farm\n");
        return -1;
    }
    ffTime(STOP_TIME);
    std::cout << ffTime(GET_TIME)/1000 << "s" << std::endl;

    #if 0   //Print matrix
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
