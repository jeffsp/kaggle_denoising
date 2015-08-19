/// @file test_log2.cc
/// @brief test log2
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/about_equal.h"
#include "horny_toad/log2.h"
#include "horny_toad/verify.h"
#include <iostream>

using namespace std;
using namespace horny_toad;

int main ()
{
    try
    {
        //for (double x = 1; x < 20; ++x)
        //    clog << "logx(" << x << ")=" << log2 (x) << endl;
        VERIFY (about_equal (log2 (1.0/3.0), -1.585));
        VERIFY (about_equal (log2 (1.0), 0.0));
        VERIFY (about_equal (log2 (3.0), 1.585));
        VERIFY (about_equal (log2 (4.0), 2.000));
        VERIFY (about_equal (log2 (7.0), 2.807));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
