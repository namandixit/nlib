/*
 * Creator: Naman Dixit
 * Notice: © Copyright 2019 Naman Dixit
 */

#include "nlib_tests.h"

#include "nlib_color.h"
#include "nlib_linmath.h"

Sint main (Sint argc, Char *argv[])
{
    unused_variable(argc);
    unused_variable(argv);

# if !defined(NLIB_NO_LIBC)
# endif
    raUnitTest(); // Uses sbufPrint, so tests might clash with printUnitTest
    htUnitTest();
    internUnitTest();
    mapUnitTest();
   return 0;
}
