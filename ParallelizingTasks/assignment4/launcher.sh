#!/bin/bash

## specify the name of the job in the queueing system
#SBATCH --job-name=Nkeys

## specify the partition for the resource allocation. if not specified, slurm is allowed to take the default(the one with a star *)
#SBATCH --partition=normal

## format for time is days-hours:minutes:seconds, is used as time limit for the execution duration
#SBATCH --time=3:00:00

## computational nodes to be used
#SBATCH -N 4

## to specify the number of cpus per task on the same node to be used, default is one
#SBATCH --cpus-per-task=1

## to specify the number of tasks to be invoked on each node
#SBATCH --ntasks-per-node=1

## to specify the file of utput and error
#SBATCH --output ./%x.%j.out
#SBATCH --error ./e%x.%j.err

echo "Program executed on: $SLURM_JOB_NODELIST"


# mpirun --report-bindings nkeys-par 4 100 1


# srun --mpi=pmix nkeys-par 2 1000000 0
mpirun --report-bindings nkeys-par 2 1000000 0

# # srun --mpi=pmix nkeys-par 100 1000000 0
# mpirun --report-bindings nkeys-par 100 1000000 0
