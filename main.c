#include "mavalloc.h"
#include "tinytest.h"
#include <stdio.h>

/*
*
* TEST CASE 1: Test init and a single allocation
*
*/
int test_case_1()
{
  mavalloc_init( 65535, BEST_FIT );
  char * ptr = ( char * ) mavalloc_alloc ( 65535 );

  int size = mavalloc_size();

  TINYTEST_EQUAL( size, 1); 
  return 1;
}

/*
*
* TEST CASE 2: Test Coalescing Free Blocks 
*
* This test case tests combining free block when the
* second free block is freed second 
*
*/
int test_case_2()
{
  mavalloc_init( 128000, BEST_FIT );

  char * ptr1    = (char*)mavalloc_alloc( 65535 );
  char * ptr2    = (char*)mavalloc_alloc( 65 );

  mavalloc_free( ptr1 );
  mavalloc_free( ptr2 );

  int size = mavalloc_size();
  TINYTEST_EQUAL( size, 1 ); 

  return 1;
}

/*
*
* TEST CASE 3: Test Coalescing Free Blocks 
*
* This test case tests combining free block when the
* second free block is freed first 
*
*/
int test_case_3()
{
  mavalloc_init( 65600, BEST_FIT );

  char * ptr1    = (char*)mavalloc_alloc( 65536 );
  char * ptr2    = (char*)mavalloc_alloc( 64 );

  mavalloc_free( ptr2 );
  mavalloc_free( ptr1 );

  int size = mavalloc_size();
  TINYTEST_EQUAL( size, 1 ); 

  return 1;
}

/*
*
* TEST CASE 4: Test First Fit and splitting free blocks
*
*/
int test_case_4()
{
  mavalloc_init( 65535, FIRST_FIT );

  char * ptr1  = ( char * ) mavalloc_alloc ( 10000 );
  char * ptr2  = ( char * ) mavalloc_alloc ( 65 );

  int size = mavalloc_size();

  TINYTEST_EQUAL( size, 3); 
  return 1;
}


/*
*
* TEST CASE 5: Test Next 
*
*/
int test_case_5()
{
  mavalloc_init( 65535, NEXT_FIT );

  char * ptr1  = ( char * ) mavalloc_alloc ( 10 );
  char * buf1  = ( char * ) mavalloc_alloc ( 1 );
  char * ptr2  = ( char * ) mavalloc_alloc ( 1000 );
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
  return 1;
}

/*
*
* TEST CASE 6: Test Worst Fit 
*
*/
int test_case_6()
{
  mavalloc_init( 65535, WORST_FIT );
  char * ptr1    = ( char * ) mavalloc_alloc ( 65535 );
  char * buffer1 = ( char * ) mavalloc_alloc( 1 );
  char * ptr4    = ( char * ) mavalloc_alloc ( 65 );
  char * buffer2 = ( char * ) mavalloc_alloc( 1 );
  char * ptr2    = ( char * ) mavalloc_alloc ( 6000 );

  mavalloc_free( ptr1 ); 
  mavalloc_free( ptr2 ); 

  buffer1 = buffer1;
  buffer2 = buffer2;
  ptr4 = ptr4;

  char * ptr3 = ( char * ) mavalloc_alloc ( 1000 );

  TINYTEST_EQUAL( ptr1, ptr3 ); 
  return 1;
}

/*
*
* TEST CASE 7: Test Best Fit 
*
*/
int test_case_7()
{
  mavalloc_init( 75000, BEST_FIT );
  char * ptr1    = ( char * ) mavalloc_alloc ( 65535 );
  char * buffer1 = ( char * ) mavalloc_alloc( 1 );
  char * ptr4    = ( char * ) mavalloc_alloc ( 65 );
  char * buffer2 = ( char * ) mavalloc_alloc( 1 );
  char * ptr2    = ( char * ) mavalloc_alloc ( 1500 );

  TINYTEST_ASSERT( ptr1 ); 
  TINYTEST_ASSERT( ptr4 ); 
  TINYTEST_ASSERT( ptr2 ); 

  mavalloc_free( ptr1 ); 
  mavalloc_free( ptr2 ); 

  buffer1 = buffer1;
  buffer2 = buffer2;
  ptr4 = ptr4;

  char * ptr3 = ( char * ) mavalloc_alloc ( 1000 );
  TINYTEST_ASSERT( ptr3 ); 

  TINYTEST_EQUAL( ptr2, ptr3 ); 
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
  TINYTEST_ADD_TEST(test_case_4,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_5,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_6,tinytest_setup,tinytest_teardown);
  TINYTEST_ADD_TEST(test_case_7,tinytest_setup,tinytest_teardown);
TINYTEST_END_SUITE();

TINYTEST_MAIN_SINGLE_SUITE(MavAllocTestSuite);
