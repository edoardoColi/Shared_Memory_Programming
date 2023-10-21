#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

/*
*/
long long benchmark_reader_file(const char *pathname)
{
    struct timeval start, end;
    unsigned long long startOffset = 0; // Start from the first number (0-based index)
    unsigned long long endOffset = -1;   // Read up to the X number (exclusive), -1 in the last one because is unsigned
    FILE *file;
    int64_t num;

    file = fopen(pathname, "rb");
    if (!file)
        return -1;//Something went wrong
    fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
    gettimeofday(&start, NULL);//Timer Start
    unsigned long long currentOffset = startOffset;
    while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
        currentOffset++;
    gettimeofday(&end, NULL);//Timer Stop
    fclose(file);
    return (end.tv_sec - start.tv_sec) * 1000000LL + (end.tv_usec - start.tv_usec);
}

/*
*/
int is_sorted(const char *pathname)
{
    unsigned long long startOffset = 0; // Start from the first number (0-based index)
    unsigned long long endOffset = -1;   // Read up to the X number (exclusive), -1 in the last one because is unsigned
    FILE *file;
    int64_t num;

    file = fopen(pathname, "rb");
    if (!file)
        return -1;//Something went wrong
    fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
    unsigned long long currentOffset = startOffset;
    int64_t tmp;
    fread(&tmp, sizeof(int64_t), 1, file); // Take first element(number) in the file
    currentOffset++;
    while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
    {
        if(tmp > num){
            fclose(file);
            return 0;
        }
        tmp = num;
        currentOffset++;
    }
    fclose(file);
    return 1;
}

/*
Start from the start number (0-based index), read up to the end number (exclusive).
*/
void print_partial_file(const char *pathname, unsigned long long startOffset, unsigned long long endOffset)
{
    FILE *file;
    int64_t num;

    file = fopen(pathname, "rb");
    if (!file)
        return;//Something went wrong
    fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
    unsigned long long currentOffset = startOffset;
    while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
    {
        printf("%lld\n", (long long)num);
        currentOffset++;
    }
    fclose(file);
    return;
}

/*
*/
void print_all_file(const char *pathname)
{
    unsigned long long startOffset = 0; // Start from the first number (0-based index)
    unsigned long long endOffset = -1;   // Read up to the X number (exclusive), -1 in the last one because is unsigned
    FILE *file;
    int64_t num;

    file = fopen(pathname, "rb");
    if (!file)
        return;//Something went wrong
    fseek(file, startOffset * sizeof(int64_t), SEEK_SET);
    unsigned long long currentOffset = startOffset;
    while (currentOffset < endOffset && fread(&num, sizeof(int64_t), 1, file) == 1)
    {
        printf("%lld - ", (long long)num);
        currentOffset++;
    }
    printf("EOF\n");
    fclose(file);
    return;
}
