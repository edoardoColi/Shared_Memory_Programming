#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include "utimer.hpp"

using namespace std; 

int main(int argc, char * argv[]) {

  // parameter parsing from command line : mapvec n seed nw
  if(argc == 2 && strcmp(argv[1],"-help")==0) {
    cout << "Usage is: " << argv[0] << " n seed nw printflag" << endl; 
    return(0);
  }

  int n = (argc > 1 ? atoi(argv[1]) : 10);  // len of the vector(s)
  int s = (argc > 2 ? atoi(argv[2]) : 123); // seed 
  int w = (argc > 3 ? atoi(argv[3]) : 4);   // par degree
  bool pf=(argc > 4 ? (argv[4][0]=='t' ? true : false) : true);  
  
  vector<double> v(n),r(n);                 // input and result vectors
  const int max = 8; 
  srand(s); 
  for(int i=0; i<n; i++)                    // random init: same seed to 
    v[i] = (double) (rand() % max);         // run different par degree 

  auto f = [] (double x) { return x*x; };   // function to be mapped on v
  auto g = [] (double x) { for(int i=0; i<1000; i++) x = sin(x); return(x); }; 

  auto body = [&](int k) {		    // body of the worker thread
    int delta = n/w;
    int from  = k*delta;                    // compute assigned iteration
    int to    = (k == (w-1) ? n : (k+1)*delta);  // (poor load balancing)

    {// utimer t1("seq");
    for(int i=from; i<to; i++)              // then apply function to the
      r[i] = FUN(v[i]);                       // assigned vectors
    }
    return; 
  };

  vector<thread*> tids;                     // vector of threads
  long usecs; 
  {
    utimer t0("parallel computation",&usecs); 
    for(int i=0; i<w; i++)                  // create threads
      tids.push_back(new thread(body, i)); 

    for(auto t : tids) t->join();           // await thread completion
  } 
  cout << "End (spent " << usecs << " usecs using " << w << " threads)"  << endl;
  if(pf)                                    // print results (if needed)
    for(int i=0; i<n; i++) cout <<i<<": "<<v[i]<<" -> "<<r[i]<< endl;

  return(0); 
}
