/// @file test_rect.cc
/// @brief test rect
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/rect.h"
#include "horny_toad/verify.h"
#include <iostream>
#include <stdexcept>

using namespace horny_toad;
using namespace std;

void test1 ()
{
    rect<float> r1 (0, 1, 2, 3);
    rect<float> r2 (1, 0, 2, 3);
    r1.clip (r2);
    VERIFY (r1.get_x () == 1);
    VERIFY (r1.get_y () == 1);
    VERIFY (r1.get_width () == 1);
    VERIFY (r1.get_height () == 2);
    rect<float> r3 (0, 0, 3, 3);
    r1.clip (r3);
    VERIFY (r1.get_x () == 1);
    VERIFY (r1.get_y () == 1);
    VERIFY (r1.get_width () == 1);
    VERIFY (r1.get_height () == 2);
    r3.clip (r1);
    VERIFY (r3.get_x () == 1);
    VERIFY (r3.get_y () == 1);
    VERIFY (r3.get_width () == 1);
    VERIFY (r3.get_height () == 2);
}

void test2 ()
{
    rect<double> r;
    VERIFY (r.empty ());
    r.set_width (0.1);
    r.set_height (0.0);
    VERIFY (r.empty ());
    r.set_width (0.0);
    r.set_height (0.1);
    VERIFY (r.empty ());
    r.set_width (0.1);
    r.set_height (0.1);
    VERIFY (!r.empty ());
}

int main ()
{
    try
    {
        test1 ();
        test2 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

