/// @file reduce.cc
/// @brief reduce
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "argv.h"
#include "pnm.h"
#include "pyramid.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace horny_toad;
using namespace jack_rabbit;
using namespace std;

void ReadHeader (istream &s, bool &rgb, bool &bpp16, size_t &w, size_t &h)
{
    read_pnm_header (s, rgb, bpp16, w, h);
    cerr << w << "x" << h << " " << (bpp16 ? "16bpp" : "8bpp") << " " << (rgb ? "rgb" : "grayscale") << endl;

    if (rgb)
        throw runtime_error ("Only grayscale images are supported");
}

template <typename T>
void Process (T &pyramid,
        unsigned kernel,
        unsigned expand_levels,
        unsigned input_level,
        unsigned output_level)
{
    cerr << pyramid.levels () << " levels" << endl;

    if (output_level >= pyramid.levels ())
        throw runtime_error ("invalid output level specified");
    const unsigned top_level = output_level + expand_levels;
    if (top_level >= pyramid.levels ())
        throw runtime_error ("invalid expand levels specified");

    for (unsigned i = input_level; i < top_level; ++i)
    {
        cerr << "reducing from level " << i << " to " << i + 1 << endl;
        switch (kernel)
        {
            case 2: pyramid.reduce2x2 (i); break;
            case 3: pyramid.reduce3x3 (i); break;
            case 4: pyramid.reduce2x2 (i); break;
            default: throw runtime_error ("invalid kernel parameter");
        }
    }

    for (unsigned i = top_level; i != output_level; --i)
    {
        cerr << "expanding from level " << i << " to " << i - 1 << endl;
        switch (kernel)
        {
            case 2: pyramid.expand2x2 (i); break;
            case 3: pyramid.expand3x3 (i); break;
            case 4: pyramid.expand2x2_and_blur3x3 (i); break;
            default: throw runtime_error ("invalid kernel parameter");
        }
    }
}

int main (int argc, char **argv)
{
    try
    {
        bool help = false;
        string inputfn;
        unsigned kernel = 3;
        unsigned expand_levels = 0;
        unsigned input_level = 0;
        unsigned output_level = 1;

        CommandLine cl;
        cl.AddSpec ("help", 'h', help, "Print help message");
        cl.AddSpec ("kernel", 'k', kernel, "Kernel size (2,3,4)[3]", "#", "#");
        cl.AddSpec ("expand_levels", 'x', expand_levels, "Expand levels (1-n)[0]", "#", "#");
        cl.AddSpec ("input_level", 'i', input_level, "Input level (0-n)[0]", "#", "#");
        cl.AddSpec ("output_level", 'l', output_level, "Output level (0-n)[1]", "#", "#");
        cl.AddSpec ("", '\0', inputfn, "Input filename", "[FN]");
        // Group argv's into option groups
        cl.GroupArgs (argc, argv, 1);
        // Convert from strings to their proper type
        cl.ExtractBegin ();
        cl.Extract (help);
        cl.Extract (kernel);
        cl.Extract (expand_levels);
        cl.Extract (input_level);
        cl.Extract (output_level);
        cl.Extract (inputfn);
        cl.ExtractEnd ();

        if (help)
        {
            cerr << argv[0] << " " << cl.Usage ();
            cerr << std::endl;
            cerr << cl.Help ();
            return 0;
        }
        if (!cl.GetLeftOverArgs ().empty ())
            throw runtime_error ("usage: " + string (argv[0]) + " " + cl.Usage ());

        bool rgb, bpp16;
        size_t w, h;
        raster<unsigned char> buffer;

        if (inputfn.empty ())
        {
            cerr << "Reading from stdin" << endl;
            ReadHeader (cin, rgb, bpp16, w, h);
            buffer.resize (h, w * (bpp16 ? 2 : 1));
            read_pnm_pixels (cin, buffer);
        }
        else
        {
            cerr << "Reading " << inputfn << endl;
            ifstream ifs (inputfn.c_str ());
            if (!ifs)
                throw runtime_error ("Could not open file for reading");
            ReadHeader (ifs, rgb, bpp16, w, h);
            buffer.resize (h, w * (bpp16 ? 2 : 1));
            read_pnm_pixels (ifs, buffer);
        }

        size_t base_cols = w << input_level;
        size_t base_rows = h << input_level;
        cerr << "Creating " << base_cols << "x" << base_rows << " pyramid" << endl;

        if (bpp16)
        {
            pyramid<unsigned short> pyramid (base_rows, base_cols);

            assert (pyramid[input_level].size () * 2 == buffer.size ());
            convert_pnm_8bit_to_16bit (buffer, pyramid[input_level]);

            //cerr << "min: " << *min_element (pyramid[input_level].begin (), pyramid[input_level].end ()) << endl;
            //cerr << "max: " << *max_element (pyramid[input_level].begin (), pyramid[input_level].end ()) << endl;

            Process (pyramid,
                kernel,
                expand_levels,
                input_level,
                output_level);

            cerr << "writing 16bpp pgm to stdout" << endl;
            const raster<unsigned short> &p = pyramid[output_level];

            //cerr << "min: " << *min_element (p.begin (), p.end ()) << endl;
            //cerr << "max: " << *max_element (p.begin (), p.end ()) << endl;

            write_pnm (cout, p.cols (), p.rows (), p, false, true);
        }
        else
        {
            pyramid<unsigned char> pyramid (base_rows, base_cols);

            assert (pyramid[input_level].size () == buffer.size ());
            ifstream ifs (inputfn.c_str ());
            if (!ifs)
                throw runtime_error ("Could not open file for reading");
            pyramid[input_level] = buffer;

            //cerr << "min: " << int(*min_element (pyramid[input_level].begin (), pyramid[input_level].end ())) << endl;
            //cerr << "max: " << int(*max_element (pyramid[input_level].begin (), pyramid[input_level].end ())) << endl;

            Process (pyramid,
                kernel,
                expand_levels,
                input_level,
                output_level);

            cerr << "writing 8bpp pgm to stdout" << endl;
            const raster<unsigned char> &p = pyramid[output_level];
            //cerr << "min: " << int(*min_element (p.begin (), p.end ())) << endl;
            //cerr << "max: " << int(*max_element (p.begin (), p.end ())) << endl;
            write_pnm (cout, p.cols (), p.rows (), p, false, false);
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
