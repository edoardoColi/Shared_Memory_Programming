# Program for compiling MPI programs
CC		= mpicc
# Extra flags to give to the processor compiler
CFLAGS	= -g
#  -Wall -Werror -Wextra

#
SRC		= $(wildcard *.c)
OBJ		= $(SRC:.c=.o)
NAME	= distributed_computation
#
.PHONY: all run detail clean fclean re

.o: .c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: $(NAME)
	@if ! [ -f launcher.sh ]; then \
		echo "#!/bin/bash" >  launcher.sh; \
		echo "" >> launcher.sh; \
		echo "## sbatch is the command line interpreter for Slurm" >> launcher.sh; \
		echo "#SBATCH --job-name=distributed_sorting" >> launcher.sh; \
		echo "## format for time is days-hours:minutes:seconds, is used as time limit for the execution duration" >> launcher.sh; \
		echo "#SBATCH --time=12:00:00" >> launcher.sh; \
		echo "## specify the real memory required per node. suffix can be K-M-G-T but if not present is MegaBytes by default" >> launcher.sh; \
		echo "#SBATCH --mem=4[G]" >> launcher.sh; \
		echo "## format for hosts as a range(steffe[1-4,10-15,20]), to specify hosts needed to satisfy resource requirements" >> launcher.sh; \
		echo "#SBATCH --nodelist=steffe[1-20]" >> launcher.sh; \
		echo "## to specify the number of processors per task, default is one" >> launcher.sh; \
		echo "#SBATCH --cpus-per-task=1" >> launcher.sh; \
		echo "## to specify the number of tasks to be invoked on each node" >> launcher.sh; \
		echo "#SBATCH --ntasks-per-node=1" >> launcher.sh; \
		echo "" >> launcher.sh; \
		echo "" >> launcher.sh; \
		echo "mpirun distributed_computation" >> launcher.sh; \
		chmod +x launcher.sh; \
		echo "The 'launcher.sh' script has been created and is ready to run."; \
	else \
		chmod +x launcher.sh; \
	fi
	@sbatch launcher.sh; squeue
	@echo "To cancel a job you can use 'scancel jobid'."; \

detail:
	@echo "Compiler flags and options that mpicc would use for compiling an MPI program: "
	@mpicc --showme:compile
	@echo
	@echo "Linker flags and options that mpicc would use for linking an MPI program: "
	@mpicc --showme:link

clean:
	@if [ -f launcher.sh ]; then \
		rm -i ./launcher.sh; \
	fi
	rm -f *~ $(OBJ)

fclean: clean
	rm -f $(NAME)


re: fclean all



# mpicc *.c

# mpirun/mpiexec ...	//will run X copies of the program in the current run-time environment, scheduling(by default) in a round-robin fashion by CPU slot.

# SLIDE 5 Durastante
# The Script
# #!/bin/bash
# #SBATCH --job-name=dascegliere
# #SBATCH --mem=size[unis]
# #SBATCH -n 10
# #SBATCH --time=12:00:00
# #SBATCH --nodelist=lista
# #SBATCH --partition=ports
# #ecc..
# mpirun ...
