# Program for compiling MPI programs
CC		= mpicc
# Extra flags to give to the processor compiler
CFLAGS	= -g
#  -Wall -Werror -Wextra

#
SRC		= $(wildcard *.c)
OBJ		= $(SRC:.c=.o)
NAME	= distributed_computation

# SBATCH parameters
JOBNAME	= Distributed_Sorting
TIME = 12:00:00
MEM = 3G
NODELIST = steffe[1-20]
CPUS_PER_TASK = 1
NTASKS_PER_NODE = 1
OUTPUT = ./%x.%j.out
ERROR = ./e%x.%j.err

#
.PHONY: all run detail clean fclean re

.o: .c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: $(NAME)
## Se modifico il Makefile cosi rimodifica anche il launcher.sh
#	@if ! [ -f launcher.sh ]; then \

		@echo "#!/bin/bash" >  launcher.sh; \
		echo "" >> launcher.sh; \
		echo "## sbatch is the command line interpreter for Slurm" >> launcher.sh; \
		echo "#SBATCH --job-name=$(JOBNAME)" >> launcher.sh; \
		echo "## format for time is days-hours:minutes:seconds, is used as time limit for the execution duration" >> launcher.sh; \
		echo "#SBATCH --time=$(TIME)" >> launcher.sh; \
		echo "## specify the real memory required per node. suffix can be K-M-G-T but if not present is MegaBytes by default" >> launcher.sh; \
		echo "#SBATCH --mem=$(MEM)" >> launcher.sh; \
		echo "## format for hosts as a range(steffe[1-4,10-15,20]), to specify hosts needed to satisfy resource requirements" >> launcher.sh; \
		echo "#SBATCH --nodelist=$(NODELIST)" >> launcher.sh; \
		echo "## to specify the number of processors per task, default is one" >> launcher.sh; \
		echo "#SBATCH --cpus-per-task=$(CPUS_PER_TASK)" >> launcher.sh; \
		echo "## to specify the number of tasks to be invoked on each node" >> launcher.sh; \
		echo "#SBATCH --ntasks-per-node=$(NTASKS_PER_NODE)" >> launcher.sh; \
		echo "## to specify the file of utput and error" >> launcher.sh; \
		echo "#SBATCH --output $(OUTPUT)" >> launcher.sh; \
		echo "#SBATCH --error $(ERROR)" >> launcher.sh; \
		echo "" >> launcher.sh; \
		echo "mpirun distributed_computation" >> launcher.sh; \
		chmod +x launcher.sh; \
		echo "The 'launcher.sh' script has been created and is ready to run."; \

#	else \
#		chmod +x launcher.sh; \
#	fi
	@echo; sbatch launcher.sh
	@echo "  To see job list you can use 'squeue'."
	@echo "  To cancel a job you can use 'scancel jobid'."
	
detail:
	@echo "Compiler flags and options that mpicc would use for compiling an MPI program: "
	@mpicc --showme:compile
	@echo
	@echo "Linker flags and options that mpicc would use for linking an MPI program: "
	@mpicc --showme:link

clean:
## Sembra non funzionare read
#	read -p "rm: remove all files \"./$(JOBNAME).*.out\" and \"./e$(JOBNAME).*.err\"? (y/n)" choice
#	@if [ "$$choice" = "y" ]; then \

		@echo rm -f ./$(JOBNAME).*.out
		@for file in ./$(JOBNAME).*.out; do \
			rm -f "$$file"; \
		done
		@echo rm -f ./e$(JOBNAME).*.err
		@for file in ./e$(JOBNAME).*.err; do \
			rm -f "$$file"; \
		done

#	fi
	rm -f *~ $(OBJ)

fclean: clean
	@if [ -f launcher.sh ]; then \
		rm -i ./launcher.sh; \
	fi
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
