# Releted to Lesson 23-1h+2h and 24-1h. Slides 'MPI e Slurm'
Some example of usage of MPI with gluster as queue handler of the cluster. Showed how use mpirun or srun and how handke MPI librari with the Makefile to compile (the /include and/or the /lib).  
Usage of communicator and tags.  
Usually comunication can be Symmetric or Asymmetric and Synchronous or Asynchronous(Blocking/Non-blocking). Mpi con be used in chosing the tyme of comunication we want for our needs.
## Here it is a structure of the example folder
```
example/
│
├── hello_world.cpp             (Example for print name and rank, first Helloworld MPI program)
├── hello_world.sh
├── send-recv.cpp               (Simple example of sending and reciving from the manual examples)
├── ping_pong_ring_nb.cpp       (Example from the book that emulate ping pong send response in a circular/ring way)
├── trapezoid.cpp               (Split function in intervals and compure area of rectangular unger it, result the integral. In this case function for Pi)
├── master-workers.cpp          (Example of a number of clients in mpi that square the input and give back result until client sent EOS with special tag. Once server sollect all EOS application complete)
├── basic_sbatch.sh
├── osu.sh
└── Makefile
```
