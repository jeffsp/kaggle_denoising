#include "denoise.h"

using namespace opp;
using namespace std;
using namespace denoise;
using namespace horny_toad;

const string usage = "usage: mklut < file_list.txt > fn.lut";

int main (int argc, char **)
{
    try
    {
        if (argc != 1)
            throw runtime_error (usage);
        vector<string> fns = horny_toad::readwords<string> (cin);
        clog << fns.size () << " files to process" << endl;
        if (fns.size () % 2)
            throw runtime_error ("you must supply an even number of file names");
        multi_codec<PASSES> c;
        for (size_t pass = 0; pass < c.lut_passes (); ++pass)
        {
            size_t k = fns.size () / 2;
#pragma omp parallel for schedule (dynamic)
            for (size_t n = 0; n < fns.size (); n += 2)
            {
#pragma omp critical
                clog << "pass " << pass+1 << "/" << c.lut_passes () << " " << k--
                    << " processing " << fns[n]
                    << " " << fns[n + 1] << endl;
                image_t p = read_grayscale (fns[n].c_str ());
                image_t q = read_grayscale (fns[n + 1].c_str ());
                c.update (p, q, pass);
            }
        }
        clog << "writing lut" << endl;
        cout << c;
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
