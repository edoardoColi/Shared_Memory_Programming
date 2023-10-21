#include <stdio.h>

long long benchmark_generate_file(const char *pathname, unsigned int seed);
long long benchmark_reader_file(const char *pathname);
int is_sorted(const char *pathname);
void print_partial_file(const char *pathname, unsigned long long startOffset, unsigned long long endOffset);
void print_all_file(const char *pathname);

int main(){

    printf("scrittura file %s: time(%lld microseconds)\n","testiamolo.bin",benchmark_generate_file("testiamolo.bin",3210));//TODO cambiare il seed una volta completatp
printf("POI\n");
    printf("lettura file %s: time(%lld microseconds)\n","testiamolo.bin",benchmark_reader_file("testiamolo.bin"));
printf("POI\n");
printf("sono uguali? %d\n",is_sorted("testiamolo.bin"));
print_partial_file("testiamolo.bin",2,4);
printf("POI\n");
print_all_file("testiamolo.bin");
    return 0;
}
