/// @file test_fft.cc
/// @brief test fft
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/fft.h"
#include "horny_toad/verify.h"
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;
using namespace horny_toad;

template<typename T,int DIMS,int SZ>
void test_fft (bool verbose)
{
    vector<int> rdims (DIMS, SZ);
    vector<int> cdims (rdims);
    cdims.back () = cdims.back () / 2 + 1;
    if (verbose)
    {
        clog << "real dims: ";
        copy (rdims.begin (), rdims.end (), ostream_iterator<T> (clog, " "));
        clog << endl;
        clog << "complex dims: ";
        copy (cdims.begin (), cdims.end (), ostream_iterator<T> (clog, " "));
        clog << endl;
    }
    size_t rsize = accumulate (rdims.begin (), rdims.end (), 1, multiplies<int> ());
    size_t csize = accumulate (cdims.begin (), cdims.end (), 1, multiplies<int> ());
    if (verbose)
    {
        clog << "real size: " << rsize << endl;
        clog << "complex size: " << csize << endl;
    }
    vector<T> r (rsize);
    vector<complex<T> > c (csize);
    forward_real_fft frfft (
        rdims.begin (), rdims.end (),
        r.begin (), r.end (),
        c.begin (), c.end ());
    inverse_real_fft irfft (
        rdims.begin (), rdims.end (),
        c.begin (), c.end (),
        r.begin (), r.end ());
    // transform
    frfft ();
    // inverse transform
    irfft ();

    forward_complex_fft fcfft (
        cdims.begin (), cdims.end (),
        c.begin (), c.end (),
        c.begin (), c.end ());
    inverse_complex_fft icfft (
        cdims.begin (), cdims.end (),
        c.begin (), c.end (),
        c.begin (), c.end ());
    // complex fft
    fcfft ();
    // inverse complex fft
    icfft ();
}

int main (int argc, char **)
{
    try
    {
        const bool verbose = (argc != 1);
        test_fft<double,1,10> (verbose);
        test_fft<double,1,7> (verbose);
        test_fft<double,5,10> (verbose);
        test_fft<double,4,7> (verbose);
        test_fft<double,2,100> (verbose);
        test_fft<double,2,123> (verbose);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
