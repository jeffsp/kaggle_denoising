/// @file test_foveator.cc
/// @brief test foveator
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/foveator.h"
#include "horny_toad/verify.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace horny_toad;
using namespace std;

// Output a raster to a stream
template<typename T>
std::ostream& operator<< (std::ostream &s, const jack_rabbit::raster<T> &m)
{
    for (size_t i = 0; i < m.rows (); ++i) {
        for (size_t j = 0; j < m.cols (); ++j)
            s << setw (9) << m (i, j);
        s << endl; }
    s << endl;
    return s;
}

template<typename T>
const jack_rabbit::raster<typename T::precision_type> get_blending_map_image (const T &bm, size_t rows, size_t cols)
{
    jack_rabbit::raster<typename T::precision_type> rm (rows, cols);
    for (size_t r = 0; r < rm.rows (); ++r)
        for (size_t c = 0; c < rm.cols (); ++c)
            rm (r, c) = bm.get_blending_level (rect<typename T::precision_type> (c, r, 1, 1));
    return rm;
}

template<typename T,template<typename> class K>
void test0 ()
{
    size_t R = 16;
    size_t C = 16;
    T N = 10;
    jack_rabbit::raster<T> m (R, C, 0);
    m.insert_rows (m.loc (R / 2, 0), 1, N);
    e2_blending_map<> b;
    b.set_fixation (0, R / 2);
    b.set_e2 (C);
    foveator<T,K> f (m.rows (), m.cols (), &b);
    f.set_source_pixels (m.begin (), m.end ());
    //clog << get_blending_map_image (b, m.rows (), m.cols ());
    f.foveate ();
    jack_rabbit::raster<T> d = f.get_foveated_pyramid ()[0];
    //clog << d << endl;
    VERIFY (d (R / 2, 0) == N);
    VERIFY (d (R / 2, C - 1) == N);
    b.set_e2 (C / 4);
    //clog << get_blending_map_image (b, m.rows (), m.cols ());
    f.foveate ();
    d = f.get_foveated_pyramid ()[0];
    //clog << d << endl;
    VERIFY (d (R / 2, 0) == N);
    //clog << d (R / 2, C - 1) << endl;
    VERIFY (static_cast<T> (d (R / 2, C - 1)) != 10);
}

template<typename T,template<typename> class K>
void test1 ()
{
    size_t R = 7;
    size_t C = 11;
    jack_rabbit::raster<T> m (R, C, 128);
    e2_blending_map<> b;
    foveator<T,K> f;
    f.resize (R, C);
    f.set_blending_map (&b);
    f.set_source_pixels (m.begin (), m.end ());
    const jack_rabbit::raster<T> &d = f.get_foveated_pyramid ()[0];
    VERIFY (d.rows () == R);
    VERIFY (d.cols () == C);
    VERIFY (d[0] == 0);
    f.foveate ();
    VERIFY (d[0] == 128);
}

template<typename T>
void test2 ()
{
    size_t R = 100;
    size_t C = 100;
    jack_rabbit::raster<T> m (R, C, 128);
    e2_blending_map<> b;
    foveator<T,kernel3x3> f;
    f.resize (R, C);
    f.set_blending_map (&b);
    f.set_source_pixels (m.begin (), m.end ());
    b.set_e2 (1);
    size_t t;
    t = f.foveate ();
    //clog << t << endl;
    VERIFY (t < R * C);
    b.set_e2 (1000);
    t = f.foveate ();
    //clog << t << endl;
    VERIFY (t > R * C);
}

int main ()
{
    try
    {
        test0<int,kernel3x3> ();
        test0<int,kernel2x2> ();
        test0<int,kernel2x2_with_blur> ();
        test0<float,kernel3x3> ();
        test0<float,kernel2x2> ();
        test0<float,kernel2x2_with_blur> ();
        test0<unsigned char,kernel3x3> ();
        test0<unsigned char,kernel2x2> ();
        test0<unsigned char,kernel2x2_with_blur> ();
        test1<int,kernel3x3> ();
        test1<int,kernel2x2> ();
        test1<int,kernel2x2_with_blur> ();
        test1<float,kernel3x3> ();
        test1<float,kernel2x2> ();
        test1<float,kernel2x2_with_blur> ();
        test1<unsigned char,kernel3x3> ();
        test1<unsigned char,kernel2x2> ();
        test1<unsigned char,kernel2x2_with_blur> ();
        test2<int> ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
