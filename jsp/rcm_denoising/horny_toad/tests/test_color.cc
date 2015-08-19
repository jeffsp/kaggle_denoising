/// @file test_color.cc
/// @brief test color
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/verify.h"
#include "horny_toad/color.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace horny_toad;

typedef double (*F) (double a, double b, double c);

void test1 (F a, F b, F c, F x, F y, F z, double v1 = 1.0, double v2 = 1.0, double v3 = 1.0)
{
    // Convert from one colorspace to another and back
    double _x = a (v1, v2, v3);
    double _y = b (v1, v2, v3);
    double _z = c (v1, v2, v3);
    //cout << "X: " << _x << endl;
    //cout << "Y: " << _y << endl;
    //cout << "Z: " << _z << endl;
    double _a = x (_x, _y, _z);
    double _b = y (_x, _y, _z);
    double _c = z (_x, _y, _z);
    //cout << "A: " << _a << endl;
    //cout << "B: " << _b << endl;
    //cout << "C: " << _c << endl;
    VERIFY (round (_a) == v1);
    VERIFY (round (_b) == v2);
    VERIFY (round (_c) == v3);
}

void test2 ()
{
    for (double a = 0.0; a < 1.0; a += 0.1)
    {
        //cout << "a: " << a << endl;
        double x = sRGBtoRGB709 (a);
        //cout << "x: " << x << endl;
        x = RGB709tosRGB (x);
        //cout << "x: " << x << endl;
        VERIFY (round (x * 100) == round (a * 100));
    }
}

void test3 ()
{
    for (double x = 0.0; x <= 1.0; x += 0.1)
    {
        double y = x;
        double z = x;
        //cout << "x: " << x << endl;
        //cout << "y: " << y << endl;
        //cout << "z: " << z << endl;
        double l_ = CIE1976LabL (x, y, z);
        double a_ = CIE1976Laba (x, y, z);
        double b_ = CIE1976Labb (x, y, z);
        //cout << "l: " << l_;
        //cout << " a: " << a_;
        //cout << " b: " << b_ << endl;
        double newx = CIE1976LabX (l_, a_, b_);
        double newy = CIE1976LabY (l_, a_, b_);
        double newz = CIE1976LabZ (l_, a_, b_);
        //cout << "newx: " << newx << endl;
        //cout << "newy: " << newy << endl;
        //cout << "newz: " << newz << endl;
        VERIFY (round (x * 100) == round (newx * 100));
        VERIFY (round (y * 100) == round (newy * 100));
        VERIFY (round (z * 100) == round (newz * 100));
    }
}

void test4 ()
{
    vector<double> rs, gs, bs;
    rs.push_back (0.0); gs.push_back (0.0); bs.push_back (0.0);
    rs.push_back (1.0); gs.push_back (0.0); bs.push_back (0.0);
    rs.push_back (0.0); gs.push_back (1.0); bs.push_back (0.0);
    rs.push_back (0.0); gs.push_back (0.0); bs.push_back (1.0);
    rs.push_back (1.0); gs.push_back (1.0); bs.push_back (0.0);
    rs.push_back (0.0); gs.push_back (1.0); bs.push_back (1.0);
    rs.push_back (1.0); gs.push_back (0.0); bs.push_back (1.0);
    rs.push_back (1.0); gs.push_back (1.0); bs.push_back (1.0);
    for (size_t i = 0; i < rs.size (); ++i)
    {
        double x = RGB709X (sRGBtoRGB709 (rs[i]), sRGBtoRGB709 (gs[i]), sRGBtoRGB709 (bs[i]));
        double y = RGB709Y (sRGBtoRGB709 (rs[i]), sRGBtoRGB709 (gs[i]), sRGBtoRGB709 (bs[i]));
        double z = RGB709Z (sRGBtoRGB709 (rs[i]), sRGBtoRGB709 (gs[i]), sRGBtoRGB709 (bs[i]));
        double l = CIE1976LabL (x, y, z);
        double a = CIE1976Laba (x, y, z);
        double b = CIE1976Labb (x, y, z);
        double l2, a2, b2;
        sRGBtoLab (static_cast<unsigned char> (rs[i] * 255),
            static_cast<unsigned char> (gs[i] * 255),
            static_cast<unsigned char> (bs[i] * 255), &l2, &a2, &b2);
        //cout << "x: " << x;
        //cout << "\ty: " << y;
        //cout << "\tz: " << z;
        //cout << "\tl: " << l;
        //cout << "\ta: " << a;
        //cout << "\tb: " << b << endl;
        // L's extent is [0,100]
        // a and b's extents are about [-100,100]
        VERIFY (l <= 100);
        VERIFY (l >= 0);
        VERIFY (a <= 110);
        VERIFY (a >= -110);
        VERIFY (b <= 110);
        VERIFY (b >= -110);
        VERIFY (round (l) == round (l2));
        VERIFY (round (a) == round (a2));
        VERIFY (round (b) == round (b2));
    }
}

int main ()
{
    try
    {
        // rgb<->YCbCr
        test1 (YCbCr709Y, YCbCr709Cb, YCbCr709Cr, YCbCr709R, YCbCr709G, YCbCr709B);
        test1 (YCbCr709Y, YCbCr709Cb, YCbCr709Cr, YCbCr709R, YCbCr709G, YCbCr709B, 0.0, 0.0, 0.0);
        // rgb<->YPbPr
        test1 (YPbPrY, YPbPrPb, YPbPrPr, YPbPrR, YPbPrG, YPbPrB);
        test1 (YPbPrY, YPbPrPb, YPbPrPr, YPbPrR, YPbPrG, YPbPrB, 0.0, 0.0, 0.0);
        // rgb<->XYZ
        test1 (RGB709R, RGB709G, RGB709B, RGB709X, RGB709Y, RGB709Z);
        test1 (RGB709R, RGB709G, RGB709B, RGB709X, RGB709Y, RGB709Z, 0.0, 0.0, 0.0);

        // XYZ<->L*a*b*
        test1 (CIE1976LabL, CIE1976Laba, CIE1976Labb, CIE1976LabX, CIE1976LabY, CIE1976LabZ);
        test1 (CIE1976LabL, CIE1976Laba, CIE1976Labb, CIE1976LabX, CIE1976LabY, CIE1976LabZ, 0.0, 0.0, 0.0);

        test2 ();
        test3 ();
        test4 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
