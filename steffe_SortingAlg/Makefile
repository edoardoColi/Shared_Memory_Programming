# Program for compiling MPI cpp programs
CC		= mpicxx
CXX		= mpicxx
# Extra flags to give to the processor compiler
CFLAGS	= -g -Wall -Werror -Wextra -O3

#
SRC		= main.cpp
OBJ		= $(SRC:.cpp=.o)
NAME	= sort_big_file

# Check if NODES variable is defined
ifeq ($(filter run, $(MAKECMDGOALS)), run)
ifndef NODES
$(error NODES is not defined. Please define NODES before running the target (make run NODES="1-5"))
endif
endif

# SBATCH parameters
JOBNAME	= Distributed_Sorting
PARTITION = production
TIME = 12:00:00
MEM = 3G
NODELIST = steffe[$(NODES)]
CPUS_PER_TASK = 1
NTASKS_PER_NODE = 1
OUTPUT = ./%x.%j.out
ERROR = ./e%x.%j.err

#
.PHONY: all run detail clean fclean cleanall re

.o: .cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: $(NAME)
	@echo "#!/bin/bash" >  launcher.sh; \
	echo "## sbatch is the command line interpreter for Slurm" >> launcher.sh; \
	echo "" >> launcher.sh; \
	echo "## specify the name of the job in the queueing system" >> launcher.sh; \
	echo "#SBATCH --job-name=$(JOBNAME)" >> launcher.sh; \
	echo "## specify the partition for the resource allocation. if not specified, slurm is allowed to take the default(the one with a star *)" >> launcher.sh; \
	echo "#SBATCH --partition=$(PARTITION)" >> launcher.sh; \
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
	echo "mpirun $(NAME) $(ARGS)" >> launcher.sh; \
	chmod +x launcher.sh; \
	echo "The 'launcher.sh' script has been created and is ready to run."; \

	@echo; sbatch launcher.sh
	@echo "  To see job list you can use 'squeue'."
	@echo "  To cancel a job you can use 'scancel jobid'."
	@echo
	@echo "  To visualize binary files in bash can be used:"
	@echo "   od -t d8 -A n binaryfile.bin                    #For in use format"
	@echo "   od -t d8 -A n --endian=little binaryfile.bin    #For little-endian format"
	@echo "   od -t d8 -A n --endian=big binaryfile.bin       #For big-endian format"

detail:
	@echo "Compiler flags and options that mpicxx would use for compiling an MPI program: "
	@mpicxx --showme:compile
	@echo
	@echo "Linker flags and options that mpicxx would use for linking an MPI program: "
	@mpicxx --showme:link

clean:
	@echo rm -f ./e$(JOBNAME).*.err
	@for file in ./e$(JOBNAME).*.err; do \
		rm -f "$$file"; \
	done
	rm -f *~ $(OBJ)

fclean:
	rm -f ./launcher.sh;
	rm -f ./nohup.out
	rm -f /mnt/raid/tmp/SortedRun*
	rm -f $(NAME)

cleanall: fclean clean
	@echo rm -f ./$(JOBNAME).*.out
	@for file in ./$(JOBNAME).*.out; do \
		rm -f "$$file"; \
	done
	rm -f /mnt/raid/tmp/*.sort

re: fclean all
