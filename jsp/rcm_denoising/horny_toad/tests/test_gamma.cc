/// @file test_gamma.cc
/// @brief test gamma
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/gamma.h"
#include "horny_toad/verify.h"
#include <iostream>

using namespace std;
using namespace horny_toad;

int main ()
{
    try
    {
        vector<unsigned short> lut = sRGB_gamma_curve ();
        for (size_t i = 0; i < lut.size (); ++i)
            VERIFY (i <= lut[i]);
        VERIFY (lut[0] == 0);
        VERIFY (lut[100] == round (100 * 12.92));
        VERIFY (lut.back () == 65535);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
