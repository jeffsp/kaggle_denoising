/// @file test_polar.cc
/// @brief test polar
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/about_equal.h"
#include "horny_toad/pi.h"
#include "horny_toad/polar.h"
#include "horny_toad/verify.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace horny_toad;

int main ()
{
    try
    {
        double r, th;
        double x, y;
        cart2pol (1.0, 0.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, 1.0));
        VERIFY (about_equal (th, 0*PI()/4));
        VERIFY (about_equal (x, 1.0));
        VERIFY (about_equal (y, 0.0));
        cart2pol (1.0, 1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, sqrt (2.0)));
        VERIFY (about_equal (th, 1*PI()/4));
        VERIFY (about_equal (x, 1.0));
        VERIFY (about_equal (y, 1.0));
        cart2pol (0.0, 1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, 1.0));
        VERIFY (about_equal (th, 2*PI()/4));
        VERIFY (about_equal (x, 0.0));
        VERIFY (about_equal (y, 1.0));
        cart2pol (-1.0, 1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, sqrt (2.0)));
        VERIFY (about_equal (th, 3*PI()/4));
        VERIFY (about_equal (x, -1.0));
        VERIFY (about_equal (y, 1.0));
        cart2pol (-1.0, 0.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, 1.0));
        VERIFY (about_equal (th, 4*PI()/4));
        VERIFY (about_equal (x, -1.0));
        VERIFY (about_equal (y, 0.0));
        cart2pol (-1.0, -1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, sqrt (2.0)));
        VERIFY (about_equal (th, -3*PI()/4));
        VERIFY (about_equal (x, -1.0));
        VERIFY (about_equal (y, -1.0));
        cart2pol (0.0, -1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, 1.0));
        VERIFY (about_equal (th, -2*PI()/4));
        VERIFY (about_equal (x, 0.0));
        VERIFY (about_equal (y, -1.0));
        cart2pol (1.0, -1.0, r, th);
        pol2cart (r, th, x, y);
        VERIFY (about_equal (r, sqrt (2.0)));
        VERIFY (about_equal (th, -1*PI()/4));
        VERIFY (about_equal (x, 1.0));
        VERIFY (about_equal (y, -1.0));
        vector<double> phi (1);
        phi[0] = PI()/3;
        vector<double> cx;
        hyper2cart (1.0, phi, cx);
        // 1st dimension is the y axis
        VERIFY (about_equal (cx[0], 0.5));
        VERIFY (about_equal (cx[1], 0.866));
        phi.resize (2);
        phi[0] = PI()/3;
        phi[1] = PI()/3;
        hyper2cart (1.0, phi, cx);
        VERIFY (about_equal (cx[0], 0.5));
        // 2st dimension is the z axis
        VERIFY (about_equal (cx[1], 0.866/2));
        // 3rd dimension is the x axis
        VERIFY (about_equal (cx[2], 0.75));
        // should be a unit vector
        VERIFY (about_equal (sqrt (cx[0]*cx[0]+cx[1]*cx[1]+cx[2]*cx[2]), 1.0));
        phi[0] = PI()/2;
        phi[1] = 0.0;
        hyper2cart (1.0, phi, cx);
        VERIFY (about_equal (cx[0], 0.0));
        VERIFY (about_equal (cx[1], 1.0));
        VERIFY (about_equal (cx[2], 0.0));
        phi[0] = PI()*13/17;
        phi[1] = PI()*17/19;
        hyper2cart (1.0, phi, cx);
        // should be a unit vector
        VERIFY (about_equal (sqrt (cx[0]*cx[0]+cx[1]*cx[1]+cx[2]*cx[2]), 1.0));

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
