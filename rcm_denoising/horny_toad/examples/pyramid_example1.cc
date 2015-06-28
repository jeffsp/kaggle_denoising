// Pyramid Example
//
// Copyright (C) 2007 Jeffrey S. Perry
//
// This program is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//
// contact: jeffsp@gmail.com

#include "horny_toad/pyramid.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace horny_toad;
using namespace std;

// PNM file writer helper
template<class T>
void WritePNM (const T &m, ofstream &ofs, bool rgb = false)
{
    // Write a pnm header
    ofs << (rgb ? "P6" : "P5") << "\n";
    ofs << "# Pyramid Example\n";
    ofs << (rgb ? m.cols () / 3 : m.cols ()) << ' ' << m.rows () << '\n';
    ofs << "255\n";

    // Transform to a vector of chars
    vector<char> mm (m.begin (), m.end ());

    // Write the pnm pixels
    const std::streamsize sz =
        static_cast<std::streamsize> (mm.size ());
    ofs.write (&mm[0], sz);
}

// Convert from YUV colorspace to RGB colorspace
template<typename Ty>
inline float YUVR (Ty y, Ty , Ty v)
{ return  y + 1.14 * (v - 127.5); }
template<typename Ty>
inline float YUVG (Ty y, Ty u, Ty v)
{ return  y - 0.39 * (u - 127.5) - 0.58 * (v - 127.5); }
template<typename Ty>
inline float YUVB (Ty y, Ty u, Ty )
{ return  y + 2.03 * (u - 127.5); }
template<typename Ty>
inline unsigned char CLIP (Ty x)
{ return static_cast<unsigned char> (x > 255 ? 255 : (x < 0 ? 0 : x)); }

template<class T>
T yuv2rgb (const T &y, const T &u, const T &v)
{
    T rgb (y.rows (), y.cols () * 3);
    for (size_t i = 0; i < y.rows (); ++i) {
        for (size_t j = 0; j < y.cols (); ++j) {
            rgb (i, j * 3 + 0) = CLIP (YUVR (y (i, j), u (i, j), v (i, j)));
            rgb (i, j * 3 + 1) = CLIP (YUVG (y (i, j), u (i, j), v (i, j)));
            rgb (i, j * 3 + 2) = CLIP (YUVB (y (i, j), u (i, j), v (i, j)));
        }
    }
    return rgb;
}

int main ()
{
    try
    {
        const size_t M = 240;
        const size_t N = 320;

        clog << "Generating a " << M << "X" << N
            << " image..." << endl;

        pyramid<unsigned char> py (M, N);
        pyramid<unsigned char> pu (M, N);
        pyramid<unsigned char> pv (M, N);

        // Generate some 1/f-like noise...
        //
        // Starting at the top of the pyramid...
        for (size_t l = py.levels () - 1; l > 0; --l) {
            // Randomize some pixels in this level
            const int N = py[l].cols () / 8 + 1;
            for (size_t i = 0; i < py[l].rows (); ++i) {
                for (size_t j = 0; j < py[l].cols (); ++j) {
                    if (!(rand () % N)) {
                        py[l](i, j) = rand ();
                        pu[l](i, j) = rand ();
                        pv[l](i, j) = rand ();
                    }
                }
            }
            // Expand to the next level
            py.expand3x3 (l);
            pu.expand3x3 (l);
            pv.expand3x3 (l);
        }

        // Save the image to a file
        const string fn("pyramid_example1_grayscale.pgm");
        clog << "Writing image to " << fn << "..." << endl;

        ofstream ofs (fn.c_str ());
        if (!ofs)
            throw std::runtime_error ("Could not open file for writing");

        WritePNM (py[0], ofs);

        // Save the image to another file
        const string fn_color("pyramid_example1_color.ppm");
        clog << "Writing image to " << fn_color << "..." << endl;

        ofstream ofs_color (fn_color.c_str ());
        if (!ofs_color)
            throw std::runtime_error ("Could not open file for writing");

        WritePNM (yuv2rgb (py[0], pu[0], pv[0]), ofs_color, true);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
