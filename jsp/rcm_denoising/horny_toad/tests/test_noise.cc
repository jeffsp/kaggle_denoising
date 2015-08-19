/// @file test_noise.cc
/// @brief test noise
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-15

#include "horny_toad/about_equal.h"
#include "horny_toad/noise.h"
#include "horny_toad/statistics.h"
#include "horny_toad/verify.h"

using namespace std;
using namespace horny_toad;

void test_noise ()
{
    const size_t N = 1000000;
    vector<float> n1 = gaussian_noise<float> (N, 123);
    vector<float> n2 = gaussian_noise<float> (N, 123);
    vector<float> n3 = gaussian_noise<float> (N, 321);
    VERIFY (n1 == n2);
    VERIFY (n1 != n3);
    VERIFY (about_equal (mean (n1), 0.0, 0.01));
    VERIFY (about_equal (mean (n2), 0.0, 0.01));
    VERIFY (about_equal (mean (n3), 0.0, 0.01));
    VERIFY (about_equal (stdev (n1), 1.0, 0.01));
    VERIFY (about_equal (stdev (n2), 1.0, 0.01));
    VERIFY (about_equal (stdev (n3), 1.0, 0.01));
    vector<double> l1 = laplace_noise<double> (N, 123);
    vector<double> l2 = laplace_noise<double> (N, 123);
    vector<double> l3 = laplace_noise<double> (N, 321);
    VERIFY (l1 == l2);
    VERIFY (l1 != l3);
    VERIFY (about_equal (mean (l1), 0.0, 0.01));
    VERIFY (about_equal (mean (l2), 0.0, 0.01));
    VERIFY (about_equal (mean (l3), 0.0, 0.01));
    VERIFY (about_equal (stdev (l1), 1.0, 0.01));
    VERIFY (about_equal (stdev (l2), 1.0, 0.01));
    VERIFY (about_equal (stdev (l3), 1.0, 0.01));
}

int main ()
{
    try
    {
        test_noise ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
