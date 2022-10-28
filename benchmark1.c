#include <stdio.h>
#include "mavalloc.h"

int main( int argc, char * argv[] )
{
  int d = mavalloc_init(65535, FIRST_FIT);
  
  void *x = mavalloc_alloc(65535);
  printf("%d init: %p\n", d, x);
  mavalloc_free(x);
  void *x1 = mavalloc_alloc(10);
  void *x2 = mavalloc_alloc(10);
  void *x3 = mavalloc_alloc(10);
  void *x4 = mavalloc_alloc(10);
  void *y = mavalloc_alloc(100);
  printf("%p\n", x1);
  printf("%p\n", x2);
  printf("%p\n", x3);
  printf("%p\n", x4);
  printf("%p\n", y);
  
  mavalloc_destroy();
  return 0;
}
