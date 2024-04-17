#include <cstdint>  // uint64_t
#include <iostream> // std::cout std::endl
#include <vector>   // std::vector
#include <thread>   // std::thread

// this function will be called by the threads (should be void)
void say_hello(uint64_t id) {		//SWAP
	std::cout << "Hello from thread: " << id << std::endl;		//SWAP
}		//SWAP

// // This when i want pass by reference, must update under
// void say_hello(uint64_t & id) {		//SWAP
// 	std::cout << "Hello from thread: " << id << std::endl;		//SWAP
// }		//SWAP

// this runs in the master thread
int main(int argc, char * argv[]) {
	
	const uint64_t num_threads{(argc==2)?std::stoul(argv[1]):10};
	std::vector<std::thread> threads;
	
	// for all threads
	for (uint64_t id = 0; id < num_threads; id++)
		// emplace the thread object in vector threads
		// using argument forwarding, this avoids unnecessary
		// move operations to the vector after thread creation
		threads.emplace_back(
							 // call say_hello with argument id
							 say_hello, id		//SWAP
							// say_hello, std::ref(id)	//SWAP need this if want to use by referenc function, must update above
							 );
	
	// join each thread at the end
	for (auto& thread: threads)
		thread.join();
}

