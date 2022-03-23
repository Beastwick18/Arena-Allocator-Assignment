#include "mavalloc.h"
#include "tinytest.h"
#include <stdio.h>

int test_case_1()
{
  char * ptr = ( char * ) mavalloc_alloc ( 65535 );
  mavalloc_free( ptr ); 

  return 1;
}

int test_case_2()
{
  mavalloc_init( 2048, FIRST_FIT );

  char * ptr1 = ( char * ) mavalloc_alloc( 2048 );

  mavalloc_free( ptr1 );

  char * ptr2 = ( char * ) mavalloc_alloc( 1024 );

  mavalloc_free( ptr2 );

  mavalloc_destroy( );

  return 1;
}

int test_case_3()
{
  // intentionally fail this test for now
  
  TINYTEST_EQUAL(1, 0);

  return 1;
}

int tinytest_setup(const char *pName)
{
    fprintf( stderr, "tinytest_setup(%s)\n", pName);
    return 0;
}



int tinytest_teardown(const char *pName)
{
    fprintf( stderr, "tinytest_teardown(%s)\n", pName);
    return 0;
}


TINYTEST_START_SUITE(MavAllocTestSuite);
  TINYTEST_ADD_TEST(test_case_1,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_2,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_3,tinytest_setup,tinytest_teardown);
TINYTEST_END_SUITE();

TINYTEST_MAIN_SINGLE_SUITE(MavAllocTestSuite);
