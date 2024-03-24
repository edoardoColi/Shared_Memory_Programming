#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

using namespace std;

#define INITTIME   auto start = std::chrono::high_resolution_clock::now();\
  auto elapsed = std::chrono::high_resolution_clock::now() - start;\
  auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\

#define BEGINTIME start = std::chrono::high_resolution_clock::now();
#define ENDTIME(s,nw)   elapsed = std::chrono::high_resolution_clock::now() - start; \
  usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\
  cout << s << "\t" << usec << " usecs with " << nw << " threads " << endl;


int main(int argc, char * argv[]) {

  int n  = atoi(argv[1]);
  int nw = atoi(argv[2]);
  
  INITTIME;
    

  vector<thread*> tid(nw);
  BEGINTIME
  for(int i=0; i<n; i++) {    // repeat it a given number of times
    
    // setup thread doing nothing
    for(int j=0; j<nw; j++)
      tid[j] = new thread(
			  [] (int i) { return;},
			  j);

    // then await all of them
    for(int j=0; j<nw; j++)
      tid[j]->join();

  }
  ENDTIME("raw time", nw);
  
  cout << "Average per thread (fork+join) "
       << ((((float) usec) / ((float) n)) / ((float) nw)) << endl;
  return(0);
}
