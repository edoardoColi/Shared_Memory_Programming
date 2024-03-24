#include <iostream>
#include <vector>
#include <thread>
#include <barrier>


int main(int argc, char * argv[]) {

  int nthread = (argc == 1 ? 4 : atoi(argv[1]));
  std::vector<std::thread> t; 

  auto end_barrier = [] () noexcept {
    std::cout << "Every thread in barrier ... leaving " << std::endl; 
    return; 
  }; 

  std::barrier my_barrier(nthread, end_barrier); 

  auto body1 = [&](int n, int r) {
    std::cout << "This is thread no. " << n << " sleep(" << r << ")" << std::endl;
    sleep(r);
    std::cout << "Thread " << n << " going to enter barrier ..." << std::endl;
    my_barrier.arrive_and_wait();
    std::cout << "Thread " << n << " exiting barrier ..." << std::endl; 
    return;
  };

  std::cout << "Running " << nthread << " threads" << std::endl;
  srand(123);
  for(int i=0; i<nthread; i++) {
    int r = rand() % 8;
    t.push_back(std::thread(body1,i,r));
  }

  for(int i=0; i<nthread; i++)
    t[i].join();
  
  return(0);
}
