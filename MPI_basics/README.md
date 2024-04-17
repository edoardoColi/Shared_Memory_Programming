# Usage of cluster with slurm

Information about che cluster and partitions could be obtained doing:  
- `sinfo`  
- `scontrol show partition`  

With srun we cen reserve a subset of the resources of the compute nodes for out tasks:  
More parameters are provided in the documentation.  
`srun --partition=production --time=00:30:00 --nodes=1 --pty bash -i` (take for half hour 1 node from the production partition and launch the command 'bash -i')  

To simplify all this expression we can create a basic batch file with all directives and the code to run.  
Is a good thing to redirect the output in a file to save it securely.  
`sbatch run.sh`

Example of batch script
```
 #!/bin/bash
 #SBATCH -n 10
 #SBATCH --time=00:30:00
 ...
 mpirun ./my_exec_code
```
To see the queue of jobs in the system we can run
```
squeue
squeue --long
```

To remove a job from the queue we can run
```
scancel <job/process_id>
```
