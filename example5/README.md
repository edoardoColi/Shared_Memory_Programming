# Releted to Lesson 16-1h+2h and 17-1h. Slides 'OpenMP'
To show and test how use directives of OpenMp. In omp_hello3 is also shown how use timers.
## Here it is a structure of the example folder
```
example/
│
├── include/
|   ├── hpc_helpers.hpp
|   ├── threadPoolWS.hpp
│   └── threadPool.hpp
│
├── omp_hello.cpp               (Example how use openMP directives)
├── omp_hello2.cpp              (Define a lambra function with inside pragma statments to print Hello World)
├── omp_hello3.cpp              (Use in the pragma omp, in clause list, the if statment to discriminate if run parallel or sequential)
├── omp_scope.cpp               (Different outcome for different setup of variables scope)
├── omp_scope2.cpp
├── omp_nested.cpp              (Example of nested OpenMp, remembet to set as true OMP_NESTED)
├── omp_pi.cpp                  (Show how compute pi using reduction on + in omp)
├── omp_schedule.cpp            (Show code for scheduling for loops to threads, can use OMP_SCHEDULE="some"  to test different outcome)
├── omp_all_pair.cpp            (Same problem of previous example but now compute using not explicitly threads but omp directive)
├── omp_sections.cpp            (Example of usage of section directive)
├── omp_fibo.cpp                (Example of )
└── Makefile
```
