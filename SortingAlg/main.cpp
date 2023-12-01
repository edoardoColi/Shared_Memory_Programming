#include <mpi.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <queue>
#include <ctime>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>

/*
8-Byte numbers in 256KB = 32768
8-Byte numbers in 1MB = 131072
8-Byte numbers in 1GB = 134217728

All the programm assume numbers as 64_bits.
To visualize binary files in bash can be used:
od -t d8 -A n binaryfile.bin                    #For in use format
od -t d8 -A n --endian=little binaryfile.bin    #For little-endian format
od -t d8 -A n --endian=big binaryfile.bin       #For big-endian format
*/
#define BUFFERSIZE 32768
#define CACHENUM 130000
#define RAMNUM 268435456
#define ALLOW_BUFFER 1
#define ALLOW_SNOWPLOW 1

void sortRuns(unsigned long long fileSize, unsigned long long sliceSize, unsigned long long maxLoop, FILE* file, int id, int mpiRank, int mpiSize)
{
    unsigned long long startOffset, endOffset, currentOffset;    //The interval is [startOffset, endOffset)
    double startTot, start, end;
    int64_t num;
    std::vector<int64_t> bigVect;
    int64_t buffer[BUFFERSIZE];
    bigVect.reserve(sliceSize);

    startTot = MPI_Wtime();                            //Microsecond precision. Can't use time(), because each process will have a different "zero" time
    start = MPI_Wtime();
    for(unsigned long long l = 0; l < maxLoop; l++)     //Populate the vector with the values in the file
    {
        startOffset = sliceSize * (mpiRank + (mpiSize * l));
        if (startOffset >= fileSize)
            break;
        endOffset = startOffset + sliceSize;
        fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
        currentOffset = startOffset;
        bigVect.clear();

        if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
        {
            while (currentOffset < endOffset)
            {
                unsigned long long elementsToRead = std::min(endOffset - currentOffset,  static_cast<unsigned long long>(BUFFERSIZE));      //It's important to check because if the difference between endOffset and startOffset is smaller than BUFFERSIZE we don't have to read further
                unsigned long long elementsRead = fread(buffer, sizeof(int64_t), elementsToRead, file);

                for (unsigned long long i = 0; i < elementsRead; ++i)
                {
                    bigVect.push_back(buffer[i]);
                }
                currentOffset += elementsRead;      //Increment currentOffset based on the number of elements read
                if (elementsRead < BUFFERSIZE)      // Check if we have reached the end of the file
                    break;
            }
        }
        else
        {
            while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
            {
                bigVect.push_back(num);
                currentOffset++;
            }
        }
        end = MPI_Wtime();
        std::cout << "   " << end-start << "s" << " => Time to read file from offset " << startOffset << " to " << endOffset << " in Process " << mpiRank+1  << "/" << mpiSize << " memory" << std::endl;
        start = MPI_Wtime();

        sort(bigVect.begin(), bigVect.end());

        end = MPI_Wtime();
        std::cout << "   " << end-start << "s" << " => Time to sort elements in Process " << mpiRank+1  << "/" << mpiSize << " memory" << std::endl;
        start = MPI_Wtime();

        std::string templateName = "/mnt/raid/tmp/SortedRun" + std::to_string(id) + "_XXXXXX";     //If  absolute path does not exist the temporary file will not be created
        int tmpFile = mkstemp(&templateName[0]);     //Create a temporary file based on template
        if (tmpFile == -1)
        {
            std::cout << "Error creating temporary file" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (unsigned long long i = 0; i < bigVect.size(); ++i)     //Write the ordered number in a temp file
        {
            if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
            {
                buffer[i % BUFFERSIZE] = bigVect[i];
                if ((i + 1) % BUFFERSIZE == 0 || i == bigVect.size() - 1)
                {
                    ssize_t tw = write(tmpFile, buffer, sizeof(int64_t) * ((i % BUFFERSIZE) + 1));
                    if (tw == -1)
                    {
                        std::cout << "Error writing to file" << std::endl;
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                }
            }
            else
            {
                int64_t elem = bigVect[i];
                    ssize_t tw = write(tmpFile, &elem, sizeof(int64_t));
                    if (tw == -1)
                    {
                        std::cout << "Error writing to file" << std::endl;
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
            }
        }
        off_t sz = lseek(tmpFile, 0, SEEK_END);
        if (sz == 0)
        {
            if (close(tmpFile) == -1)
            {
                std::cout << "Error closing the file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            if (unlink(&templateName[0]) == -1)
            {
                std::cout << "Error unlinking file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        if (close(tmpFile) == -1)
        {
            std::cout << "Error closing the file" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        end = MPI_Wtime();
        std::cout << "   " << end-start << "s" << " => Time to write '" << templateName << "' and fill it up with " << sz/8 << " sorted elements by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
        start = MPI_Wtime();
    }
    end = MPI_Wtime();
    std::cout << end-startTot << "s" << " => Time function sortRuns() in Process " << mpiRank+1  << "/" << mpiSize << std::endl;
}

void snowPlowRuns(unsigned long long fileSize, unsigned long long sliceSize, unsigned long long maxLoop, FILE* file, int id, int mpiRank, int mpiSize)
{
    if (ALLOW_SNOWPLOW)
        std::cout << "Can't compute files of size bigger then " << RAMNUM * mpiSize / 134217728 << "Gb with " << mpiSize << " processes (currently file is " << fileSize / 134217728 << "Gb)" << std::endl;
    else
    {
        maxLoop = (fileSize / (RAMNUM * mpiSize)) + 1;
        sortRuns(fileSize, RAMNUM, maxLoop, file, id, mpiRank, mpiSize);
    }
}

void kMerge(const std::string &argFile, int id, int mpiRank, int mpiSize)
{
    std::string fileDir = "/mnt/raid/tmp/";
    std::string pattern = "SortedRun" + std::to_string(id) + "_";
    std::vector<int> fds;       //To store the file descriptor of each file to merge
    std::vector<std::string> fns;     //To store the file name of each file to delete after merge
    size_t lastSlash = argFile.find_last_of('/');
    std::string nameOnly = (lastSlash != std::string::npos) ? argFile.substr(lastSlash + 1) : argFile;
    std::string finalFile = "/mnt/raid/tmp/" + nameOnly + (ALLOW_BUFFER == 1 ? ".buf" : "") + ".sort";
    double start, end;
    int fileCount = 0;

    DIR *dir = opendir(fileDir.c_str());
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (entry->d_type == DT_REG)          //Check if it's a regular file
            {
                std::string filename = entry->d_name;
                if (filename.find(pattern) != std::string::npos) //Check if the file name matches the pattern
                {
                    std::string tmpFile = fileDir + "/" + filename;
                    int fd = open(tmpFile.c_str(), O_RDONLY);        //Open the file and save the file descriptor
                    if (fd != -1)
                    {
                        fds.push_back(fd);
                        fns.push_back(tmpFile);
                        fileCount++;
                    }
                    else
                        std::cout << "Error opening file '" << tmpFile << "' by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "Error opening directory '" << fileDir << "' by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int fdFinal = open(finalFile.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);     //Open the file for writing only, creating it if it doesn't exist and not overwrite if it exists
    if (fdFinal == -1)
    {
        std::cout << "Error opening or creating final file '" << finalFile << "' by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::cout << std::endl << "Starting the merge process for " << fileCount << " files" << std::endl << std::endl;
    start = MPI_Wtime();

    std::priority_queue<std::pair<int64_t, int>, std::vector<std::pair<int64_t, int>>, std::greater<std::pair<int64_t, int>>> minHeap;      //Creating a Min Heap using a priority queue
    int64_t tmpValue;
    for (int fd : fds)    //Populate the Min Heap with initial values from each file descriptor
    {
        if (read(fd, &tmpValue, sizeof(int64_t)) == sizeof(int64_t))
            minHeap.push({tmpValue, fd});
        else
        {
            std::cout << "Error reading from file descriptor by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    int tmpfd;
    int64_t tmpValue2;
    int64_t buffer[BUFFERSIZE];
    unsigned long long i = 0;
    while (!minHeap.empty())    //Write sorted elements to the temporary file
    {
        tmpValue = minHeap.top().first;
        tmpfd = minHeap.top().second;
        if (read(tmpfd, &tmpValue2, sizeof(int64_t)) == sizeof(int64_t)) //Read another integer from the same file descriptor
        {
            minHeap.pop();
            minHeap.push({tmpValue2, tmpfd});
        }
        else    //If no more values can be read
        {
            minHeap.pop();
            if (close(tmpfd) == -1)
            {
                std::cout << "Error closing the file descriptor by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
        {
            buffer[i % BUFFERSIZE] = tmpValue;
            if ((i + 1) % BUFFERSIZE == 0 || minHeap.empty())
            {
                ssize_t tw = write(fdFinal, buffer, sizeof(int64_t) * ((i % BUFFERSIZE) + 1));
                if (tw == -1)
                {
                    std::cout << "Error writing to file" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
            i++;
        }
        else
        {
            ssize_t tw = write(fdFinal, &tmpValue, sizeof(int64_t));
            if (tw == -1)
            {
                std::cout << "Error writing to file" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }        
    }

    for (const std::string &fn : fns)       //Remove all temporary files after merging them
    {
        if (unlink(&fn[0]) == -1)
        {
            std::cout << "Error unlinking file '" << fn << "' by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    end = MPI_Wtime();
    std::cout << end-start << "s" << " => Time function kMerge() in Process " << mpiRank+1  << "/" << mpiSize << std::endl;
    std::cout << std::endl << "Sorted file '" << finalFile << "'" << std::endl;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);   //Initialize the MPI environment

    double startGlobal, endGlobal;
    int id, mpiSize, mpiRank;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);    //Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);    //Get the number of process
    if (mpiRank == 0)
    {
        startGlobal = MPI_Wtime();
        std::srand(std::time(0));
        id = std::rand() % 10000;                    //Get a random id number to recognize files of different executions
    }
    MPI_Bcast(&id, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (argc != 2)
    {
        if (mpiRank == 0)
        {
            std::cout << "Usage: " << argv[0] << " <file_to_parse>" << std::endl;
            std::cout << "It returns a file with extension '.sort' in the same directory of the not-parsed one. Make sure to have space before." << std::endl;
            std::cout << "Use arguments in the make as ARGS=\"stuff\". Example 'make run ARGS=\"/path/to/file\"'." << std::endl;
        }
        MPI_Finalize(); //Clean up the MPI environment
        return 0;
    }

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
    if (mpiRank == 0)
        std::cout << "Sorting file '" << argv[1] << "' of " << fileSize << " elements" << std::endl << std::endl;

    if (fileSize < (CACHENUM * mpiSize))
        slices = (fileSize / CACHENUM) + 1;
    else if (fileSize < (RAMNUM * mpiSize))     //TODO add more granularity considering double RAM for snow plow technique
        slices = (fileSize / RAMNUM) + 1;
    else
        slices = mpiSize + 1;
    sliceSize = (fileSize / slices) + 1;        //Each process divides a number of 8-byte integers based on the size of the starting file, Attualmente dentro create Runs
    maxLoop = (slices / mpiSize) + 1;
    if (sliceSize > RAMNUM)
        snowPlowRuns(fileSize, sliceSize, maxLoop, file, id, mpiRank, mpiSize);
    else
        sortRuns(fileSize, sliceSize, maxLoop, file, id, mpiRank, mpiSize);
    fclose(file);

    MPI_Barrier(MPI_COMM_WORLD);        //Blocks the caller until all processes in the communicator have called it

    if(mpiRank==0)
    {
        kMerge(argv[1], id, mpiRank, mpiSize);

        endGlobal = MPI_Wtime();
        std::cout << (endGlobal-startGlobal)/60.0 << "min" << " => FULL EXECUTION TIME" << std::endl;
        std::cout << std::endl << "To visualize binary files in bash can be used:" << std::endl;
        std::cout << "od -t d8 -A n binaryfile.bin                    #For in use format" << std::endl;
        std::cout << "od -t d8 -A n --endian=little binaryfile.bin    #For little-endian format" << std::endl;
        std::cout << "od -t d8 -A n --endian=big binaryfile.bin       #For big-endian format" << std::endl;
    }
    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}
