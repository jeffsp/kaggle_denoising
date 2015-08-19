/// @file test_tracer.cc
/// @brief test TRACER macro
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-03-18

#include "horny_toad/verify.h"
#include "horny_toad/tracer.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace horny_toad;

void test_tracer ()
{
    std::stringstream ss;
    TRACE(ss) << "tracer output" << 123;
#ifndef NDEBUG
    VERIFY (!ss.str ().empty ());
#else
    VERIFY (ss.str ().empty ());
#endif
}

int main ()
{
    try
    {
        test_tracer ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
