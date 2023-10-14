
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

//The number of numbers that populate the file
#define BUF_SIZE 2097152//16 MegaBytes
#define BUF_SIZE_HGB 67108864//512 MegaBytes
#define BUF_SIZE_GB 134217728//1 GigaBytes
#define BUF_SIZE_10GB 1342177280//10 GigaBytes

/*
Generate a file of given size filling it with random 8 bytes numbers.
Return the number of microseconds needed for the generation and writing on disk.
*/
long long benchmark_generate_file(const char *pathname, unsigned int seed)
{
    struct timeval start, end;
    unsigned int size = 0;
    FILE *file;
    int64_t *buffer;

    srand(seed);
    // if (access(pathname, F_OK) == 0)
    //     return -2;//File already exist
    if (size == 0)
    {
        printf("Insert a multiple of %d(16 MegaBytes) for the size of the target file:\n", BUF_SIZE);
        while (1)
        {
            if (scanf("%u", &size) != 1)
            {
                printf("Insert a valid size for the file:\n");
                while (getchar() != '\n');//Clear the input buffer to prevent an infinite loop
            }
            else
                break;
        }
        printf("Future file dimension: (16 * %u) Mb\n",size);
    }
    buffer = (int64_t*)malloc(BUF_SIZE * sizeof(int64_t));
    if (!buffer)
        return -1;//Something went wrong
    file = fopen(pathname, "wb");
    if (!file)
        return -1;//Something went wrong
    gettimeofday(&start, NULL);//Timer Start
    for (unsigned int i = 0; i < size; i++)
    {
        for(int j=0; j < BUF_SIZE; j++)
        {
            buffer[j] = ((int64_t)rand() << 32) | rand();
            // printf("%ld\n", buffer[j]);
        }
        fwrite(buffer, sizeof(int64_t), BUF_SIZE, file);
    }
    gettimeofday(&end, NULL);//Timer Stop
    fclose(file);
    return (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec);
}
