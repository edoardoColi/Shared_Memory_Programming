# Releted to Lesson 6-2h. Slides 'Shared Memory 2'
Vectorization for cpp code. Can be also the compilar to create vectorized code automatically. Can be done with flags:
- `-O3` (`-O2` in recent GCC version)
- `-ftree-vectorize` (enable vectorization optimizations)
- `-ffast-math` (often do the trick to activate auto-vectorization)
- `-mavx2` (often do the trick to activate auto-vectorization)
- `-march=native` (often do the trick to activate auto-vectorization)

To know if a compiler vectorized a loop or not we can ask to compiler a report using flag `-fopt-info-<option>`.  
`-fopt-info-vec-all` enable dumps for all vectorization optimization. Can get more info [here](https://gcc.gnu.org/onlinedocs/gcc/Developer-Options.html#index-fopt-info)

## Here it is a structure of the example folder
```
example/
│
├── include/
│   └── hpc_helpers.hpp
│
├── vectorization-simple/
│   ├── f.cpp
│   ├── f.hpp
│   ├── utimer.hpp
│   ├── one.cpp                     (Toy example to see what compiler is able to vectorize)
│   ├── two.cpp                     (Toy example to see what compiler is able to vectorize)
│   ├── three.cpp                   (Toy example to see what compiler is able to vectorize)
│   └── Makefile
│
├── vector_norm_aos_plain.cpp       (Normal sequential computation execution)
├── vector_norm_aos_avx.cpp         (Using AVX instruction for use Gpu)
├── vector_norm_soa_plain.cpp       (Normal sequential computation execution)
├── vector_norm_soa_avx.cpp         (Using AVX instruction for use Gpu)
│
├── plain_vector_max.cpp            (Computes the maximum of a given array of floating-point numbers)
│
├── false_sharing.cpp
├── launch-benchmark.cpp
├── matrix_mult_avx.cpp
├── matrix_mult.cpp
└── Makefile
```
aos: Array of Structure  
soa: Structure of Array  
