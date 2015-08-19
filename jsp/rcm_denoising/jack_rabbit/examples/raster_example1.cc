/// @file raster_example1.cc
/// @brief raster example
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "jack_rabbit/jack_rabbit.h"
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>

using namespace boost::lambda;
using namespace jack_rabbit;
using namespace std;

// Affine transformation helper
class Affine
{
    public:
    Affine ()
        : t_ (0), s_ (1)
    { }
    // Translate by 't' and scale by 's'
    Affine (float t, float s)
        : t_ (t), s_ (s)
    { }
    float To (size_t x) const
    { return t_ + x * s_; }
    size_t From (float x) const
    { return static_cast<int> ((x - t_) / s_); }
    private:
    float t_, s_;
};

// Mandelbrot function object:
// Map the Mandelbrot set onto a raster
template<typename T>
class Mandelbrot
{
    public:
    Mandelbrot (size_t /*r*/, size_t /*c*/) { }
    // Define coordinate mapping
    void CoordMap (const Affine &ax, const Affine &ay)
    { ax_ = ax; ay_ = ay; }
    // Remap coordinate (r, c), and then determine if it's
    // in the set or not.
    T operator() (size_t r, size_t c) const
    {
        // Remap x and y
        complex<float> z (ax_.To (c), ay_.To (r));
        complex<float> k (z);
        size_t count = 0;
        // Iterate until we are kindof sure that the point
        // is in the set, or until the point is sure to
        // shoot off to infinity...
        while (++count != ITER)
        {
            z = z * z + k;
            if (norm (z) > 4.0)
                // ... definitely not in the set
                return OUT;
        }
        // ... probably in the set.
        return IN;
    }
    private:
    static const size_t ITER = 500;
    static const T IN = 0;
    static const T OUT = 255;
    Affine ax_, ay_;
};

// PGM file writer helper
template<class T>
void WritePGM (const T &m, ofstream &ofs)
{
    // Write a pgm header
    ofs << "P5\n"
        << "# Raster Example\n"
        << m.cols () << ' ' << m.rows () << '\n'
        << "255\n";

    // Transform to a vector of chars
    vector<char> mm (m.begin (), m.end ());

    // Write the pgm pixels
    const std::streamsize sz =
        static_cast<std::streamsize> (mm.size ());
    ofs.write (&mm[0], sz);
}

int main ()
{
    try
    {
        const size_t M = 256;
        const size_t N = 256;

        clog << "Generating a " << M << "X" << N
            << " image..." << endl;

        typedef raster<int> Image;

        // Some matrices that will contain our images
        Image m0 (M, N);
        Image m1 (M / 3, N / 3);

        // Our Mandelbrot functions
        subscript_generator<Image::value_type,Mandelbrot>
        mandel0 (m0.rows (), m0.cols ());
        Affine ax0 (-2.2f, 3.0f / m0.rows ());
        Affine ay0 (-1.7f, 3.0f / m0.rows ());
        mandel0.CoordMap (ax0, ay0);

        subscript_generator<Image::value_type,Mandelbrot>
        mandel1 (m1.rows (), m1.cols ());
        Affine ax1 (-0.28f, 0.30f / m1.rows ());
        Affine ay1 (-0.92f, 0.30f / m1.rows ());
        mandel1.CoordMap (ax1, ay1);

        // Create the main image
        generate (m0.begin (), m0.end (), mandel0);

        // Lower contrast of the region to zoom
        size_t r1 = ay0.From (ay1.To (0));
        size_t c1 = ax0.From (ax1.To (0));
        size_t r2 = ay0.From (ay1.To (m1.rows ()));
        size_t c2 = ax0.From (ax1.To (m1.cols ()));
        subregion s0 = m0.sub (
            static_cast<int> (r1), static_cast<int> (c1),
            r2 - r1, c2 - c1);
        for_each (m0.begin (s0), m0.end (s0),
            _1 = (_1 - 127) / 2 + 127);

        // Create the zoomed image
        generate (m1.begin (), m1.end (), mandel1);
        // Copy the zoomed image to the main image...
        subregion s1 = { m0.rows () / 9, m0.rows () / 9,
            m1.rows (), m1.cols () };
        copy (m1.begin (), m1.end (), m0.begin (s1));

        // Lower contrast of the zoomed image
        transform (m0.begin (s1), m0.end (s1), m0.begin (s1),
            (_1 - 127) / 2 + 127);

        // Save the image to a file
        const string fn ("raster_example1.pgm");
        clog << "Writing image to " << fn << "..." << endl;

        ofstream ofs (fn.c_str ());
        if (!ofs)
            throw std::runtime_error ("Could not open file for writing");

        WritePGM (m0, ofs);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
