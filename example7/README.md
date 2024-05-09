# Releted to Lesson 20-2h. Slides 'FastFlow'
To see example of OpenMp and FastFlow of the same problem. In primes_omp is also shown shedule specific for testing performances among `static` and `runtime`.  
In the folder ffc(FastFlowCompressor) we want use it to zip a number of files contained in a directory. Inside there is miniz folder that is a header library for zip files (Example of Data and Stream Parallel).
## Here it is a structure of the example folder
```
example/
│
├── ffc/Z
|   |
|   ├── miniz/
|   |   |
|   |   ├── examples/
|   |   |   |
|   |   |   ├── example1.c
|   |   |   ├── example2.c
|   |   |   ├── example3.c
|   |   |   ├── example4.c
|   |   |   ├── example5.c
|   |   |   └── example6.c
|   |   |
|   |   ├── ChangeLog.md
|   |   ├── examples
|   |   ├── LICENSE
|   |   ├── miniz.c
|   |   ├── miniz.h
|   |   └── readme.md
|   |
|   ├── cmdline.hpp
|   ├── compdecomp.cpp
|   ├── comp.sh
|   ├── datatask.hpp
|   ├── decomp.sh
|   ├── ffc_farm.cpp
|   ├── FFC.pdf
|   ├── reader.hpp
|   ├── utility.hpp
|   ├── worker.hpp
|   ├── writer.hpp
|   ├── Makefile
│   └── README
│
├── primes.cpp
├── primes_omp.cpp                      (Loop for computing is_prime for each element in the range)
├── primes_a2a.cpp                      (Same problem as before. L-workers split in partition the big range for assigning to R-workers)
├── primes_master-worker.cpp            (Same problem as before. Use an Emitter and a pool of workers, Emitter first shedule jobs, leater do the reduce)
└── Makefile
```
