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
#define BUFFERSIZEREAD 32768
#define BUFFERSIZEWRITE 131072
#define RAMNUM 134217728*2
#define ALLOW_BUFFER 1

// This function is used in multiple nodes(PARALLEL EXECUTION)
void sortedRuns(MPI_Offset fileSize, MPI_Offset sliceSize, int maxLoop, MPI_File file, int id, int mpiRank, int mpiSize)
{
    double startTot, start, end;
    MPI_Offset startOffset, endOffset, currentOffset;    //The interval is [startOffset, endOffset)
    MPI_Status status;
    int rmpi;
    int elementsRead;
    int64_t num;
    std::vector<int64_t> bigVect;
    int64_t bufferRead[static_cast<MPI_Offset>(BUFFERSIZEREAD)];
    int64_t bufferWrit[static_cast<MPI_Offset>(BUFFERSIZEWRITE)];
    bigVect.reserve(sliceSize);

    startTot = MPI_Wtime();                            //Microsecond precision. Can't use time(), because each process will have a different "zero" time
    start = MPI_Wtime();
    for(MPI_Offset l = 0; l < maxLoop; l++)     //Populate the vector with the values in the file
    {
        startOffset = sliceSize * (mpiRank + (mpiSize * l));
        if (startOffset >= fileSize)
            break;
        endOffset = startOffset + sliceSize;
        currentOffset = startOffset;
        bigVect.clear();

        if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
        {
            while (currentOffset < endOffset)
            {
                MPI_Offset elementsToRead = std::min(endOffset - currentOffset,  static_cast<MPI_Offset>(static_cast<MPI_Offset>(BUFFERSIZEREAD)));      //It's important to check because if the difference between endOffset and startOffset is smaller than BUFFERSIZE we don't have to read further
                rmpi = MPI_File_read_at(file, currentOffset * sizeof(int64_t), bufferRead, elementsToRead, MPI_INT64_T, &status);
                if (rmpi != MPI_SUCCESS)
                {
                    std::cout << "Error reading file at offset ...Terminating" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                rmpi = MPI_Get_count(&status, MPI_INT64_T, &elementsRead);
                if (rmpi != MPI_SUCCESS)
                {
                    std::cout << "Error getting count ...Terminating"<< std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                for (int i = 0; i < elementsRead; ++i)
                {
                    bigVect.push_back(bufferRead[i]);
                }
                currentOffset += elementsRead;      //Increment currentOffset based on the number of elements read
                if (elementsRead < static_cast<MPI_Offset>(BUFFERSIZEREAD))      // Check if we have reached the end of the file
                    break;
            }
        }
        else
        {
            while (currentOffset < endOffset)
            {
                rmpi = MPI_File_read_at(file, currentOffset * sizeof(int64_t), &num, 1, MPI_INT64_T, &status);
                if (rmpi != MPI_SUCCESS)
                {
                    std::cout << "Error reading file at offset ...Terminating" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                rmpi = MPI_Get_count(&status, MPI_INT64_T, &elementsRead);
                if (rmpi != MPI_SUCCESS)
                {
                    std::cout << "Error getting count ...Terminating"<< std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                if (elementsRead == 1)
                {
                    bigVect.push_back(num);
                    currentOffset++;
                }
                else
                {
                    break;
                }
            }
        }
        end = MPI_Wtime();
        std::cout << "   " << end-start << "s" << " => Time to read file from offset " << startOffset << " to " << endOffset << " in Process " << mpiRank+1  << "/" << mpiSize << std::endl;
        start = MPI_Wtime();

        sort(bigVect.begin(), bigVect.end());

        end = MPI_Wtime();
        std::cout << "   " << end-start << "s" << " => Time to sort elements in Process " << mpiRank+1  << "/" << mpiSize << " memory" << std::endl;

        std::string templateName = "/mnt/raid/tmp/SortedRun" + std::to_string(id) + "_XXXXXX";     //If absolute path does not exist the temporary file will not be created (mandatory 6 times X)
        int fd = mkstemp(&templateName[0]);     //Create a temporary file based on template
        if (fd == -1)
        {
            std::cout << "Error creating temporary file ...Terminating" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if (close(fd) == -1)
        {
            std::cout << "Error closing the file descriptor ...Terminating" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_File tmpFile;
        rmpi = MPI_File_open(MPI_COMM_SELF, &templateName[0], MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &tmpFile);
        if (rmpi != MPI_SUCCESS)
        {
            std::cout << "Error opening file ...Terminating"<< std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        start = MPI_Wtime();

        if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
        {
            MPI_Offset offset = 0;
            for (MPI_Offset i = 0; i < bigVect.size(); ++i)
            {
                bufferWrit[i % BUFFERSIZEWRITE] = bigVect[i];
                if ((i + 1) % BUFFERSIZEWRITE == 0 || i == bigVect.size() - 1)
                {
                    int count = (i % BUFFERSIZEWRITE) + 1;
                    rmpi = MPI_File_write_at(tmpFile, offset, bufferWrit, count, MPI_INT64_T, &status);
                    if (rmpi != MPI_SUCCESS)
                    {
                        std::cout << "Error writing to file at offset ...Terminating" << std::endl;
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                    offset += count * sizeof(int64_t);
                }
            }
        }
        else
        {
            for (MPI_Offset i = 0; i < bigVect.size(); ++i)     //Write the ordered number in a temp file
            {
                int64_t elem = bigVect[i];
                rmpi = MPI_File_write_at(tmpFile, i * sizeof(int64_t), &elem, 1, MPI_INT64_T, &status);
                if (rmpi != MPI_SUCCESS)
                {
                    std::cout << "Error writing to file at offset ...Terminating"<< std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
        }

        end = MPI_Wtime();
        MPI_Offset sz;
        rmpi = MPI_File_get_size(tmpFile, &sz);
        if (rmpi != MPI_SUCCESS)
        {
            std::cout << "Error getting file size ...Terminating"<< std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        if (sz == 0)
        {
            rmpi = MPI_File_close(&tmpFile);
            if (rmpi != MPI_SUCCESS)
            {
                std::cout << "Error closing file ...Terminating"<< std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            rmpi = MPI_File_delete(&templateName[0], MPI_INFO_NULL);
            if (rmpi != MPI_SUCCESS)
            {
                std::cout << "Error deleting file ...Terminating"<< std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        rmpi = MPI_File_close(&tmpFile);
        if (rmpi != MPI_SUCCESS)
        {
            std::cout << "Error closing file ...Terminating"<< std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        std::cout << "   " << end-start << "s" << " => Time to write '" << templateName << "' and fill it up with " << sz/8 << " sorted elements by Process " << mpiRank+1  << "/" << mpiSize << std::endl;
        start = MPI_Wtime();
    }
    end = MPI_Wtime();
    std::cout << end-startTot << "s" << " => Time function sortedRuns() in Process " << mpiRank+1  << "/" << mpiSize << std::endl;
}

// This function is used in a single node(SEQUENTIAL EXECUTION) so we can avoid using MPI functions(TODO false rewrite using MPI)
void kMerge(const std::string &argFile, int id, int mpiRank, int mpiSize)
{
    std::string fileDir = "/mnt/raid/tmp/";
    std::string pattern = "SortedRun" + std::to_string(id) + "_";
    std::vector<int> fds;       //To store the file descriptor of each file to merge
    std::vector<std::string> fns;     //To store the file name of each file to delete after merge
    size_t lastSlash = argFile.find_last_of('/');
    std::string nameOnly = (lastSlash != std::string::npos) ? argFile.substr(lastSlash + 1) : argFile;
    std::string finalFile = "/mnt/raid/tmp/" + nameOnly + (ALLOW_BUFFER != 1 ? ".nobuf" : "") + ".sort";
    off_t fileSize;
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
                        fileSize = lseek(fd, 0, SEEK_END);
                        fileSize = fileSize / 8;    //Size in bytes of the file, correspond to the number of numbers to parse. Each number is 8 bytes
                        std::cout << "Merging '" << tmpFile.c_str() << "' of size " << (fileSize/134217728 >= 1 ? fileSize/134217728.0 : fileSize/131072.0) << (fileSize/134217728 >= 1 ? "Gb" : "Mb") << std::endl;
			            lseek(fd, 0, SEEK_SET);     //Set back the pointer of file to it's begin

                        fds.push_back(fd);
                        fns.push_back(tmpFile);
                        fileCount++;
                    }
                    else
                        std::cout << "Error opening file '" << tmpFile << "'" << std::endl;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "Error opening directory '" << fileDir << "' ...Terminating" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int fdFinal = open(finalFile.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);     //Open the file for writing only, creating it if it doesn't exist and (add '| O_EXCL' near O_CREAT if want the next feature) not overwrite if it exists
    if (fdFinal == -1)
    {
        std::cout << "Error opening or creating final file '" << finalFile << "'...Terminating" << std::endl;
        for (const std::string &fn : fns)       //Remove all temporary files before abort
        {
            if (unlink(&fn[0]) == -1)
            {
                std::cout << "Error unlinking file '" << fn << "' ...Terminating" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::cout << std::endl << "Starting the merge process for " << fileCount << " files" << std::endl;
    start = MPI_Wtime();

    std::priority_queue<std::pair<int64_t, int>, std::vector<std::pair<int64_t, int>>, std::greater<std::pair<int64_t, int>>> minHeap;      //Creating a Min Heap using a priority queue
    int64_t tmpValue;

    int b=0;
    for(int i = 0; i < 1; i++)
    {
        for (int fd : fds)    //Populate the Min Heap with initial values from each file descriptor
        {
            switch (read(fd, &tmpValue, sizeof(int64_t)))
            {
                case sizeof(int64_t):
                    minHeap.push({tmpValue, fd});
                    break;
                case 0:
                    b = 1;
                    break;
                default:
                    std::cout << i << "Error reading size=" << sizeof(int64_t) << " from file descriptor ...Terminating" << std::endl;
                    MPI_Abort(MPI_COMM_WORLD, 1);
                    break;
            }
        }
        if (b == 1)
            break;
    }
    std::cout << "(heap size=" << minHeap.size() << ")" << std::endl;

    int tmpfd;
    int64_t tmpValue2;
    int64_t bufferWrit[static_cast<unsigned long long>(BUFFERSIZEWRITE)];
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
            if (fcntl(tmpfd, F_GETFD) == 1 && close(tmpfd) == -1)
            {
                std::cout << "Error closing the file descriptor ...Terminating" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        if (ALLOW_BUFFER)      //Branch to test performance with and without buffer
        {
            bufferWrit[i % static_cast<unsigned long long>(BUFFERSIZEWRITE)] = tmpValue;
            if ((i + 1) % static_cast<unsigned long long>(BUFFERSIZEWRITE) == 0 || minHeap.empty())
            {
                ssize_t tw = write(fdFinal, bufferWrit, sizeof(int64_t) * ((i % static_cast<unsigned long long>(BUFFERSIZEWRITE)) + 1));
                if (tw == -1)
                {
                    std::cout << "Error writing to file ...Terminating" << std::endl;
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
                std::cout << "Error writing to file ...Terminating" << std::endl;
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }

    for (const std::string &fn : fns)       //Remove all temporary files after merging them
    {
        if (unlink(&fn[0]) == -1)
        {
            std::cout << "Error unlinking file '" << fn << "' ...Terminating" << std::endl;
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
            std::cout << "It returns a file with extension '.sort' in the /mnt/raid/tmp/ directory. Make sure to have space before." << std::endl;
            std::cout << "Use arguments in the make as ARGS=\"stuff\". Example 'make run ARGS=\"/path/to/file\"'." << std::endl;
        }
        MPI_Finalize(); //Clean up the MPI environment
        return 0;
    }

    MPI_File file;
    MPI_Offset fileSize, sliceSize;
    int slices, maxLoop;
    int rmpi;
    rmpi = MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &file);        //Mode set to MPI_MODE_RDONLY (read only), it’s equivalent to opening a file in binary read mode ("rb") in standard C/C++
    if (rmpi != MPI_SUCCESS)
    {
        std::cout << "Error opening file: " << argv[1] << " ...Terminating" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    rmpi = MPI_File_get_size(file, &fileSize);
    if (rmpi != MPI_SUCCESS)
    {
        std::cout << "Error getting file size ...Terminating"<< std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    fileSize = fileSize / 8; // Size in bytes of the file, correspond to the number of numbers to parse. Each number is 8 bytes
    if (mpiRank == 0)
        std::cout << "Using " << mpiSize << " nodes for sorting " << (fileSize/134217728 >= 1 ? fileSize/134217728.0 : fileSize/131072.0) << (fileSize/134217728 >= 1 ? "Gb" : "Mb") << " file '" << argv[1] << "' of " << fileSize << " elements (" << (RAMNUM/134217728 >= 1 ? RAMNUM/134217728.0 : RAMNUM/131072.0) << (RAMNUM/134217728 >= 1 ? "Gb" : "Mb") << " Ram each node)" << std::endl << std::endl;

    //Load balancer
    if (fileSize < ((MPI_Offset) static_cast<MPI_Offset>(RAMNUM) * mpiSize))    //Can add more granularity considering efficiency, now is used by default all nodes
        slices = mpiSize;
    else
        slices = mpiSize;
    sliceSize = (fileSize / slices);        //Each process divides a number of 8-byte integers based on the size of the starting file
    maxLoop = 1;
    if (sliceSize >= static_cast<MPI_Offset>(RAMNUM))
    {
        maxLoop = (fileSize / (static_cast<MPI_Offset>(RAMNUM) * mpiSize)) + 1;
	sliceSize = static_cast<MPI_Offset>(RAMNUM);
        sortedRuns(fileSize, sliceSize, maxLoop, file, id, mpiRank, mpiSize);
    }
    else
    {
        sortedRuns(fileSize, sliceSize + 1, maxLoop, file, id, mpiRank, mpiSize);
    }
    rmpi = MPI_File_close(&file);
    if (rmpi != MPI_SUCCESS)
    {
        std::cout << "Error closing file: " << argv[1] << " ...Terminating" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);        //Blocks the caller until all processes in the communicator have called it

    if(mpiRank==0)
    {
        kMerge(argv[1], id, mpiRank, mpiSize);
        endGlobal = MPI_Wtime();
        std::cout << (endGlobal-startGlobal)/60.0 << "min" << " => FULL EXECUTION TIME" << std::endl;
    }
    MPI_Finalize(); //Clean up the MPI environment
    return 0;
}
