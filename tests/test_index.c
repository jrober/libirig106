
#include <errno.h>
#ifndef _WIN32
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "i106_index.h"
#include "unity.h"
#include "unity_fixture.h"
#include <assert.h>

char tmats[1000];
int tmats_size;


TEST_GROUP_RUNNER(test_index){
    RUN_TEST_CASE(test_index, TestIndexPresent);
    RUN_TEST_CASE(test_index, TestReadIndexes);
    RUN_TEST_CASE(test_index, TestMakeIndex);
}


TEST_GROUP(test_index);
TEST_SETUP(test_index){}
TEST_TEAR_DOWN(test_index){}


TEST(test_index, TestIndexPresent){
    int handle = 1;
    int found_index;

    TEST_ASSERT_EQUAL(I106_OK, I106C10Open(&handle, "tests/copy.c10", READ));

    TEST_ASSERT_EQUAL(I106_OK, IndexPresent(handle, &found_index));
    TEST_ASSERT_EQUAL(0, found_index);
    I106C10Close(handle);
}


TEST(test_index, TestReadIndexes){
    int handle = 1;
    TEST_ASSERT_EQUAL(I106_OK, I106C10Open(&handle, "tests/indexed.c10", READ));

    TEST_ASSERT_EQUAL(I106_OK, ReadIndexes(handle));
    I106C10Close(handle);
}


TEST(test_index, TestMakeIndex){
    int handle = 1;
    TEST_ASSERT_EQUAL(I106_OK, I106C10Open(&handle, "tests/copy.c10", READ));

    TEST_ASSERT_EQUAL(I106_OK, MakeIndex(handle, 2));
    I106C10Close(handle);
}
