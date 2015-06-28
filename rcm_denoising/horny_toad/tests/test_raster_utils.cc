/// @file test_raster_utils.cc
/// @brief test raster utils
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/about_equal.h"
#include "jack_rabbit/raster.h"
#include "horny_toad/raster_utils.h"
#include "jack_rabbit/subscript_function.h"
#include "horny_toad/verify.h"
#include <numeric>
#include <boost/lambda/lambda.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <iostream>

using namespace boost;
using namespace boost::lambda;
using namespace horny_toad;
using namespace jack_rabbit;
using namespace std;

void test_convenience ()
{
    rotation<double> r (radians (90));
    VERIFY (about_equal (r.fx (1, 0), 0.0));
    VERIFY (about_equal (r.fy (1, 0), 1.0));
    VERIFY (about_equal (r.fx (0, 1), -1.0));
    VERIFY (about_equal (r.fy (0, 1), 0.0));
}

void test_aperture (bool verbose)
{
    raster<double> m (7, 9);
    subscript_generator<double,aperture> a (m.rows (), m.cols ());
    generate (m.begin (), m.end (), a);
    if (verbose)
        print2d (clog, m);
    VERIFY (m.front () == 0.0); // outside
    VERIFY (m (m.rows () / 2, m.cols () / 2) == 1.0); // inside
    VERIFY (m.back () == 0.0); // outside
    subscript_unary_function<double,aperture> aa (m.rows (), m.cols ());
    aa.ramp (1.0);
    transform (m.begin (), m.end (), m.begin (), aa);
    if (verbose)
        print2d (clog, m);
    VERIFY (m.front () == 0.0); // outside
    size_t on_edge = static_cast<size_t> (m.cols () / 2 - a.radius () + 1);
    VERIFY (m (m.rows () / 2, on_edge) < 1.0);
    VERIFY (m (m.rows () / 2, on_edge) > 0.0);
    VERIFY (m.back () == 0.0); // outside
    if (verbose)
    {
        clog << "aperture" << endl;
        raster<double> m (600, 800, 1.0);
        subscript_unary_function<double,aperture> a (m.rows (), m.cols ());
        a.ramp (10.0);
        transform (m.begin (), m.end (), m.begin (), a);
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 127.5));
        transform (m.begin (), m.end (), m.begin (), bind2nd (plus<double> (), 127.5));
        //Display (m);
    }
}

void test_oriented_sin (bool verbose)
{
    raster<double> m (5, 5, 1.0);
    subscript_unary_function<double,oriented_sin> g (m.rows (), m.cols ());
    transform (m.begin (), m.end (), m.begin (), g);
    if (verbose)
        print2d (clog, m);
    VERIFY (about_equal (m (0, 0), 0.0));
    VERIFY (about_equal (m (0, 1), -1.0));
    VERIFY (about_equal (m (0, 2), 0.0));
    VERIFY (about_equal (m (0, 3), 1.0));
    VERIFY (about_equal (m (0, 4), 0.0));
    g.orientation (radians (90));
    m.assign (1.0);
    transform (m.begin (), m.end (), m.begin (), g);
    if (verbose)
        print2d (clog, m);
    VERIFY (about_equal (m (0, 0), 0.0));
    VERIFY (about_equal (m (1, 0), 1.0));
    VERIFY (about_equal (m (2, 0), 0.0));
    VERIFY (about_equal (m (3, 0), -1.0));
    VERIFY (about_equal (m (4, 0), 0.0));

    if (verbose)
    {
        clog << "oriented sine" << endl;
        raster<double> m (600, 800, 1.0);
        subscript_unary_function<double,oriented_sin> s (m.rows (), m.cols ());
        s.orientation (radians (30));
        transform (m.begin (), m.end (), m.begin (), s);
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 127.5));
        transform (m.begin (), m.end (), m.begin (), bind2nd (plus<double> (), 127.5));
        //Display (m);
    }
}

void test_radial_cos (bool verbose)
{
    raster<double> m (5, 5, 1.0);
    subscript_unary_function<double,radial_cos> c (m.rows (), m.cols ());
    transform (m.begin (), m.end (), m.begin (), c);
    if (verbose)
        print2d (clog, m);
    VERIFY (about_equal (m (2, 0), -1.0));
    VERIFY (about_equal (m (2, 2), 1.0));
    VERIFY (about_equal (m (2, 4), -1.0));
    if (verbose)
    {
        clog << "radial cosine" << endl;
        raster<double> m (600, 800, 1.0);
        subscript_unary_function<double,radial_cos> c (m.rows (), m.cols ());
        transform (m.begin (), m.end (), m.begin (), c);
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 127.5));
        transform (m.begin (), m.end (), m.begin (), bind2nd (plus<double> (), 127.5));
        //Display (m);
    }
}

void test_raised_cos (bool verbose)
{
    raster<double> m (5, 5);
    subscript_generator<double,raised_cos> c (m.rows (), m.cols ());
    generate (m.begin (), m.end (), c);
    if (verbose)
        print2d (clog, m);
    //VERIFY (about_equal (m (0, 0), 0.0));
    //VERIFY (about_equal (m (0, 4), 0.0));
    //VERIFY (about_equal (m (2, 2), 1.0));
    //VERIFY (about_equal (m (4, 0), 0.0));
    //VERIFY (about_equal (m (4, 4), 0.0));
    //VERIFY (m (1, 1) != 0.0);
    //VERIFY (m (1, 1) != 1.0);
    if (verbose)
    {
        clog << "raised cosine" << endl;
        raster<double> m (600, 800);
        subscript_generator<double,raised_cos> c (m.rows (), m.cols ());
        generate (m.begin (), m.end (), c);
        //transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 127.5));
        //transform (m.begin (), m.end (), m.begin (), bind2nd (plus<double> (), 127.5));
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 255));
        //Display (m);
        clog << "raised cosine w/half_height" << endl;
        c.half_height (c.radius () * 1 / 11);
        generate (m.begin (), m.end (), c);
        //transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 127.5));
        //transform (m.begin (), m.end (), m.begin (), bind2nd (plus<double> (), 127.5));
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 255));
        //Display (m);
    }
}

void test_gaussian_window (bool verbose)
{
    raster<double> m (5, 5, 1.0);
    subscript_unary_function<double,gaussian_window> g (m.rows (), m.cols ());
    g.stddev (m.rows () / 40.0);
    transform (m.begin (), m.end (), m.begin (), g);
    if (verbose)
        print2d (clog, m);
    VERIFY (about_equal (m (2, 0), 0.0));
    VERIFY (about_equal (m (2, 2), 1.0));
    VERIFY (about_equal (m (2, 4), 0.0));

    if (verbose)
    {
        clog << "gaussian window" << endl;
        raster<double> m (600, 800);
        subscript_generator<double,gaussian_window> g (m.rows (), m.cols ());
        generate (m.begin (), m.end (), g);
        transform (m.begin (), m.end (), m.begin (), bind2nd (multiplies<double> (), 255));
        //Display (m);
    }

    {
    if (verbose)
        clog << "gaussian window" << endl;

    raster<double> m (6, 6);
    subscript_generator<double,gaussian_window> g (m.rows (), m.cols ());
    g.stddev (0.85);
    generate (m.begin (), m.end (), g);
    if (verbose)
        print2d (clog, m);
    VERIFY (about_equal (m (2, 2), 0.707));
    }
}

void test_mean (bool verbose)
{
    const size_t W = 4;
    const size_t H = 5;
    const size_t SW = 3;
    const size_t SH = 4;
    raster<double> m (H, W, 128.0);
    // Create a weighting function
    raster<double> w (SH, SW, 1.0);
    w (SH - 1, 0) = 2;
    // Image subregion
    subregion s = { 0, 0, SH, SW };
    // Zero out the top left corner
    for_each (m.begin (s), m.end (s), _1 = 0.0);
    double mean = local_mean (m.begin (s), m.end (s), w.begin ());
    if (verbose)
    {
        clog << "local mean" << endl;
        clog << "image" << endl;
        print2d (clog, m);
        clog << "weights" << endl;
        print2d (clog, w);
        clog << "mean = " << mean << endl;
    }
    VERIFY (about_equal (mean, 0.0));
    s.c = 1;
    mean = local_mean (m.begin (s), m.end (s), w.begin ());
    if (verbose)
        clog << "mean = " << mean << endl;
    VERIFY (about_equal (mean, 4 * 128.0));
    s.r = 1;
    mean = local_mean (m.begin (s), m.end (s), w.begin ());
    if (verbose)
        clog << "mean = " << mean << endl;
    VERIFY (about_equal (mean, 7 * 128.0));
    // violates invariant:
    //s.r = 2;
    //mean = local_mean (m, s, w);
    mean = local_mean (m.begin (s), m.end (s));
    if (verbose)
        clog << "mean = " << mean << endl;
    VERIFY (about_equal (mean, 6 * 128.0 / (SW * SH)));
}

void test_variance (bool verbose)
{
    const size_t W = 1000;
    const size_t H = 1000;
    raster<double> m (H, W);
    // Create a uniform rng
    uniform_01<boost::mt19937> uni_rng (boost::mt19937 (17));
    // Create a random patch
    generate (m.begin (), m.end (), uni_rng);
    if (verbose)
    {
        clog << "variance" << endl;
        double mean = accumulate (m.begin (), m.end (), 0.0) / (W * H);
        clog << "mean = " << mean << endl;
        double var = 0.0;
        for (size_t i = 0; i < m.size (); ++i)
            var += (m[i] - mean) * (m[i] - mean);
        var /= m.size ();
        clog << "variance = " << var << endl;
        subregion s = { 100, 100, 100, 100 };
        // Weight normalized to 1.0
        raster<double> w (s.rows, s.cols, 1.0 / (s.rows * s.cols));
        double lmean = local_mean (m.begin (s), m.end (s));
        clog << "local mean = " << lmean << endl;
        double lvar = local_variance (m.begin (s), m.end (s), w.begin ());
        clog << "local variance = " << lvar << endl;
        VERIFY (about_equal (mean, lmean, 0.01));
        VERIFY (about_equal (var, lvar, 0.01));
    }
}

void test_rms_contrast (bool verbose)
{
    const size_t W = 1000;
    const size_t H = 1000;
    raster<double> m (H, W);
    // Create a uniform rng
    uniform_01<boost::mt19937> uni_rng (boost::mt19937 (17));
    // Create a random patch
    generate (m.begin (), m.end (), uni_rng);
    double mean = accumulate (m.begin (), m.end (), 0.0) / (W * H);
    if (verbose)
        clog << "mean: " << mean << endl;
    // Create a uniform weighting function
    raster<double> w (H, W, 1.0 / (W * H));
    double w_sum = accumulate (w.begin (), w.end (), 0.0);
    double rms = local_rms_contrast_p (m.begin (), m.end (), w.begin (), w_sum, 0.0);
    // rms of uniform [0,1] = 0.5773
    if (verbose)
        clog << "rms contrast: " << rms << endl;
    VERIFY (about_equal (rms, 0.58, 0.01));
    double rms2 = local_rms_contrast (m.begin (), m.end (), w.begin ());
    if (verbose)
        clog << "rms contrast2: " << rms2 << endl;
    VERIFY (about_equal (rms, rms2));

    // Create an aperture
    subscript_generator<double,aperture> a (w.rows (), w.cols ());
    generate (w.begin (), w.end (), a);
    // Multiply by a raised cosine
    subscript_unary_function<double,radial_cos> c (w.rows (), w.cols ());
    // Raised cosine, -1 to 1
    transform (w.begin (), w.end (), w.begin (), c);
    // Raised cosine, 0 to 1
    for_each (w.begin (), w.end (), _1 = (_1 + 1.0) / 2);

    // Zero contrast
    raster<unsigned char> mc (H, W, 127);
    w_sum = accumulate (w.begin (), w.end (), 0.0);
    rms = local_rms_contrast_p (mc.begin (), mc.end (), w.begin (), w_sum, 0.0);
    if (verbose)
        clog << "rms contrast: " << rms << endl;
    VERIFY (about_equal (rms, 0.0));
    // Normalize w
    for_each (w.begin (), w.end (), _1 = (_1 / w_sum));
    w_sum = accumulate (w.begin (), w.end (), 0.0);
    if (verbose)
        clog << "w_sum: " << w_sum << endl;
    rms2 = local_rms_contrast (mc.begin (), mc.end (), w.begin ());
    if (verbose)
        clog << "rms contrast2: " << rms2 << endl;
    VERIFY (about_equal (rms, rms2));

    // Random pixels
    boost::mt19937 rng;
    uniform_int<> uni_int (0, 255);
    variate_generator<boost::mt19937&, uniform_int<> > sampler (rng, uni_int);
    generate (mc.begin (), mc.end (), sampler);
    mean = accumulate (mc.begin (), mc.end (), 0.0) / (W * H);
    w_sum = accumulate (w.begin (), w.end (), 0.0);
    rms = local_rms_contrast_p (mc.begin (), mc.end (), w.begin (), w_sum, 0.0);
    // rms of uniform [0,1] = 0.5773
    if (verbose)
        clog << "rms contrast: " << rms << endl;
    VERIFY (about_equal (rms, 0.58, 0.01));
}

void test_rms_contrast_image (bool verbose)
{
    // Test rms across an entire image
    raster<double> w (5, 5);
    subscript_generator<double,raised_cos> weighting_function (w.rows (), w.cols ());
    generate (w.begin (), w.end (), weighting_function);

    const unsigned W = 16;
    const unsigned H = 16;

    // Create a random image
    raster<uint16_t> img (H, W);
    generate (img.begin (), img.end (), rand);

    raster<double> c;

    c = rms_contrast<subregion> (img, w);

    if (verbose)
    {
        clog << "weights:" << endl;
        print2d (clog, w);
        clog << "image:" << endl;
        print2d (clog, img);
        clog << "contrast:" << endl;
        print2d (clog, c);
    }

    raster<double> m (c.rows (), c.cols ());

    rms_contrast_m<subregion> (img, w, c, m);

    if (verbose)
    {
        clog << "weights:" << endl;
        print2d (clog, w);
        clog << "image:" << endl;
        print2d (clog, img);
        clog << "contrast:" << endl;
        print2d (clog, c);
    }
}

void test_convolve (bool verbose)
{
    // Test convolution across an entire image
    raster<double> w (5, 5);
    subscript_generator<double,raised_cos> weighting_function (w.rows (), w.cols ());
    generate (w.begin (), w.end (), weighting_function);
    double w_sum = std::accumulate (w.begin (), w.end (), 0.0);
    for_each (w.begin (), w.end (),  _1 = _1 / w_sum);

    const unsigned W = 16;
    const unsigned H = 16;

    // Create a random image
    raster<uint16_t> img (H, W);
    generate (img.begin (), img.end (), rand);

    raster<double> m;

    m = convolve<subregion> (img, w);

    if (verbose)
    {
        clog << "weights:" << endl;
        print2d (clog, w);
        clog << "image:" << endl;
        print2d (clog, img);
        clog << "mean:" << endl;
        print2d (clog, m);
    }
}

void test_split3_join3 (bool verbose)
{
    raster<unsigned char> d (5, 5 * 3);
    for (raster<unsigned char>::iterator i = d.begin (); i != d.end (); )
    {
        *i++ = 'a';
        *i++ = 'b';
        *i++ = 'c';
    }
    if (verbose)
    {
        clog << "src:" << endl;
        print2d (clog, d);
    }
    raster<float> a (5, 5);
    raster<float> b (5, 5);
    raster<float> c (5, 5);
    split3 (d.begin (), d.end (), a.begin (), b.begin (), c.begin ());
    if (verbose)
    {
        clog << "a:" << endl;
        print2d (clog, a);
        clog << "b:" << endl;
        print2d (clog, b);
        clog << "c:" << endl;
        print2d (clog, c);
    }
    raster<unsigned char> e (5, 5 * 3);
    join3 (a.begin (), a.end (), b.begin (), c.begin (), e.begin ());
    if (verbose)
    {
        clog << "e:" << endl;
        print2d (clog, e);
    }
    VERIFY (d == e);
}

template<typename T>
struct add_op { T operator() (T a, T b, T c) { return a + b + c; } };

void test_transform3 (bool verbose)
{
    raster<int> a (5, 5, 1);
    raster<int> b (5, 5, 3);
    raster<int> c (5, 5, 7);
    raster<int> d (5, 5, 0);
    if (verbose)
    {
        clog << "a:" << endl;
        print2d (clog, a);
        clog << "b:" << endl;
        print2d (clog, b);
        clog << "c:" << endl;
        print2d (clog, c);
    }
    transform3 (a.begin (), a.end (), b.begin (), c.begin (), d.begin (), add_op<int> ());
    if (verbose)
    {
        clog << "d:" << endl;
        print2d (clog, d);
    }
    VERIFY (d == raster<int> (5, 5, 11));
}

void test_reflect (bool verbose)
{
    if (verbose)
        clog << "reflect(0,10)=" << reflect (0, 10) << endl;
    VERIFY (reflect (0, 10) == 0);
    if (verbose)
        clog << "reflect(-1,10)=" << reflect (-1, 10) << endl;
    VERIFY (reflect (-1, 10) == 1);
    if (verbose)
        clog << "reflect(9,10)=" << reflect (9, 10) << endl;
    VERIFY (reflect (9, 10) == 9);
    if (verbose)
        clog << "reflect(10,10)=" << reflect (10, 10) << endl;
    VERIFY (reflect (10, 10) == 10);
    if (verbose)
        clog << "reflect(11,10)=" << reflect (11, 10) << endl;
    VERIFY (reflect (11, 10) == 9);
}

void test_mirrored_dot_product (bool verbose)
{
    raster<int> p (100, 100, 1);
    // create gaussian kernel
    raster<double> g (11, 11, 1.0);
    subscript_unary_function<double,gaussian_window> f (g.rows (), g.cols ());
    f.stddev (1.0);
    std::transform (g.begin (), g.end (), g.begin (), f);
    if (verbose)
        print2d (clog, g);
    double x = mirrored_dot_product (g, p, 99, 99);
    if (verbose)
        clog << "x=" << x << endl;
    VERIFY (x > 6);
    raster<int> q (100, 200);
    for (size_t i = 0; i < q.rows (); ++i)
    for (size_t j = 0; j < q.cols (); ++j)
        q (i, j) = i;
    raster<int> h (17, 11, 1);
    // mirror at the corners
    int d1 = mirrored_dot_product (h, q, 0, 0);
    if (verbose)
        clog << "d1=" << d1 << endl;
    int d2 = mirrored_dot_product (h, q, 1-h.rows (), 1-h.cols ());
    if (verbose)
        clog << "d2=" << d2 << endl;
    VERIFY (d1 == d2);
    d1 = mirrored_dot_product (h, q, 0, q.cols()-h.cols ());
    if (verbose)
        clog << "d1=" << d1 << endl;
    d2 = mirrored_dot_product (h, q, 1-h.rows (), q.cols()-1);
    if (verbose)
        clog << "d2=" << d2 << endl;
    VERIFY (d1 == d2);
    d1 = mirrored_dot_product (h, q, q.rows()-h.rows(), 0);
    if (verbose)
        clog << "d1=" << d1 << endl;
    d2 = mirrored_dot_product (h, q, q.rows()-1, 1-h.cols());
    if (verbose)
        clog << "d2=" << d2 << endl;
    VERIFY (d1 == d2);
    d1 = mirrored_dot_product (h, q, q.rows()-h.rows(), q.cols()-h.cols());
    if (verbose)
        clog << "d1=" << d1 << endl;
    d2 = mirrored_dot_product (h, q, q.rows()-1, q.cols()-1);
    if (verbose)
        clog << "d2=" << d2 << endl;
    VERIFY (d1 == d2);
}

void test_crop (bool verbose)
{
    raster<int> p (9, 9);
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            p (i, j) = (i + 1) * 10 + (j + 1);
    if (verbose)
        print2d (clog, p);
    raster<int> q = crop (p, 2, 3, 8, 7);
    if (verbose)
        print2d (clog, q);
    VERIFY (q(0,0) == 34);
    VERIFY (q(6,4) == 98);
    q = crop (p, 2);
    if (verbose)
        print2d (clog, q);
    VERIFY (q(0,0) == 33);
    VERIFY (q(4,4) == 77);
}

void test_border (bool verbose)
{
    raster<int> p (3, 3);
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            p (i, j) = (i + 1) * 10 + (j + 1);
    if (verbose)
        print2d (clog, p);
    raster<int> q = border<subregion> (p, 2);
    if (verbose)
        print2d (clog, q);
    VERIFY (q(0,0) == 0);
    VERIFY (q(2,2) == 11);
    VERIFY (q(4,4) == 33);
    q = mborder<subregion> (p, 2);
    if (verbose)
        print2d (clog, q);
    VERIFY (q(0,0) == 33);
    VERIFY (q(0,3) == 32);
    VERIFY (q(3,0) == 23);
    VERIFY (q(2,2) == 11);
    VERIFY (q(4,4) == 33);
    VERIFY (q(6,6) == 11);
    VERIFY (q(3,6) == 21);
    VERIFY (q(6,3) == 12);
}

unsigned rand (unsigned n) { return rand () % n; }

void test_flip (bool verbose)
{
    const size_t N = 10;
    for (size_t n = 0; n < rand (N); ++n)
    {
        const size_t R = rand (N) + N;
        const size_t C = rand (N) + N;
        raster<int> p (R, C);
        raster<int> q (R, C);
        for (size_t i = 0; i < p.rows (); ++i)
            for (size_t j = 0; j < p.cols (); ++j)
                p (i, j) = i * p.cols () + j;
        for (size_t i = 0; i < q.rows (); ++i)
            for (size_t j = 0; j < q.cols (); ++j)
                q (i, j) = i * q.cols () + q.cols () - j - 1;
        if (verbose)
            print2d (clog, p);
        if (verbose)
            print2d (clog, q);
        q = fliplr (q);
        if (verbose)
            print2d (clog, q);
        VERIFY (p == q);
        for (size_t i = 0; i < q.rows (); ++i)
            for (size_t j = 0; j < q.cols (); ++j)
                q (i, j) = (q.rows () - i - 1) * q.cols () + j;
        if (verbose)
            print2d (clog, p);
        if (verbose)
            print2d (clog, q);
        q = flipud (q);
        if (verbose)
            print2d (clog, q);
        VERIFY (p == q);
        for (size_t i = 0; i < q.rows (); ++i)
            for (size_t j = 0; j < q.cols (); ++j)
                q (i, j) = (q.rows () - i - 1) * q.cols () + (q.cols () - j - 1);
        if (verbose)
            print2d (clog, p);
        if (verbose)
            print2d (clog, q);
        q = fliplrud (q);
        if (verbose)
            print2d (clog, q);
        VERIFY (p == q);
    }
}

void test_transpose (bool verbose)
{
    const size_t N = 10;
    for (size_t n = 0; n < rand (N); ++n)
    {
        const size_t R = rand (N) + N;
        const size_t C = rand (N) + N;
        raster<int> p (R, C);
        raster<int> q (C, R);
        for (size_t i = 0; i < p.rows (); ++i)
            for (size_t j = 0; j < p.cols (); ++j)
                p (i, j) = q (j, i) = i * p.cols () + j;
        if (verbose)
            print2d (clog, p);
        if (verbose)
            print2d (clog, q);
        q = transpose (q);
        if (verbose)
            print2d (clog, q);
        VERIFY (p == q);
    }
}

int main (int argc, char **)
{
    try
    {
        const bool verbose = (argc != 1);
        test_convenience ();
        test_aperture (verbose);
        test_oriented_sin (verbose);
        test_radial_cos (verbose);
        test_raised_cos (verbose);
        test_gaussian_window (verbose);
        test_mean (verbose);
        test_variance (verbose);
        test_rms_contrast (verbose);
        test_rms_contrast_image (verbose);
        test_convolve (verbose);
        test_split3_join3 (verbose);
        test_transform3 (verbose);
        test_reflect (verbose);
        test_mirrored_dot_product (verbose);
        test_crop (verbose);
        test_border (verbose);
        test_flip (verbose);
        test_transpose (verbose);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
