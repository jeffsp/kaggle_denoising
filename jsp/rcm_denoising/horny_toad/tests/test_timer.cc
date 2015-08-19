/// @file test_timer.cc
/// @brief test timer
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/timer.h"
#include "horny_toad/verify.h"
#include <boost/lambda/lambda.hpp>
#include <boost/random.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>

using namespace std;
using namespace horny_toad;

void test_timer (double secs)
{
    timer t;

    cout.precision (20);
    cout << fixed;
    cout << showpoint;
    //cout << secs << " secs to wait" << endl;
    double e = 0.0;
    t.tic ();
    while (e < secs)
        e = t.toc ();
    //cout << e << " secs elapsed" << endl;

    // We should get some minimum resolution
    VERIFY (e - secs < 0.01);
}

int main ()
{
    try
    {
        test_timer (0.000000001);
        test_timer (0.000001);
        test_timer (0.001);
        test_timer (1.0);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
