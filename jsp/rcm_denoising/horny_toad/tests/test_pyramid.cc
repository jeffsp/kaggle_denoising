/// @file test_pyramid.cc
/// @brief test pyramid
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/verify.h"
#include "horny_toad/pyramid.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>

using namespace horny_toad;
using namespace jack_rabbit;
using namespace std;

// Output a raster to a stream
template<typename T>
std::ostream& operator<< (std::ostream &s, const raster<T> &m)
{
    for (size_t i = 0; i < m.rows (); ++i) {
        for (size_t j = 0; j < m.cols (); ++j)
            s << setw (4) << m (i, j);
        s << endl; }
    return s;
}

template <class T>
struct diff : public binary_function<T,T,T>
{
    T operator() (const T &x, const T &y) const
    {
        T a = x;
        T b = y;
        return static_cast<T> (abs (a - b));
    }
};

// Helper functions
void test_helpers ()
{
    VERIFY (next_lowest_pow2 (1) == 0);
    VERIFY (next_highest_pow2 (1) == 0);
    VERIFY (next_lowest_pow2 (2) == 1);
    VERIFY (next_highest_pow2 (2) == 1);
    VERIFY (next_lowest_pow2 (3) == 1);
    VERIFY (next_highest_pow2 (3) == 2);
    VERIFY (next_lowest_pow2 (1023) == 9);
    VERIFY (next_highest_pow2 (1023) == 10);
    VERIFY (next_lowest_pow2 (1024) == 10);
    VERIFY (next_highest_pow2 (1024) == 10);
    VERIFY (next_lowest_pow2 (1025) == 10);
    VERIFY (next_highest_pow2 (1025) == 11);
}

// Pyramid ctor, levels, dimensions, swap
void test_ctor ()
{
    { pyramid<int> p;
    VERIFY (p.levels () == 0); }
    { pyramid<int> p (0, 0);
    VERIFY (p.levels () == 0); }
    { pyramid<int> p (1, 1);
    VERIFY (p.levels () == 1); }
    { pyramid<int> p (2, 2);
    VERIFY (p.levels () == 2); }
    { pyramid<int> p (3, 3);
    VERIFY (p.levels () == 3); }
    {
        pyramid<int> p (4, 4);
        VERIFY (p.levels () == 3);
        p.resize (3, 3);
        VERIFY (p.levels () == 3);
        p.resize (2, 2);
        VERIFY (p.levels () == 2);
    }
    const size_t M = 10;
    const size_t N = 19;
    pyramid<int> p (M, N);
    VERIFY (p.levels () == 5);
    VERIFY (p[0].rows () == M);
    VERIFY (p[0].cols () == N);
    for (size_t i = 1; i < p.levels (); ++i)
    {
        VERIFY (p[i].rows () == (p[i-1].rows () + 1) / 2);
        VERIFY (p[i].cols () == (p[i-1].cols () + 1) / 2);
    }
    pyramid<double> q (p);
    VERIFY (q.levels () == 5);
    pyramid<int>::const_iterator i = p.begin ();
    pyramid<double>::const_iterator j = q.begin ();
    for (; i != p.end (); ++i, ++j)
    {
        VERIFY (i->rows () == j->rows ());
        VERIFY (i->cols () == j->cols ());
    }
    pyramid<double> r (0, 0);
    r.swap (q);
    VERIFY (r.levels () == 5);
    VERIFY (q.empty ());
    raster<double> s (100, 100, 1);
    pyramid<double> t (s, 4);
    VERIFY (t.levels () == 4);
    VERIFY (t[0][0] == 1);
    pyramid<double> u (100, 100, 5, 1);
    VERIFY (u.levels () == 5);
    VERIFY (u.bottom ().front () == 1);
    VERIFY (u.top ().back () == 1);
    u.resize (200, 200, 6, 2);
    VERIFY (u.bottom ().front () == 1);
    VERIFY (u.top ().back () == 2);
}

// Instantiate with different types
template<typename T>
void test_types ()
{
    const size_t R = 10;
    const size_t C = 10;
    pyramid<T> p (R, C);
    p.reduce2x2 ();
    p.reduce3x3 ();
    p.expand2x2 (1);
    p.expand3x3 (1);
    p.expand2x2_and_blur3x3 (1);
}

// Reduce
template<typename T>
void test_reduce (size_t r, size_t c)
{
    pyramid<T> p (r, c, 0);
    p.bottom ().assign (255);
    VERIFY (p.bottom ().back () == 255);
    p.top ().assign (0);
    VERIFY (p.top ().back () == 0);
    for (size_t i = 0; i + 1 < p.levels (); ++i)
        p.reduce2x2 (i);
    VERIFY (p.top ().front () != 0);

    p.bottom ().assign (255);
    VERIFY (p.bottom ().back () == 255);
    p.top ().assign (0);
    VERIFY (p.top ().back () == 0);
    for (size_t i = 0; i + 1 < p.levels (); ++i)
        p.reduce3x3 (i);
    VERIFY (p.top ().front () != 0);
}

// Expand
template<typename T>
void test_expand (const size_t R,
    const size_t C)
{
    pyramid<T> p1 (R, C, 0);
    pyramid<T> p2 (R, C, 0);
    pyramid<T> p3 (R, C, 0);
    pyramid<T> p4 (R, C, 0);
    pyramid<T> p5 (R, C, 0);

    p1.top ().assign (255);
    for (size_t i = p1.levels () - 1; i > 0; --i)
    {
        size_t src_level = i;
        size_t dest_level = i - 1;
        VERIFY (p1[src_level].front () != 0);
        VERIFY (p1[dest_level].front () == 0);
        p1.expand2x2 (src_level);
        VERIFY (p1[dest_level].front () != 0);
    }

    p2.top ().assign (255);
    for (size_t i = p2.levels () - 1; i > 0; --i)
    {
        size_t src_level = i;
        size_t dest_level = i - 1;
        VERIFY (p2[src_level].front () != 0);
        VERIFY (p2[dest_level].front () == 0);
        p2.expand3x3 (src_level);
        VERIFY (p2[dest_level].front () != 0);
    }

    p3.top ().assign (255);
    for (size_t i = p3.levels () - 1; i > 0; --i)
    {
        size_t src_level = i;
        size_t dest_level = i - 1;
        VERIFY (p3[src_level].front () != 0);
        VERIFY (p3[dest_level].front () == 0);
        p3.expand2x2_and_blur3x3 (src_level);
        VERIFY (p3[dest_level].front () != 0);
    }

    p4.top ().assign (255);
    for (size_t i = p4.levels () - 1; i > 0; --i)
    {
        size_t src_level = i;
        size_t dest_level = i - 1;
        VERIFY (p4[src_level].front () != 0);
        VERIFY (p4[dest_level].front () == 0);
        p4.expand3x3_with_jitter (src_level, 0);
        VERIFY (p4[dest_level].front () != 0);
    }

    p5.top ().assign (255);
    for (size_t i = p5.levels () - 1; i > 0; --i)
    {
        size_t src_level = i;
        size_t dest_level = i - 1;
        VERIFY (p5[src_level].front () != 0);
        VERIFY (p5[dest_level].front () == 0);
        p5.expand3x3_with_jitter (src_level, 100);
        VERIFY (p5[dest_level].front () != 0);
    }
}

// Reduced/expanded pyramid should get completely filled
template<typename T>
void test_everything_filled (const size_t R,
    const size_t C)
{
    pyramid<T> r1 (R, C, 255);
    pyramid<T> e1 (R, C, 0);

    for (size_t i = 0; i + 1 < r1.levels (); ++i)
        r1.reduce2x2 (i);
    VERIFY (r1.top ().front () != 0);
    VERIFY (r1.top ().back () != 0);

    e1.top () = r1.top ();

    VERIFY (e1.bottom ().front () == 0);
    VERIFY (e1.bottom ().back () == 0);
    for (size_t i = e1.levels () - 1; i > 0; --i)
        e1.expand2x2 (i);

    VERIFY (e1.bottom ().front () != 0);
    VERIFY (e1.bottom ().back () != 0);

    pyramid<T> r2 (R, C, 255);
    pyramid<T> e2 (R, C, 0);

    for (size_t i = 0; i + 1 < r2.levels (); ++i)
        r2.reduce3x3 (i);
    VERIFY (r2.top ().front () != 0);
    VERIFY (r2.top ().back () != 0);

    e2.top () = r2.top ();

    VERIFY (e2.bottom ().front () == 0);
    VERIFY (e2.bottom ().back () == 0);
    for (size_t i = e2.levels () - 1; i > 0; --i)
        e2.expand3x3 (i);

    VERIFY (e2.bottom ().front () != 0);
    VERIFY (e2.bottom ().back () != 0);

    pyramid<T> r3 (R, C, 255);
    pyramid<T> e3 (R, C, 0);

    for (size_t i = 0; i + 1 < r3.levels (); ++i)
        r3.reduce3x3 (i);
    VERIFY (r3.top ().front () != 0);
    VERIFY (r3.top ().back () != 0);

    e3.top () = r3.top ();

    VERIFY (e3.bottom ().front () == 0);
    VERIFY (e3.bottom ().back () == 0);
    for (size_t i = e3.levels () - 1; i > 0; --i)
        e3.expand3x3 (i);

    VERIFY (e3.bottom ().front () != 0);
    VERIFY (e3.bottom ().back () != 0);
}

// Operations
template<typename M>
void test_ops (const size_t R,
    const size_t C,
    void (*Reduce) (const M &src, M &dest),
    void (*Expand) (const M &src, M &dest)
    )
{
    M base (R, C);
    generate (base.begin (), base.end (), rand);
    transform (base.begin (),
        base.end (),
        base.begin (),
        bind2nd (modulus<typename M::value_type> (), 100));
    pyramid<typename M::value_type> r (base);
    pyramid<typename M::value_type> e (R, C, 0);
    pyramid<typename M::value_type> d (R, C, 0);

    for (size_t i = 0; i + 1 < r.levels (); ++i)
    {
        //clog << r[i] << endl;
        //clog << r[i + 1] << endl;
        Reduce (r[i], r[i + 1]);
        //clog << r[i + 1] << endl;
        //clog << e[i] << endl;
        Expand (r[i + 1], e[i]);

        // STL rocks
        transform (r[i].begin (),
            r[i].end (),
            e[i].begin (),
            d[i].begin (),
            diff<int> ());

        //clog << "original: " << endl;
        //clog << r[i];
        //clog << "reduced: " << endl;
        //clog << r[i + 1];
        //clog << "expanded: " << endl;
        //clog << e[i];
        //clog << "difference: " << endl;
        //clog << d[i];
    }
}

void test_rand ()
{
    for (size_t i = 0; i < 1000; ++i)
    {
        const size_t R = rand () % 32 + 2;
        const size_t C = rand () % 32 + 2;
        //clog << R << " X " << C << endl;
        raster<int> base (R, C);
        generate (base.begin (), base.end (), rand);
        pyramid<int> p (base);
        VERIFY (p.levels () != 0);
        for (size_t j = 0; j + 1 < p.levels (); ++j)
        {
            reduce2x2 (p[j], p[j + 1]);
            reduce3x3 (p[j], p[j + 1]);
            expand2x2 (p[j + 1], p[j]);
            expand3x3 (p[j + 1], p[j]);
        }
    }
}

template<typename T>
void test_tests ()
{
    test_types<T> ();
    test_reduce<T> (2, 2);
    test_reduce<T> (3, 3);
    test_reduce<T> (8, 8);
    test_reduce<T> (12, 12);
    test_expand<T> (7, 23);
    test_expand<T> (16, 16);
    test_expand<T> (19, 16);
    test_everything_filled<T> (16, 16);
    test_everything_filled<T> (9, 9);
    test_everything_filled<T> (12, 12);
    test_everything_filled<T> (15, 15);
}

template<typename T>
void test_ops ()
{
    typedef raster<T> M;
    test_ops (8, 8, reduce2x2<M,M>, expand2x2<M,M>);
    test_ops (8, 8, reduce3x3<M,M>, expand3x3<M,M>);
    test_ops (8, 8, reduce2x2<M,M>, expand2x2_and_blur3x3<M,M>);
    test_ops (9, 9, reduce2x2<M,M>, expand2x2<M,M>);
    test_ops (9, 9, reduce3x3<M,M>, expand3x3<M,M>);
    test_ops (9, 9, reduce2x2<M,M>, expand2x2_and_blur3x3<M,M>);
    test_ops (29, 19, reduce2x2<M,M>, expand2x2<M,M>);
    test_ops (29, 19, reduce3x3<M,M>, expand3x3<M,M>);
    test_ops (29, 19, reduce2x2<M,M>, expand2x2_and_blur3x3<M,M>);
}

int main ()
{
    try
    {
        test_helpers ();
        test_ctor ();
        test_rand ();
        test_tests<unsigned char> ();
        test_tests<short> ();
        test_tests<unsigned short> ();
        test_tests<int> ();
        test_tests<unsigned int> ();
        test_tests<double> ();
        test_ops<unsigned char> ();
        test_ops<short> ();
        test_ops<unsigned short> ();
        test_ops<int> ();
        test_ops<unsigned int> ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
