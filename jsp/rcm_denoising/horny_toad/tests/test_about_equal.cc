/// @file test_about_equal.cc
/// @brief test about_equal
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/about_equal.h"
#include "horny_toad/pi.h"
#include "horny_toad/verify.h"
#include <iostream>

using namespace std;
using namespace horny_toad;

int main ()
{
    try
    {
        VERIFY (about_equal (1, 1));
        VERIFY (about_equal (PI(), 3.14159));
        VERIFY (about_equal (PI(), 3.0, 1.0));
        VERIFY (about_equal (PI(), 3.142, 0.001));
        VERIFY (!about_equal (PI(), 3.141, 0.001));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
