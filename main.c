#include "mavalloc.h"
#include <stdio.h>

int test_case_1()
{
  char * ptr = ( char * ) mavalloc_alloc ( 65535 );
  mavalloc_free( ptr ); 

  return 0;
}

int test_case_2()
{
  printf("Running test 2 to test a block split and reuse\n");

  mavalloc_init( 2048, FIRST_FIT );

  char * ptr1 = ( char * ) mavalloc_alloc( 2048 );

  mavalloc_free( ptr1 );

  char * ptr2 = ( char * ) mavalloc_alloc( 1024 );

  mavalloc_free( ptr2 );

  mavalloc_destroy( );

  return 0;
}

int main( int argc, char *argv[] )
{

  printf("Running the unit tests\n");
 
  size_t bytes = 65535;

  mavalloc_init( bytes, FIRST_FIT );


  mavalloc_destroy( );

  return 0;
}
