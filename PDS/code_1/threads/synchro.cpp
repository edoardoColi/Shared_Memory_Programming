#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

int main(int argc, char * argv[]) {

  int sharedX = 0; 
  atomic<bool> go = false; 

  auto consumer = [&] () {
    cout << "Consumer started " << endl; 
    cout << "sharedX = " << sharedX << endl; 
    while(!go); 
    cout << "sharedX = " << sharedX << endl; 
    while(go);
    cout << "sharedX = " << sharedX << endl; 
    cout << "Consumer ending " << endl; 
    return; 
  }; 

  auto producer = [&] () {
    cout << "Producer started " << endl; 
    cout << "sharedX = " << sharedX << endl; 
    sleep(5); 
    sharedX = 123; 
    go = true; 
    sleep(3); 
    sharedX = 234; go = false; 
    cout << "Producer ending " << endl; 
    return; 
  }; 

  thread tc(consumer); 
  thread tp(producer); 

  tc.join();
  tp.join();
     
  return(0); 
}
  
