CXX        = mpicxx -std=c++20
OPTFLAGS   = -O3
CXXFLAGS   += -Wall 
INCLUDES   = -I. -I./include
LIBS       = 
SOURCES    = Project_mpi.cpp
TARGET     = $(SOURCES:.cpp=)

.PHONY: all detail clean cleanall 

%: %.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OPTFLAGS) -o pmpi $< $(LIBS)

all: $(TARGET)

detail:
	@echo "Compiler flags and options that mpicxx would use for compiling an MPI program: "
	@mpicxx --showme:compile
	@echo
	@echo "Linker flags and options that mpicxx would use for linking an MPI program: "
	@mpicxx --showme:link

clean: 
	-rm -fr *.o *~ *.err

cleanall: clean
	-rm -fr *.out
	-rm -fr $(TARGET)
	
# mpicxx -std=c++20 -I. -I./include -Wall -O3 -o wfm Wf_mpi.cpp -fopenmp
