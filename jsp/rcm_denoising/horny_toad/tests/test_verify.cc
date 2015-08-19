/// @file test_verify.cc
/// @brief test verify
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/verify.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>

using namespace std;
using namespace horny_toad;

void test_verify ()
{
    // Make sure VERIFY(true) works
    VERIFY (true);

    // Make sure VERIFY(false) works
    try
    { VERIFY (false); }
    catch (...)
    { return; }
    throw runtime_error ("VERIFY failed");
}

int main ()
{
    try
    {
        test_verify ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
