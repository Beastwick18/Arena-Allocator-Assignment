#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mavalloc.h"

int main( int argc, char * argv[] )
{
  clock_t start = clock();
  
  mavalloc_init(131072, BEST_FIT);
  
  void *ptrs[1000];
  
  // 1 large allocation, immediatly free'd, many times
  for(int i = 0; i < 1000; i++) {
    ptrs[0] = mavalloc_alloc(131072);
    mavalloc_free(ptrs[0]);
  }
  
  for(int i = 0; i < 32; i++) {
    ptrs[i] = mavalloc_alloc(1024);
  }
  for(int i = 0; i < 32; i+=2) {
    mavalloc_free(ptrs[i]);
  }
  for(int i = 0; i < 32; i+=2) {
    ptrs[i] = mavalloc_alloc(1024);
  }
  
  for(int i = 0; i < 32; i++) {
    mavalloc_free(ptrs[i]);
  }
  
  for(int i = 0; i < 999; i++) {
    ptrs[i] = mavalloc_alloc(15);
  }
  for(int i = 0; i < 999; i += 3) {
    mavalloc_free(ptrs[i]);
    mavalloc_free(ptrs[i+2]);
    mavalloc_free(ptrs[i+1]);
  }
  
  mavalloc_destroy();
  
  clock_t end = clock();
  // Get elapsed time in milliseconds
  double elapsed = ((double) (end - start)) / ((double)CLOCKS_PER_SEC/1000);
  
  printf("%f\n", elapsed);
  return 0;
}
