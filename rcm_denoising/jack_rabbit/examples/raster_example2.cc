/// @file raster_example2.cc
/// @brief raster example
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "jack_rabbit/jack_rabbit.h"
#include <algorithm>
#include <boost/static_assert.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace jack_rabbit;
using namespace std;

// Our video display mode dimensions.
const size_t WIDTH = 320;
const size_t HEIGHT = 240;

// An RGB pixel
struct Pixel
{
    char rgb[3];
};

// Imagine that some sort of a display class, like this one,
// provides access to the video display buffer.  We can
// interface with the video buffer using a raster if we
// utilize a simple custom allocator, defined below.
template <
    size_t W,
    size_t H
>
class Display
{
    public:
    Display () : p_ (W * H) { }
    Pixel *Pixels () { return &p_[0]; }
    size_t Width () const { return W; }
    size_t Height () const { return H; }
    private:
    vector<Pixel> p_;
};
Display<WIDTH,HEIGHT> display;

// Our custom allocator does not allocate any memory, but
// instead it returns an address to the video display
// memory.  The container that uses this allocator might not
// ask for *exactly* the number of bytes specified in the
// raster constructor.  If this happens, allocate will fail.
template<class T>
class Alloc : public allocator<T>
{
    public:
    T *allocate (size_t n, const void *hint = 0)
    {
        if (n != display.Width () * display.Height ())
            throw runtime_error ("allocate failed");
        return display.Pixels ();
    }
    void deallocate (Pixel *p, size_t n) { }
    private:
};

// Convert from YUV colorspace to RGB colorspace
template<typename Ty>
inline Ty YUVR (Ty y, Ty  , Ty v) { return  y + 1.14 * v; }
template<typename Ty>
inline Ty YUVG (Ty y, Ty u, Ty v) { return  y - 0.40 * u - 0.58 * v; }
template<typename Ty>
inline Ty YUVB (Ty y, Ty u, Ty  ) { return  y + 2.03 * u; }
template<typename Ty>
inline Ty CLIP (Ty x) { return x > 1.0 ? 1.0 : (x < 0.0 ? 0.0 : x); }

// Our subscript function object writes a chromaticity
// diagram of YUV values.  The diagram is white in the
// center and smoothly changes color from green to red, then
// purple, blue, and back to green as it goes around the
// edge of the diagram.
template<typename T>
struct Func
{
    Func (size_t /*r*/, size_t /*c*/) :
        MAXD (sqrt (static_cast<double> (CX * CX + CY * CY)))
    { }
    T operator() (size_t r, size_t c)
    {
        // Luminance is determined by distance from center
        double dx = static_cast<double> (c) - CX;
        double dy = static_cast<double> (r) - CY;
        double l = 1.0 - sqrt (dx * dx + dy * dy) / MAXD;
        // Color is determined by coordinate
        double v = static_cast<double> (c) / W - 0.5;
        double u = static_cast<double> (r) / H - 0.5;
        Pixel p;
        // Convert YUV to RGB
        p.rgb[0] = static_cast<char> (CLIP (YUVR (l, u, v)) * 255);
        p.rgb[1] = static_cast<char> (CLIP (YUVG (l, u, v)) * 255);
        p.rgb[2] = static_cast<char> (CLIP (YUVB (l, u, v)) * 255);
        return p;
    }
    static const int W = WIDTH;
    static const int H = HEIGHT;
    static const int CX = (W - 1) / 2;
    static const int CY = (H - 1) / 2;
    const double MAXD;
};

// PPM file writer helper
template<class T>
void WritePPM (const T &m, ofstream &ofs)
{
    // Write a ppm header
    ofs << "P6\n"
        << "# Raster Example\n"
        << m.cols () << ' ' << m.rows () << '\n'
        << "255\n";

    // Write the ppm pixels
    const std::streamsize sz =
        static_cast<std::streamsize> (m.size () * 3);
    ofs.write (reinterpret_cast<const char *> (&m[0]), sz);
}

int main ()
{
    try
    {
        BOOST_STATIC_ASSERT (sizeof (Pixel) == 3);

        // Access the display through a raster
        raster<Pixel> pixels (display.Height (), display.Width (), Pixel (), Alloc<Pixel> ());

        // Note that functor expects rows X cols
        subscript_generator<Pixel,Func> f (display.Height (), display.Width ());

        // Make a colorful display
        generate (pixels.begin (), pixels.end (), f);

        // Save the image to a file
        const string fn ("raster_example2.ppm");
        clog << "Writing image to " << fn << "..." << endl;

        ofstream ofs (fn.c_str ());
        if (!ofs)
            throw std::runtime_error ("Could not open file for writing");

        WritePPM (pixels, ofs);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
