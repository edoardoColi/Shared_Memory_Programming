# Releted to Lesson 18-2h and 19-1h+2h and 20-1h. Slides 'FastFlow'
To show and practice on how use FastFlow.
## Here it is a structure of the example folder
```
example/
│
├── onenode.cpp                     (Example of network with only one node. Show how works svc_init() e svc_end() works)
├── pipe_square.cpp                 (Three stage pipeline that compute the sum of the square of the numbers)
├── termination_with_loop.cpp       (Example of )
|
├── example-pipe.cpp                (Example of computing dot product. Not known the size and elements of vectors)
├── example-farm.cpp                (Farm in the middle stage with default sink and collector)
├── example-farm2.cpp               (Farm sink is molti-input, and collector removed)
├── example-farm3.cpp               (As before components but builded in a different way)
├── example-farm3-ordered.cpp       (No on-demand shedule but round-robin, can change in the code. To have ordered farm)
├── example-farm-ondemand.cpp       (Farm with a real on-demand policy by redefining the emitter)
├── example-farm4.cpp               (Change less possible, we want aggregate components to reduce threads using combine(third, first) stages)
├── example-farm5.cpp
├── example-a2a.cpp                 (2 stages, left and right workers)
|
├── example-seq.cpp
|
├── example-omp.cpp
├── example-omp2.cpp
|
└── Makefile
```
