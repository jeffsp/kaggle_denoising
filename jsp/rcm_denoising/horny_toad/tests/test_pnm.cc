/// @file test_pnm.cc
/// @brief test utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/pnm.h"
#include "horny_toad/verify.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace horny_toad;

void test_pnm1 ()
{
    const size_t W = 10;
    const size_t H = 10;
    vector<unsigned char> m1 (W * H, 0xCC);
    stringstream os (stringstream::out | stringstream::binary);
    write_pnm (os, W, H, m1);
    stringstream is (os.str (), stringstream::in | stringstream::binary);
    vector<unsigned char> m2;
    size_t bpp, w, h;
    read_pnm_header (is, bpp, w, h);
    m2.resize (w * h * bpp);
    read_pnm_pixels (is, m2);
    VERIFY (m1 == m2);
}

void test_pnm2 ()
{
    const size_t W = 255;
    const size_t H = 255;
    vector<unsigned short> m1 (W * H);
    for (size_t i = 0; i < m1.size (); ++i)
        m1[i] = i;
    stringstream os (stringstream::out | stringstream::binary);
    write_pnm (os, W, H, m1, false, true);
    stringstream is (os.str (), stringstream::in | stringstream::binary);
    vector<unsigned char> m2;
    size_t w, h;
    bool rgb, bpp16;
    read_pnm_header (is, rgb, bpp16, w, h);
    VERIFY (!rgb);
    VERIFY (bpp16);
    m2.resize (w * h * 2);
    read_pnm_pixels (is, m2);
    vector<unsigned short> m3 (w * h);
    convert_pnm_8bit_to_16bit (m2, m3);
    VERIFY (m1 == m3);
}

int main ()
{
    try
    {
        test_pnm1 ();
        test_pnm2 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
