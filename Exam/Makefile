# CXX        = mpicxx -std=c++20
# OPTFLAGS   = -O3
# CXXFLAGS   += -Wall 
# INCLUDES   = -I. -I./include
# LIBS       = 
# SOURCES    = Project_mpi.cpp
# TARGET     = $(SOURCES:.cpp=)

# .PHONY: all detail clean cleanall 

# %: %.cpp
# 	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o pmpi $< $(LIBS)

# all: $(TARGET)

# detail:
# 	@echo "Compiler flags and options that mpicxx would use for compiling an MPI program: "
# 	@mpicxx --showme:compile
# 	@echo
# 	@echo "Linker flags and options that mpicxx would use for linking an MPI program: "
# 	@mpicxx --showme:link

# clean: 
# 	-rm -fr *.o *~ *.err

# cleanall: clean
# 	-rm -fr *.out
# 	-rm -fr $(TARGET)
	

# g++ -std=c++20 -I. -I /home/e.coli3/fastflow -Wall -O3 -DNDEBUG -ffast-math -o wff Wf_ff.cpp -pthread

# mpicxx -std=c++20 -I. -Wall  -O3 -o wfm Wf_mpi.cpp 

# g++ -std=c++20 -O3 -march=native -I. Wf_seq.cpp -o wfs


# Compiler definitions
CXX = g++
MPICXX = mpicxx

# Compiler flags
CXXFLAGS = -std=c++20 -I. -Wall -O3 -fopenmp
FFLAGS = -I /home/e.coli3/fastflow -DNDEBUG -ffast-math -pthread
SEQFLAGS = -march=native

# Source files
FF_SRC = Wf_ff.cpp
MPI_SRC = Wf_mpi.cpp
SEQ_SRC = Wf_seq.cpp

# Executable names
FF_EXE = wff
MPI_EXE = wfm
SEQ_EXE = wfs

# Targets
all: $(FF_EXE) $(MPI_EXE) $(SEQ_EXE)

$(FF_EXE): $(FF_SRC)
	$(CXX) $(CXXFLAGS) $(FFLAGS) -o $@ $^

$(MPI_EXE): $(MPI_SRC)
	$(MPICXX) $(CXXFLAGS) -o $@ $^

$(SEQ_EXE): $(SEQ_SRC)
	$(CXX) $(CXXFLAGS) $(SEQFLAGS) -o $@ $^

clean:
	rm -f $(FF_EXE) $(MPI_EXE) $(SEQ_EXE)

.PHONY: all clean
