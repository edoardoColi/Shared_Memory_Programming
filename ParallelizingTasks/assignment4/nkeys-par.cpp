#include <mpi.h>
#include <iostream>
#include <cstdio>
#include <random>
#include <map>
#include <vector>
#include <string>

#ifndef SSTEP
#define SSTEP 0
#endif

// Global variable to control the behavior
const long   SIZE = 64;

long random(const int &min, const int &max) {
	static std::mt19937 generator(117);
	std::uniform_int_distribution<long> distribution(min,max);
	return distribution(generator);
};

void init(auto& M, const long c1, const long c2, const long key) {
	for(long i=0;i<c1;++i)
		for(long j=0;j<c2;++j)
			M[i][j] = (key-i-j)/static_cast<double>(SIZE);
}

// matrix multiplication:  C = A x B  A[c1][c2] B[c2][c1] C[c1][c1]
// mm returns the sum of the elements of the C matrix
auto mm(const auto& A, const auto& B, const long c1,const long c2, MPI_Comm comm) {

    int mpiSize, mpiRank;
    MPI_Comm_size(comm, &mpiSize);
    MPI_Comm_rank(comm, &mpiRank);

    int rows_per_process = c1 / mpiSize;
    int remainder = c1 % mpiSize;

    int start_row = mpiRank * rows_per_process + std::min(mpiRank, remainder);
    int end_row = start_row + rows_per_process + (mpiRank < remainder ? 1 : 0);

    double local_sum = 0;
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < c1; j++) {
            double accum = 0.0;
            for (int k = 0; k < c2; k++) {
                accum += A[i][k] * B[k][j];
            }
            local_sum += accum;
        }
    }

    double sum = 0;
    MPI_Allreduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, comm);

    return sum;
}

// initialize two matrices with the computed values of the keys
// and execute a matrix multiplication between the two matrices
// to obtain the sum of the elements of the result matrix 
double compute(const long c1, const long c2, long key1, long key2, MPI_Comm comm) {

	std::vector<std::vector<double>> A(c1, std::vector<double>(c2,0.0));
	std::vector<std::vector<double>> B(c2, std::vector<double>(c1,0.0));

	init(A, c1, c2, key1);
	init(B, c2, c1, key2);
	auto r = mm(A,B, c1,c2,comm);
	return r;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment

	if (argc < 3) {
		std::printf("use: %s nkeys length [print(0|1)]\n", argv[0]);
		std::printf("     print: 0 disabled, 1 enabled\n");
		return -1;
	}

	long nkeys  = std::stol(argv[1]);  // total number of keys
	// length is the "stream length", i.e., the number of random key pairs
	// generated
	long length = std::stol(argv[2]);
	bool print = false;
	if (argc == 4)
		print = (std::stoi(argv[3]) == 1) ? true : false;

    int mpiSize, mpiRank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

    MPI_Comm odd_comm, even_comm;
    int color = mpiRank % 2;  // Color is 0 for even ranks, 1 for odd ranks

	if (mpiRank == 0)
		std::cout << "SSTEP = " << SSTEP << std::endl;
	if (SSTEP == 1 && mpiSize == 1){
	    MPI_Finalize();
		std::cout << "Execute with more that 1 node" << std::endl;
	    return 0;
	}

    // Split the communicator based on the color
    if (color == 0) {
        MPI_Comm_split(MPI_COMM_WORLD, color, mpiRank, &even_comm);
    } else {
        MPI_Comm_split(MPI_COMM_WORLD, color, mpiRank, &odd_comm);
    }


    double startGlobal, endGlobal;
	startGlobal = MPI_Wtime();

	long key1, key2;

	std::map<long, long> map;
	for(long i=0;i<nkeys; ++i) map[i]=0;

	std::vector<double> V(nkeys, 0);
	bool resetkey1=false;
	bool resetkey2=false;
	for(int i=0;i<length; ++i) {
		key1 = random(0, nkeys-1);  // value in [0,nkeys[
		key2 = random(0, nkeys-1);  // value in [0,nkeys[

		if (key1 == key2) // only distinct values in the pair
			key1 = (key1+1) % nkeys;

		map[key1]++;  // count the number of key1 keys
		map[key2]++;  // count the number of key2 keys

		double r1=0;
		double r2=0;


#if SSTEP
		if (color == 0){
			if (map[key1] == SIZE && map[key2]!=0) {
				r1= compute(map[key1], map[key2], key1, key2, even_comm);
				resetkey1=true;
			}
		}
		if (color == 1){
			if (map[key2] == SIZE && map[key1]!=0) {
				r2= compute(map[key2], map[key1], key2, key1, odd_comm);
				resetkey2=true;
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);        //Blocks the caller until all processes in the communicator have called it
		MPI_Bcast(&resetkey1, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
		MPI_Bcast(&resetkey2, 1, MPI_C_BOOL, 1, MPI_COMM_WORLD);
    	MPI_Bcast(&r1, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	    MPI_Bcast(&r2, 1, MPI_DOUBLE, 1, MPI_COMM_WORLD);
		V[key1] += r1;  // sum the partial values for key1
		V[key2] += r2;  // sum the partial values for key2
#else
		// if key1 reaches the SIZE limit, then do the computation and then
		// reset the counter ....
		if (map[key1] == SIZE && map[key2]!=0) {			
			r1= compute(map[key1], map[key2], key1, key2, MPI_COMM_WORLD);
			V[key1] += r1;  // sum the partial values for key1
			resetkey1=true;			
		}
		// if key2 reaches the SIZE limit ....
		if (map[key2] == SIZE && map[key1]!=0) {			
			r2= compute(map[key2], map[key1], key2, key1, MPI_COMM_WORLD);
			V[key2] += r2;  // sum the partial values for key1
			resetkey2=true;
		}
#endif

		if (resetkey1) {
			// updating the map[key1] initial value before restarting
			// the computation
			auto _r1 = static_cast<unsigned long>(r1) % SIZE;
			map[key1] = (_r1>(SIZE/2)) ? 0 : _r1;
			resetkey1 = false;
		}
		if (resetkey2) {
			// updating the map[key2] initial value before restarting
			// the computation
			auto _r2 = static_cast<unsigned long>(r2) % SIZE;
			map[key2] = (_r2>(SIZE/2)) ? 0 : _r2;
			resetkey2 = false;
		}
	}

	// compute the last values
	for(long i=0;i<nkeys; ++i) {
		for(long j=0;j<nkeys; ++j) {
			if (i==j) continue;
			if (map[i]>0 && map[j]>0) {
				double r1=0;
				double r2=0;
#if SSTEP
				if (color == 0){
					r1= compute(map[i], map[j], i, j, even_comm);
				}
				if (color == 1){
					r2= compute(map[j], map[i], j, i, odd_comm);
				}
				MPI_Barrier(MPI_COMM_WORLD);        //Blocks the caller until all processes in the communicator have called it
				MPI_Bcast(&r1, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
				MPI_Bcast(&r2, 1, MPI_DOUBLE, 1, MPI_COMM_WORLD);
				V[i] += r1;  // sum the partial values for key1
				V[j] += r2;  // sum the partial values for key2
#else
				r1= compute(map[i], map[j], i, j, MPI_COMM_WORLD);
				r2= compute(map[j], map[i], j, i, MPI_COMM_WORLD);
				V[i] += r1;
				V[j] += r2;
#endif
			}
		}
	}

	// printing the results
	if (mpiRank == 0 && print) {
		for(long i=0;i<nkeys; ++i)
			std::printf("key %ld : %f\n", i, V[i]);
	}

	endGlobal = MPI_Wtime();
    if (color == 0) {
		MPI_Comm_free(&even_comm);
	} else {
		MPI_Comm_free(&odd_comm);
	}

	double singleTime = endGlobal-startGlobal;
	double avgTime;
	MPI_Reduce(&singleTime, &avgTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (mpiRank == 0) {
		std::cout << "Full execution time => " << (avgTime / mpiSize) << " (s)" <<  std::endl;
    }
    MPI_Finalize();
    return 0;
}
