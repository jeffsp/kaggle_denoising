// Bicubic Interpolation Example
//
// Copyright (C) 2004-2011
// Center for Perceptual Systems
// University of Texas at Austin
//
// contact: jeffsp@gmail.com

#include "horny_toad/horny_toad.h"
#include "jack_rabbit/jack_rabbit.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace horny_toad;
using namespace jack_rabbit;

int main ()
{
    try
    {
        const size_t M = 16;
        const size_t N = 16;
        const size_t SCALE = 30;

        raster<unsigned char> p (M, N);
        generate (p.begin (), p.end (), rand);
        raster<double> q (M * SCALE, N * SCALE);
        bicubic_interp (p, q);
        raster<unsigned char> u (q.rows (), q.cols ());
        transform (q.begin (), q.end (), q.begin (), (double(*)(double))&round);
        transform (q.begin (), q.end (), u.begin (), clip_functor<int> (0, 255));

        raster<unsigned char> ind (u.rows () - 3 * SCALE * 2, u.cols () - 3 * SCALE * 2);
        subregion s = { 3 * SCALE, 3 * SCALE, ind.rows (), ind.cols () };
        copy (u.begin (s), u.end (s), ind.begin ());

        double pal[96] = {
            1.0000,         0,         0,
            1.0000,    0.1875,         0,
            1.0000,    0.3750,         0,
            1.0000,    0.5625,         0,
            1.0000,    0.7500,         0,
            1.0000,    0.9375,         0,
            0.8750,    1.0000,         0,
            0.6875,    1.0000,         0,
            0.5000,    1.0000,         0,
            0.3125,    1.0000,         0,
            0.1250,    1.0000,         0,
                 0,    1.0000,    0.0625,
                 0,    1.0000,    0.2500,
                 0,    1.0000,    0.4375,
                 0,    1.0000,    0.6250,
                 0,    1.0000,    0.8125,
                 0,    1.0000,    1.0000,
                 0,    0.8125,    1.0000,
                 0,    0.6250,    1.0000,
                 0,    0.4375,    1.0000,
                 0,    0.2500,    1.0000,
                 0,    0.0625,    1.0000,
            0.1250,         0,    1.0000,
            0.3125,         0,    1.0000,
            0.5000,         0,    1.0000,
            0.6875,         0,    1.0000,
            0.8750,         0,    1.0000,
            1.0000,         0,    0.9375,
            1.0000,         0,    0.7500,
            1.0000,         0,    0.5625,
            1.0000,         0,    0.3750,
            1.0000,         0,    0.1875
        };

        raster<unsigned char> c (ind.rows (), ind.cols () * 3);
        for (size_t i = 0; i < ind.size (); ++i)
        {
            c[i * 3 + 0] = pal[ind[i]/8*3+0] * 255;
            c[i * 3 + 1] = pal[ind[i]/8*3+1] * 255;
            c[i * 3 + 2] = pal[ind[i]/8*3+2] * 255;
        }
        write_pnm (cout, c.cols () / 3, c.rows (), c, true);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
