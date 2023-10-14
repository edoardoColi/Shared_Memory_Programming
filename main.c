#include <stdio.h>

long long benchmark_generate_file(const char *pathname, unsigned int seed);
long long benchmark_reade_file(const char *pathname);

int main(){

    printf("scrittura file %s: time(%lld microseconds)\n","testiamolo.bin",benchmark_generate_file("testiamolo.bin",3210));
printf("POI\n");
    printf("lettura file %s: time(%lld microseconds)\n","testiamolo.bin",benchmark_reade_file("testiamolo.bin"));
    return 0;
}