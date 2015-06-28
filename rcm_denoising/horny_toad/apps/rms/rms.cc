// RMS
//
// Copyright (C) 2009
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Feb 26 11:01:00 CST 2009

#include "argv.h"
#include "pnm.h"
#include "raster.h"
#include "raster_utils.h"
#include "subscript_function.h"
#include <boost/lambda/lambda.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace boost::lambda;
using namespace horny_toad;
using namespace jack_rabbit;
using namespace std;

int main (int argc, char **argv)
{
    try
    {
        bool help = false;
        bool verbose = false;
        string fn;
        unsigned wsize = 33;
        bool mean = false;

        CommandLine cl;
        cl.AddSpec ("help", 'h', help, "Print help message");
        cl.AddSpec ("verbose", 'v', verbose, "Be verbose");
        cl.AddSpec ("wsize", 'w', wsize, "Weighting function size[33]", "#", "#");
        cl.AddSpec ("mean", 'm', mean, "Only output mean");
        cl.AddSpec ("", '\0', fn, "Input filename", "[FN]");
        // Group argv's into option groups
        cl.GroupArgs (argc, argv, 1);
        // Convert from strings to their proper type
        cl.ExtractBegin ();
        cl.Extract (help);
        cl.Extract (verbose);
        cl.Extract (wsize);
        cl.Extract (mean);
        cl.Extract (fn);
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
        raster<unsigned char> img;
        if (fn.empty ())
        {
            cerr << "Reading from stdin" << endl;
            size_t bpp, rows, cols;
            read_pnm_header (cin, bpp, cols, rows);
            if (verbose)
                clog << cols << "x" << rows << "x" << bpp << endl;
            if (bpp != 1)
                throw runtime_error ("The image must be 8 bits/pixel");
            if (verbose)
                clog << "Reading PNM pixels..." << endl;
            img.resize (rows, cols);
            read_pnm_pixels (cin, img);
        }
        else
        {
            cerr << "Reading " << fn << endl;
            if (verbose)
                clog << "Opening " << fn << endl;
            ifstream ifs (fn.c_str ());
            if (!ifs)
                throw runtime_error ("Could not open file for reading");
            if (verbose)
                clog << "Reading PNM header..." << endl;
            size_t bpp, rows, cols;
            read_pnm_header (ifs, bpp, cols, rows);
            if (verbose)
                clog << cols << "x" << rows << "x" << bpp << endl;
            if (bpp != 1)
                throw runtime_error ("The image must be 8 bits/pixel");
            if (verbose)
                clog << "Reading PNM pixels..." << endl;
            img.resize (rows, cols);
            read_pnm_pixels (ifs, img);
        }

        // Create a weighting function
        if (verbose)
        clog << "Computing weighting function..." << endl;
        raster<double> w (wsize, wsize);
        subscript_generator<double,raised_cos> f (w.rows (), w.cols ());
        generate (w.begin (), w.end (), f);
        double w_sum = std::accumulate (w.begin (), w.end (), 0.0);
        for_each (w.begin (), w.end (),  _1 = _1 / w_sum);
        if (verbose)
        {
            clog << "Weighting function:" << endl;
            print2d (clog, w);
        }
        raster<double> r;
        if (mean)
        {
            clog << "Computing mean..." << endl;
            r = convolve<subregion> (img, w);
        }
        else
        {
            clog << "Computing RMS contrast..." << endl;
            r = rms_contrast<subregion> (img, w, 1.0);
        }

        print2d (cout, r);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
