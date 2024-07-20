#Source files
FF_SRC = Wf_ff.cpp
MPI_SRC = Wf_mpi.cpp
SEQ_SRC = Wf_seq.cpp

#Executable names
FF_EXE = wff
MPI_EXE = wfm
SEQ_EXE = wfs

#Targets
all: $(FF_EXE) $(MPI_EXE) $(SEQ_EXE)

$(SEQ_EXE): $(SEQ_SRC)
	g++ -std=c++20 -O3 -march=native -I. -o $@ $^

$(FF_EXE): $(FF_SRC)
	g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o $@ $^ -pthread

#can also compile adding "-fopenmp"
$(MPI_EXE): $(MPI_SRC)
	mpicxx -std=c++20 -I. -Wall  -O3 -o $@ $^

clean:
	rm -fr *.o *~ *.err
	rm -f $(FF_EXE) $(MPI_EXE) $(SEQ_EXE)

cleanall: clean
	rm -fr *.out

re: clean all

.PHONY: all clean cleanall re
