#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

/*
*/
long long benchmark_reade_file(const char *pathname)
{
    struct timeval start, end;
    unsigned int startOffset = 0; // Start from the third number (0-based index)
    unsigned int endOffset = -1;   // Read up to the X number (exclusive)
    FILE *file;
    int64_t num;

    file = fopen(pathname, "rb");
    if (!file)
        return -1;//Something went wrong
    fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
    gettimeofday(&start, NULL);//Timer Start
    unsigned int currentOffset = startOffset;
    while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
    {
        // printf("%lld\n", (long long)num);
        currentOffset++;
    }
    gettimeofday(&end, NULL);//Timer Stop
    fclose(file);
    return (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec);
}
