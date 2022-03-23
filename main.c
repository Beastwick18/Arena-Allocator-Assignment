#include "mavalloc.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{

  printf("Running the unit tests\n");
 
  size_t bytes = 65535;

  mavalloc_init( bytes, FIRST_FIT );


  mavalloc_destroy( );

  return 0;
}
