#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mavalloc.h"

int main( int argc, char * argv[] )
{
  clock_t start = clock();
  
  // mavalloc_init(131072, BEST_FIT);
  
  void *ptrs[1000];
  
  for(int i = 0; i < 1000; i++) {
    ptrs[0] = malloc(131072);
    free(ptrs[0]);
  }
  
  for(int i = 0; i < 32; i++) {
    ptrs[i] = malloc(1024);
  }
  for(int i = 0; i < 32; i+=2) {
    free(ptrs[i]);
  }
  for(int i = 0; i < 32; i+=2) {
    ptrs[i] = malloc(1024);
  }
  
  for(int i = 0; i < 32; i++) {
    free(ptrs[i]);
  }
  
  for(int i = 0; i < 999; i++) {
    ptrs[i] = malloc(15);
  }
  for(int i = 0; i < 999; i += 3) {
    free(ptrs[i]);
    free(ptrs[i+2]);
    free(ptrs[i+1]);
  }
  
  clock_t end = clock();
  double elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  
  printf("%f\n", elapsed);
  return 0;
}
