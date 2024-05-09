# Releted to Lesson 21-1h+2h. Slides 'FastFlow'
Fastflow examples analyzing CPU affinity, concurrency control.  
Construct in FastFlow as parallel_reduce that try emulate the behaviour of openmp.  
  
ff_chol_mdf: The block-based Cholesky factorization algorithm is implemented in this file, requires the Itel MKL library. However, it is possible to compile the code with "fake" macro operations (i.e., without the MKL), just compile with the flag -DDO_NOTHING.  
For real exacution of the algorithm (i.e., once installed the MKL) then chol1024.bin is a valid input of a matric having size 1024x1024 in binary format.  
  
- dac is Divide and Conquer.  
- mdf is Macro Data Flow.  
## Here it is a structure of the example folder
```
example/
│
├── farm_pinning.cpp                (To measure thread affinity using FastFlow)
├── primes_parallelfor.cpp          (Previous solved with the omp pragma. Here we use FastFlow with a2a)
├── ff_pi.cpp                       (Previous solved with the omp pragma. Here we use FastFlow with parallel reduce or parallel for reduce)
├── strassen_mdf.cpp                (Strassen algorithm implementation use as macro data-flow example)
├── ff_chol_mdf.cpp                 (Cholesky factorization is a numerical problem. Each level (leftlooking) has less parallelism so good schedule of macro instruction is mandatory for performances)
├── mergesort_dac_ptr.cpp           (Example of merge with dac in FastFlow)
|
├── mdf_example.cpp
├── strassen_dac.cpp
├── fib_dac.cpp
├── cho1024.bin
└── Makefile
```










