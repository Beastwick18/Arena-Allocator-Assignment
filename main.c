#include "mavalloc.h"
#include "tinytest.h"
#include <stdio.h>

int test_case_1()
{
  mavalloc_init( 65535, BEST_FIT );
  char * ptr = ( char * ) mavalloc_alloc ( 65535 );

  int size = mavalloc_size();

  TINYTEST_EQUAL( size, 1); 
}

int test_case_2()
{
  mavalloc_init( 128000, BEST_FIT );

  char * ptr1    = (char*)mavalloc_alloc( 65535 );
  char * buffer1 = (char*)mavalloc_alloc( 1 );
  char * ptr2    = (char*)mavalloc_alloc( 65 );
  char * buffer2 = (char*)mavalloc_alloc( 1 );
  char * ptr3    = (char*)mavalloc_alloc( 6000 );

  mavalloc_free( ptr1 );
  mavalloc_free( ptr3 );

  char * val = (char*) mavalloc_alloc( 1000 );

  TINYTEST_EQUAL( val, ptr3); 
}

int test_case_3()
{
  mavalloc_init( 128000, WORST_FIT );

  char * ptr1    = (char*)mavalloc_alloc( 65535 );
  char * buffer1 = (char*)mavalloc_alloc( 1 );
  char * ptr2    = (char*)mavalloc_alloc( 65 );
  char * buffer2 = (char*)mavalloc_alloc( 1 );
  char * ptr3    = (char*)mavalloc_alloc( 6000 );

  mavalloc_free( ptr1 );
  mavalloc_free( ptr3 );

  char * val = (char*) mavalloc_alloc( 1000 );

  TINYTEST_EQUAL( val, ptr1); 
}

int test_case_4()
{
  mavalloc_init( 65535, FIRST_FIT );

  char * ptr1  = ( char * ) mavalloc_alloc ( 10000 );
  char * ptr2  = ( char * ) mavalloc_alloc ( 65 );

  int size = mavalloc_size();

  TINYTEST_EQUAL( size, 3); 
}

int test_case_5()
{
  mavalloc_init( 65535, NEXT_FIT );

  char * ptr1  = ( char * ) mavalloc_alloc ( 1000 );
  char * buf1  = ( char * ) mavalloc_alloc ( 1 );
  char * ptr2  = ( char * ) mavalloc_alloc ( 10 );
  char * buf2  = ( char * ) mavalloc_alloc ( 1 );
  char * ptr3  = ( char * ) mavalloc_alloc ( 6000 );
  char * buf3  = ( char * ) mavalloc_alloc ( 1 );
  char * ptr4  = ( char * ) mavalloc_alloc ( 1000 );
  char * buf4  = ( char * ) mavalloc_alloc ( 1 );

  char * right_answer = ptr4;


  ptr1 = ptr1;
  ptr2 = ptr2;
  ptr3 = ptr3;
  ptr4 = ptr4;

  buf1 = buf1;
  buf2 = buf2;
  buf3 = buf3;
  buf4 = buf4;

  mavalloc_free( ptr1 );
  mavalloc_free( ptr2 );
  mavalloc_free( ptr3 );
  mavalloc_free( ptr4 );


  char * ptr5 = ( char * ) mavalloc_alloc( 6000 );

  ptr5 = ptr5;

  char * ptr6 = ( char * ) mavalloc_alloc( 1000 );

  ptr6 = ptr6;

  TINYTEST_EQUAL( ptr6, right_answer ); 
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
  //TINYTEST_ADD_TEST(test_case_2,tinytest_setup,tinytest_teardown);
  //TINYTEST_ADD_TEST(test_case_3,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_4,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_5,tinytest_setup,tinytest_teardown);
TINYTEST_END_SUITE();

TINYTEST_MAIN_SINGLE_SUITE(MavAllocTestSuite);
