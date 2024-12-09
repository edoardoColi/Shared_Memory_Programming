# Releted to Lesson 12-1h+2h. Slides 'C++ Concurrency Basics 2'
Test for Static Data Distribution Policies and also Dynamic.  
Second part on the producers and consumers with std::jthreads, std::condition_variable_any, std::stop_source. Also contain a classic example of reader and writer problem using shared mutex.  
Example of the Thread Pool with an implementation in the hpp file.
## Here it is a structure of the example folder
```
example/
│
├── include/
|   ├── hpc_helpers.hpp
│   └── threadPool.hpp              (Implementation of a Thread Pool)
│
├── all_pair.cpp                    (Compare parallel block cicle with dynamic, by using a chared mutex variable)
├── prod-cons.cpp                   (Lambda function implement a producer, we spawn nprod e ncons passing stoken, used to broadcast message to all can read it so wanna use it for the stop)
├── reader-writer.cpp               (Using C++17 std::shared[_timed]_mutex implement access to multiple thread if only reading)
│
├── squareTP.cpp                    (Compute square of the number putting the task in a Thread Pool)
├── preorder-tree.cpp               (Thread Pool for visiting a tree for computing square of value in the nodes)
├── mnist_dataset.bin.gz
└── Makefile
```
