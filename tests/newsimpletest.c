/*
 * File:   newsimpletest.c
 * Author: Jeff
 *
 * Created on May 1, 2012, 5:34:02 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "my_string.h"

/*
 * Simple C Test Suite
 */


int main(int argc, char** argv) {
    int r;
    
    printf("%%SUITE_STARTING%% newsimpletest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (compare \"string one\" and \"string completely different\")\n");
    r = same_string("string one", "string completely different");
    printf("result = %d\n", r);
    printf("%%TEST_FINISHED%% time=0 test1 (newsimpletest) \n");

    printf("%%TEST_STARTED%% test2 (compare \"same 1 length\" and \"same 2 length\")\n");
    r = same_string("same 1 length", "same 2 length");
    printf("result = %d\n", r);
    printf("%%TEST_FINISHED%% time=0 test2 (newsimpletest) \n");

    printf("%%TEST_STARTED%% test3 (compare \"same str\" and \"same str\")\n");
    r = same_string("same str", "same str");
    printf("result = %d\n", r);
    printf("%%TEST_FINISHED%% time=0 test3 (newsimpletest) \n");
    
    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}
