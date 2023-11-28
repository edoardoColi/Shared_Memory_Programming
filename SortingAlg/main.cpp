#include <mpi.h>
#include <unistd.h>
#include <sys/stat.h>
#include <queue>
#include <vector>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <iostream>

/*
8-Byte numbers in 1MB = 131072
8-Byte numbers in 1GB = 134217728
*/
#define CACHENUM 131072
#define RAMNUM 268435456

void snowPlowRuns()
{
    std::cout << "Ancora non ci siamo qui" <<std::endl;
}

void heapSortRuns(unsigned long long fileSize, unsigned long long sliceSize, unsigned long long maxLoop, int mpiRank, int mpiSize, FILE* file)
{
    unsigned long long startOffset, endOffset, currentOffset;    //The interval is [startOffset, endOffset)
    int64_t num;
    std::vector<int64_t> bigVect;
    bigVect.reserve(sliceSize);

    for(unsigned long long l = 0; l < maxLoop; l++)     //Populate the vector with the values in the file
    {
        startOffset = sliceSize * (mpiRank + (mpiSize * l));
        if (startOffset > fileSize)
            break;
        endOffset = startOffset + sliceSize;
        fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
        currentOffset = startOffset;
        bigVect.clear();
        while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
        {
            bigVect.push_back(num);
            currentOffset++;
        }

        char templateName[] = "/mnt/raid/tmp/SortedRun_XXXXXX";     //If  absolute path does not exist the temporary file will not be created
        int tmpFile = mkstemp(templateName);     //Create a temporary file based on template
        if (tmpFile == -1)
        {
            std::cout << "Error creating temporary file" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> minHeap(bigVect.begin(), bigVect.end());  //Create a min-heap using priority queue
        for (size_t i = 0; i < bigVect.size(); ++i)     //Write the ordered number in a temp file
        {
            int64_t *buffer;
            buffer = (int64_t*)malloc(1 * sizeof(int64_t));
            buffer[0] = minHeap.top();
            int64_t elem = buffer[0];
            write(tmpFile, &elem, sizeof(int64_t));//TODO controlla scrittura/valore di ritorno
            // if (bytes_written == -1) {
            //     std::cout << "Error writing to file" << std::endl;
            //     MPI_Abort(MPI_COMM_WORLD, 1);
            // }
            minHeap.pop();
        }
            std::cout << templateName << "file empty?-" << lseek(tmpFile, 0, SEEK_END) << std::endl;
        if (lseek(tmpFile, 0, SEEK_END) == 0)
        {//TODO controlla valori di ritorno delle funzioni sotto
            close(tmpFile);
            unlink(templateName);
        }
        close(tmpFile);
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment

    int mpiSize, mpiRank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);    //Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);    //Get the number of process

    if (argc != 2)
    {
        if (mpiRank == 0)
        {
            std::cout << "Usage: " << argv[0] << " <file_to_parse>" << std::endl;
            std::cout << "It returns a file with extension 'TODO' in the same directory of the not-parsed one. Make sure to have space before." << std::endl;
            std::cout << "Use arguments in the make as ARGS=\"stuff\". Example 'make run ARGS=\"/path/to/file\"'." << std::endl;
        }
        return 0;
    }
// std::cout << "Process(" << mpiRank+1 << "/" << mpiSize << "): " << std::endl;

    FILE *file;
    unsigned long long fileSize, slices, sliceSize, maxLoop;
    file = fopen(argv[1], "rb");        //Open the file in mode rb (read binary)
    if(!file)
    {
        std::cout << "Error opening file: " << argv[1] << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    fseek(file,0,SEEK_END);
    fileSize = ftell(file) / 8;    //Size in bytes of the file, correspond to the number of numbers to parse. Each number is 8 bytes
    if (fileSize < (CACHENUM * mpiSize))
        slices = (fileSize / CACHENUM) + 1;
    else if (fileSize < (RAMNUM * mpiSize))
        slices = (fileSize / RAMNUM) + 1;
    else
        slices = mpiSize + 1;
    sliceSize = (fileSize / slices);        //Each process divides a number of 8-byte integers based on the size of the starting file, Attualmente dentro create Runs
    maxLoop = (slices / mpiSize) + 1;

    if (sliceSize > RAMNUM)
        snowPlowRuns();
    else
        heapSortRuns(fileSize, sliceSize, maxLoop, mpiRank, mpiSize, file);
    fclose(file);

    MPI_Barrier(MPI_COMM_WORLD); /*************************************************************/

//TODO k-merge sort

// if(mpiRank==0){
//     std::cout<<"FIN"<<std::endl;
//     unsigned long long startOffset = 0; // Start from the first number (0-based index)
//     unsigned long long endOffset = -1;   // Read up to the X number (exclusive), -1 in the last one because is unsigned
//     FILE *file;
//     int64_t num;

//     file = fopen(argv[1], "rb");
//     fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
//     unsigned long long currentOffset = startOffset;
//     while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
//     {
//         printf("%lld - ", (long long)num);
//         currentOffset++;
//     }
//     printf("EOF\n");
//     fclose(file);

    // std::cout<<"FIN"<<std::endl;
    // for (int64_t n:bigVect)
    //     std::cout << n << std::endl;
    // std::cout<<"EOF"<<std::endl;

// }
    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}

/*TEST

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>

// Function to partition the vector using a random pivot
int partition(std::vector<int64_t>& arr, int low, int high) {
    // Generate a random index between low and high
    int pivotIndex = low + rand() % (high - low + 1);

    // Swap the pivot element with the last element
    std::swap(arr[pivotIndex], arr[high]);

    // Partition the array
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }

    // Swap the pivot element back to its correct position
    std::swap(arr[i + 1], arr[high]);

    return i + 1;
}

// Iterative QuickSort function
void quickSort(std::vector<int64_t>& arr, int low, int high) {
    // Create a stack for iterative approach
    std::vector<std::pair<int, int>> stack;

    // Push initial values of low and high to the stack
    stack.push_back(std::make_pair(low, high));

    // Keep popping from the stack while it is not empty
    while (!stack.empty()) {
        // Pop low and high from the stack
        low = stack.back().first;
        high = stack.back().second;
        stack.pop_back();

        // Get the pivot element such that elements smaller than the pivot
        // are on the left and elements greater are on the right
        int pivot = partition(arr, low, high);

        // If there are elements on the left of the pivot, push them to the stack
        if (pivot - 1 > low) {
            stack.push_back(std::make_pair(low, pivot - 1));
        }

        // If there are elements on the right of the pivot, push them to the stack
        if (pivot + 1 < high) {
            stack.push_back(std::make_pair(pivot + 1, high));
        }
    }
}

// Function to write the sorted vector to a file
void writeToFile(const std::vector<int64_t>& arr, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (int64_t num : arr) {
            outFile << num << " ";
        }
        outFile.close();
        std::cout << "Sorted array written to " << filePath << std::endl;
    } else {
        std::cout << "Unable to open file for writing: " << filePath << std::endl;
    }
}

int main() {
    // Example usage
    std::vector<int64_t> arr = {12, 4, 5, 6, 7, 3, 1, 15};
    int n = arr.size();

    // Set seed for random number generation
    srand(time(0));

    // Perform iterative QuickSort
    quickSort(arr, 0, n - 1);

    // Specify the file path to write the sorted array
    std::string filePath = "sorted_array.txt";

    // Write the sorted array to the specified file
    writeToFile(arr, filePath);

    return 0;
}

*/
