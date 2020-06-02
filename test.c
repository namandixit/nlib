/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2019 Naman Dixit
 */
#define NLIB_TESTS
//#define NLIB_PRINT_BAD_FLOAT
#include "nlib.h"

Sint main (Sint argc, Char *argv[])
{
    unused_variable(argc);
    unused_variable(argv);
//    say("%e", -3.0);
//    say("\n");
# if !defined(NLIB_NO_LIBC)
# endif
    sbufUnitTest(); // Uses sbufPrint, so tests might clash with printUnitTest
    htUnitTest();
    internUnitTest();
    mapUnitTest();
    printUnitTest();
    return 0;
}
