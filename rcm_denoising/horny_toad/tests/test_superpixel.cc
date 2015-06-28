/// @file test_superpixel.cc
/// @brief test super pixel
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/superpixel.h"
#include "horny_toad/verify.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace horny_toad;
using namespace std;

void test0 ()
{
    superpixel s1 (0, 0, 0);
    VERIFY (s1.get_row () == 0);
    VERIFY (s1.get_col () == 0);
    VERIFY (s1.get_size () == 0);
    superpixel s2 (1, 2, 3);
    VERIFY (s2.get_row () == 1);
    VERIFY (s2.get_col () == 2);
    VERIFY (s2.get_size () == 3);
}

int main ()
{
    try
    {
        test0 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
