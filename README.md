# Practice on Parallel Programming

This repository contains various examples and exercises on parallel programming, sourced from university courses at the University of Pisa. The code is organized into directories, each focusing on different parallel programming techniques and paradigms.

### Practiced with
- **MPI (Message Passing Interface)**
- **OpenMP**
- **Threads**
- **FastFlow**

Run `make` to build the examples, but note that some makefiles may not work due to system configurations.

# Slurm
Slurm is a job scheduler designed for high-performance computing clusters, often used in environments with distributed memory architectures. It enables resource allocation and job execution management, and is commonly found in large-scale clusters that run parallel programs.  
The `sbatch` command is used in Slurm to submit batch jobs, specifying resources and job parameters. Both OpenMP and MPI are parallel programming models: OpenMP is for shared memory systems, using threads within the same machine, while MPI handles communication between processes in distributed memory systems.  
A basic sorting algorithm, like merge sort or quicksort, can be parallelized using these tools to improve performance across multiple cores or nodes.
