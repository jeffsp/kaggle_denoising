/// @file test_invert.cc
/// @brief test invert
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-06-12

#include "horny_toad/horny_toad.h"
#include <iostream>

using namespace std;
using namespace jack_rabbit;
using namespace horny_toad;

void test1 (bool verbose)
{
    raster<float> x (3, 3);
    x (0, 0) = 1;    x (0, 1) = 3;    x (0, 2) = 3;
    x (1, 0) = 1;    x (1, 1) = 4;    x (1, 2) = 3;
    x (2, 0) = 1;    x (2, 1) = 3;    x (2, 2) = 4;
    raster<float> y (3, 3);
    y (0, 0) = 7;    y (0, 1) = -3;   y (0, 2) = -3;
    y (1, 0) = -1;   y (1, 1) = 1;    y (1, 2) = 0;
    y (2, 0) = -1;   y (2, 1) = 0;    y (2, 2) = 1;
    if (verbose)
    {
        print2d (clog, x);
        print2d (clog, y);
        print2d (clog, invert (x));
    }
    raster<float> z = invert (x);
    for (size_t i = 0; i < x.size (); ++i)
        VERIFY (about_equal (z[i], y[i]));
}

int main (int argc, char **)
{
    try
    {
        const bool verbose = (argc != 1);
        test1 (verbose);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
