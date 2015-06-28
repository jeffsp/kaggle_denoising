/// @file test_bresenham.cc
/// @brief test line drawing
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-04-26

#include "horny_toad/horny_toad.h"
#include <iostream>

using namespace std;
using namespace horny_toad;

int main ()
{
    try
    {
        vector<int> x;
        vector<int> y;
        line (-2, -3, 1, 4, x, y);
        VERIFY (x.size () == y.size ());
        VERIFY (x == vector<int> ({ -2, -2, -1, -1, 0, 0, 1, 1 }));
        VERIFY (y == vector<int> ({ -3, -2, -1, 0, 1, 2, 3, 4 }));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
