/// @file denoise.cc
/// @brief denoising
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-11-25

#include <chrono>
#include "denoise.h"

using namespace horny_toad;
using namespace jack_rabbit;
using namespace denoise;
using namespace opp;
using namespace std;

const string usage = "usage: denoise fn.lut < fn";

int main (int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw runtime_error (usage);

        // read codec
        clog << "reading " << argv[1] << endl;
        ifstream ifs (argv[1]);

        if (!ifs)
            throw runtime_error ("Could not read lut");

        multi_codec<PASSES> c;
        ifs >> c;

        // read image
        image_t p = read_grayscale (cin);

        const unsigned BORDER = 32;
        p = mborder<subregion> (p, BORDER);

        image_t q = c.denoise (p);

        // crop borders
        q = crop (q, BORDER);

        // save the fixed image
        clog << "writing image" << endl;
        write_pnm (cout, q.cols (), q.rows (), q);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
