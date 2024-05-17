// In una farm e' importante lo scheduling. E' importante il
// nondeterminismo che si introduce parallelizzando le cose, ma
// a noi ora non interessa l'ordine.
//===========================================================================================
// Parallel code of the second SPM Assignment a.a. 23/24
// Compile using:
// g++ -std=c++20 -I. -I /home/torquati/fastflow -Wall -O3 -DNDEBUG -ffast-math -o wc Word-Count-ff.cpp -pthread
//
// Execute with:
// ./wc /opt/SPMcode/A2/filelist.txt 0 5 1
//
// Schema:
//                   |────> Worker ──|
//         Emitter ──|────> Worker ──├───|
//           ^       |────> Worker ──|   |
//			 |                           |
//           └───────────────────────────|
//

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
using namespace ff;

using ull = unsigned long long;

// see http://en.wikipedia.org/wiki/Primality_test
static inline bool is_prime(ull n) {
    if (n <= 3)  return n > 1; // 1 is not prime !
    
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (ull i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) 
            return false;
    }
    return true;
}

struct Task_t {
    Task_t(ull n1, ull n2):n1(n1),n2(n2) {}
    const ull n1;
	const ull n2;
    std::vector<ull> V;
};

// generates the numbers
struct Emitter: ff_monode_t<Task_t> {

    Emitter(ull n1, ull n2)
        :n1(n1),n2(n2) {
		results.reserve( (size_t)(n2-n1)/log(n1) );
    }

    Task_t *svc(Task_t *in) {
        if (in == nullptr) { // first call //MIO La freccia in ingresso dall'esterno
			const int nw = get_num_outchannels(); // gets the total number of workers added to the farm
			const size_t  size = (n2 - n1) / (nw+1);
			ssize_t more = (n2-n1) % (nw+1);
			ull start = n1+size;
			ull stop  = start;
			
			for(int i=1; i<=nw; ++i) {
				start = stop;
				stop  = start + size + (more>0 ? 1:0);
				--more;
				
				Task_t *task = new Task_t(start, stop);
				ff_send_out_to(task, i-1);
			}
			// broadcasting the End-Of-Stream to all workers
			broadcast_task(EOS);
			
			// now compute the first partition (n1, n1+size) locally
			start = n1; stop = n1 + size;
			for(ull i=start; i<stop; ++i)
				if (is_prime(i)) results.push_back(i);
	    
			return GO_ON;  // keep me alive 
        }
		// someting coming back from Workers
		auto &V = in->V;
		if (V.size())  // We may receive an empty vector 
            results.insert(std::upper_bound(results.begin(), results.end(), V[0]),
						   V.begin(), V.end());
		delete in;
        return GO_ON;
    }
    // these are the edge values of the range
    ull n1,n2; 
    // this is where we store the results coming back from the Workers
    std::vector<ull> results; 
};
struct Worker: ff_node_t<Task_t> {
    Task_t *svc(Task_t *in) {
		auto& V   = in->V;
		ull   n1 = in->n1;
		ull   n2 = in->n2;
		ull   prime;
		while( (prime=n1++) < n2 )  if (is_prime(prime)) V.push_back(prime);
        return in;
    }
};

void printOutput(const std::vector<ull>& results, const bool print_primes) {
    const size_t n = results.size();
    std::cout << "Found " << n << " primes\n";
    if (print_primes) {
        for(size_t i=0;i<n;++i) 
            std::cout << results[i] << " ";
        std::cout << "\n\n";
    }
    std::cout << "Time: " << ff::ffTime(ff::GET_TIME) << " (ms)\n";
}

int main(int argc, char *argv[]) {    
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " number1 number2 nworkers [print=off|on]\n";
        return -1;
    }
    ull n1          = std::stoll(argv[1]);
    ull n2          = std::stoll(argv[2]);
    const size_t nw = std::stol(argv[3]);
    bool print_primes = false;
    if (argc >= 5)  print_primes = (std::string(argv[4]) == "on");
    
    if (nw == 1) {
		ffTime(START_TIME);
		std::vector<ull> results;
		results.reserve((size_t)(n2-n1)/log(n1));
		ull prime;
		while( (prime=n1++) <= n2 ) 
			if (is_prime(prime)) results.push_back(prime);
		ffTime(STOP_TIME);
		printOutput(results, print_primes);		
    } else {
		ffTime(START_TIME);
		ff_Farm<> farm([&]() {
			std::vector<std::unique_ptr<ff_node> > W;
			for(size_t i=0;i<(nw-1);++i)
				W.push_back(make_unique<Worker>());
			return W;
	    } () ); // by default it has both an emitter and a collector
		
		Emitter E(n1, n2);        // creating the Emitter
		farm.add_emitter(E);      // replacing the default emitter
		farm.remove_collector();  // removing the default collector
		farm.wrap_around();       // adding feedback channels between Workers and the Emitter
		
		if (farm.run_and_wait_end()<0) {
			error("running farm\n");
			return -1;
		}
		ffTime(STOP_TIME);
		printOutput(E.results, print_primes);
		std::cout << "farm Time: " << farm.ffTime() << " (ms)\n";
    }
    
    return 0;
}

//
// This example shows how to use the all2all (A2A) building block (BB).
// It finds the prime numbers in the range (n1,n2) using the A2A.
//
//          L-Worker --|   |--> R-Worker --|
//                     |-->|--> R-Worker --|
//          L-Worker --|   |--> R-Worker --|  
//      
//
//  -   Each L-Worker manages a partition of the initial range. It sends sub-partitions
//      to the R-Workers in a round-robin fashion.
//  -   Each R-Worker checks if the numbers in each sub-partition received are
//      prime by using the is_prime function
//

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <ff/ff.hpp>
#include <ff/all2all.hpp>
using namespace ff;

using ull = unsigned long long;

// see http://en.wikipedia.org/wiki/Primality_test
static inline bool is_prime(ull n) {
    if (n <= 3)  return n > 1; // 1 is not prime !
    
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (ull i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) 
            return false;
    }
    return true;
}

struct Task_t {
    Task_t(ull n1, ull n2):n1(n1),n2(n2) {}
    const ull n1, n2;
};

// generates the numbers
struct L_Worker: ff_monode_t<Task_t> {  // must be multi-output

    L_Worker(ull n1, ull n2)
        : n1(n1),n2(n2) {}

    Task_t *svc(Task_t *) {

		const int nw = 	get_num_outchannels();
		const size_t  size = (n2 - n1) / nw;
		ssize_t more = (n2-n1) % nw;
		ull start = n1, stop = n1;
	
		for(int i=0; i<nw; ++i) {
			start = stop;
			stop  = start + size + (more>0 ? 1:0);
			--more;
	    
			Task_t *task = new Task_t(start, stop);
			ff_send_out_to(task, i);
		}
		return EOS;
    }
    ull n1,n2; 
};
struct R_Worker: ff_minode_t<Task_t> { // must be multi-input
    R_Worker(const size_t Lw):Lw(Lw) {}
    Task_t *svc(Task_t *in) {
		ull   n1 = in->n1, n2 = in->n2;
		ull  prime;
		while( (prime=n1++) < n2 )  if (is_prime(prime)) results.push_back(prime);
        return GO_ON;
    }
    std::vector<ull> results;
    const size_t Lw;
};

int main(int argc, char *argv[]) {    
    if (argc<5) {
        std::cerr << "use: " << argv[0]  << " number1 number2 L-Workers R-Workers [print=off|on]\n";
        return -1;
    }
    ull n1          = std::stoll(argv[1]);
    ull n2          = std::stoll(argv[2]);
    const size_t Lw = std::stol(argv[3]);
    const size_t Rw = std::stol(argv[4]);
    bool print_primes = false;
    if (argc >= 6)  print_primes = (std::string(argv[5]) == "on");
	
    ffTime(START_TIME);
	
    const size_t  size  = (n2 - n1) / Lw;
    ssize_t       more  = (n2-n1) % Lw;
    ull           start = n1;
	ull           stop  = n1;

    std::vector<ff_node*> LW;
    std::vector<ff_node*> RW;
    for(size_t i=0; i<Lw; ++i) {
		start = stop;
		stop  = start + size + (more>0 ? 1:0);
		--more;
		LW.push_back(new L_Worker(start, stop));
    }
    for(size_t i=0;i<Rw;++i)
		RW.push_back(new R_Worker(Lw));
	
    ff_a2a a2a;
    a2a.add_firstset(LW, 1); //, 1 , true);
    a2a.add_secondset(RW); //, true);
    
    if (a2a.run_and_wait_end()<0) {
		error("running a2a\n");
		return -1;
    }
	
    std::vector<ull> results;
    results.reserve( (size_t)(n2-n1)/log(n1) );
    for(size_t i=0;i<Rw;++i) {
		R_Worker* r = reinterpret_cast<R_Worker*>(RW[i]);
		if (r->results.size())  
            results.insert(std::upper_bound(results.begin(), results.end(), r->results[0]),
						   r->results.begin(), r->results.end());
    }
	if (Lw>1) // results must be sorted
		std::sort(results.begin(), results.end());
	ffTime(STOP_TIME);
    
    // printing obtained results
    const size_t n = results.size();
    std::cout << "Found " << n << " primes\n";
    if (print_primes) {
		for(size_t i=0;i<n;++i) 
			std::cout << results[i] << " ";
		std::cout << "\n\n";
    }
    std::cout << "Time: " << ffTime(GET_TIME) << " (ms)\n";
    std::cout << "A2A Time: " << a2a.ffTime() << " (ms)\n";
	
    return 0;
}
