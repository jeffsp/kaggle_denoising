/// @file test_mlr.cc
/// @brief test multiple linear regression
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-06-13

#include "horny_toad/horny_toad.h"

using namespace std;
using namespace horny_toad;
using namespace jack_rabbit;

typedef raster<double> matrix;

void test_mlr (bool verbose)
{
    // see http://en.wikipedia.org/wiki/Ordinary_least_squares
    // number of observations
    const size_t N = 100000;
    // number of predictors
    const size_t P = 5;
    // linear parameters
    const double b[P] = { 0.1, 0.4, 0.05, 0.2, 0.3 };
    // predictors: random values in [0,9]
    matrix x (N, P);
    for (auto &i : x)
        i = rand () % 10;
    // noise
    matrix e (N, P);
    vector<double> n = gaussian_noise<double> (P * N, 123);
    assert (e.size () == n.size ());
    copy (n.begin (), n.end (), e.begin ());
    // responses
    matrix y (N, 1);
    // y = x * b + e
    for (size_t i = 0; i < x.rows (); ++i)
    {
        // get the responses plus added noise
        for (size_t j = 0; j < x.cols (); ++j)
            y (i, 0) += x (i, j) * b[j] + 0.05 * e (i, j);
        // add an offset
        y (i, 0) += 42.0;
    }
    // Try to recover b
    matrix bhat = mlr_inverse (y, x);
    if (verbose)
    {
        for (auto i : b) cout << " " << i;
        cout << endl;
        for (auto i : bhat) cout << " " << i;
        cout << endl;
    }
    // the offset is in the first term
    VERIFY (about_equal (42.0, bhat[0], 0.01));
    for (size_t i = 1; i < bhat.size (); ++i)
        VERIFY (about_equal (b[i - 1], bhat[i]));
    /*
    // Try to recover b, with lapack version
    bhat = mlr_lapack (y, x);
    if (verbose)
    {
        for (auto i : b) cout << " " << i;
        cout << endl;
        for (auto i : bhat) cout << " " << i;
        cout << endl;
    }
    VERIFY (about_equal (42.0, bhat[0], 0.01));
    for (size_t i = 1; i < bhat.size (); ++i)
        VERIFY (about_equal (b[i - 1], bhat[i]));
    */
}

int main (int argc, char **)
{
    try
    {
        const bool verbose = (argc != 1);
        test_mlr (verbose);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
