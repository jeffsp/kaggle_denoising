/// @file test_blending_map.cc
/// @brief test resmap
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/blending_map.h"
#include "jack_rabbit/raster.h"
#include "horny_toad/verify.h"
#include <iostream>
#include <iomanip>
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
            s << std::setw (4) << m (i, j);
        s << std::endl; }
    s << std::endl;
    return s;
}

template<typename T1,typename T2>
bool about_equal (T1 a, T2 b, T2 prec = 1000)
{
    return round (a * prec) == round (b * prec);
}

void test0 ()
{
    e2_blending_map<float> m;
    m.set_e2 (1);
    VERIFY (m.get_e2 () == 1);
    m.set_e2 (2);
    VERIFY (m.get_e2 () == 2);
}

void test1 ()
{
    e2_blending_map<double> m;
    m.set_e2 (1.0);
    VERIFY (about_equal (m.get_blending_level (superpixel (0, 0, 0)), -1.0));
    VERIFY (about_equal (m.get_blending_level (superpixel (0, 1, 0)), 0.0));
    VERIFY (about_equal (m.get_blending_level (superpixel (0, 1, 0)), 0.0));
    VERIFY (about_equal (m.get_blending_level (superpixel (1, 0, 0)), 0.0));
    VERIFY (about_equal (m.get_blending_level (superpixel (0, 3, 0)), 1.0));
    VERIFY (about_equal (m.get_blending_level (superpixel (3, 0, 0)), 1.0));
    VERIFY (m.get_blending_level (superpixel (0, 10000, 1)) > 10);
    VERIFY (m.get_blending_level (superpixel (10000, 0, 1)) > 10);
}

void test2 ()
{
    e2_blending_map<double> m;
    m.set_e2 (1.0);
    m.set_fixation (0, 0);
    double last = m.get_blending_level (superpixel (0, 0, 1));
    for (size_t x = 1; x < 100; ++x)
    {
        // should be monotonically increasing
        double b = m.get_blending_level (superpixel (x, 0, 1));
        VERIFY (b > last);
        //clog << r << endl;
        last = b;
    }
}

void test3 ()
{
    e2_blending_map<double> m;
    m.set_e2 (10.0);
    m.set_fixation (0, 0);
    for (int i = 0; i < 20; ++i)
    {
        double l = m.get_blending_level (superpixel (i, 0, 0));
        double e = m.get_eccentricity (l);
        //clog << l << "\t";
        //clog << e << endl;
        VERIFY (about_equal (e, i));
    }
}

void test4 ()
{
    e2_blending_map<double> m;
    m.set_e2 (1000.0);
    m.set_fixation (5, 5);
    vector<rect<double> > r = m.get_blending_regions (0);
    VERIFY (r.size () == 1);
    VERIFY (r[0].get_x () < 0);
    VERIFY (r[0].get_y () < 0);
    VERIFY (r[0].get_width () > 10);
    VERIFY (r[0].get_height () > 10);
}

void test5 ()
{
    size_t R = 128;
    size_t C = 128;
    e2_blending_map<double> m;
    m.set_e2 (0.1);
    m.set_fixation (C / 2, R / 2);
    for (size_t i = 0; i < 7; ++i)
    {
        vector<rect<double> > r = m.get_blending_regions (i);
        //clog << "level " << i << " has " << r.size () << " regions" << endl;
        for (size_t j = 0; j < r.size (); ++j)
        {
            //clog << "x: " << r[j].x << "\t";
            //clog << "y: " << r[j].y << "\t";
            //clog << "w: " << r[j].w << "\t";
            //clog << "h: " << r[j].h << "\t";
            //clog << endl;
        }
        VERIFY (r.size () == 1);
    }
}

void test6 ()
{
    size_t R = 16;
    size_t C = 16;
    typedef raster<float> B;
    B b (R, C, 0);
    // b's quadrants will look like
    // 0 1
    // 2 3
    for (size_t i = 0; i < R; ++i)
    {
        for (size_t j = 0; j < C; ++j)
        {
            if (j >= C / 2) b (i, j) = 1;
            if (i >= R / 2) b (i, j) += 2;
        }
    }
    bitmap_blending_map<B,double> m;
    m.set_bitmap (b);
    m.set_fixation (C / 2, R / 2);
    //clog << b;
    m.set_divisor (1);
    //clog << "divisor: " << m.get_divisor () << endl;
    vector<vector<rect<double> > > rr1;
    for (size_t i = 0; i < 7; ++i)
    {
        vector<rect<double> > r = m.get_blending_regions (i);
        // If there are no rects, this is the highest level
        if (r.empty ())
            break;
        rr1.push_back (r);
    }
    //clog << rr1.size () << " levels of regions" << endl;
    VERIFY (rr1.size () == 4);
    m.set_divisor (2);
    //clog << "divisor: " << m.get_divisor () << endl;
    vector<vector<rect<double> > > rr2;
    for (size_t i = 0; i < 7; ++i)
    {
        vector<rect<double> > r = m.get_blending_regions (i);
        // If there are no rects, this is the highest level
        if (r.empty ())
            break;
        rr2.push_back (r);
    }
    //clog << rr2.size () << " levels of regions" << endl;
    VERIFY (rr2.size () == 2);
    m.set_divisor (0.5);
    //clog << "divisor: " << m.get_divisor () << endl;
    vector<vector<rect<double> > > rr3;
    for (size_t i = 0; i < 7; ++i)
    {
        vector<rect<double> > r = m.get_blending_regions (i);
        // If there are no rects, this is the highest level
        if (r.empty ())
            break;
        rr3.push_back (r);
    }
    //clog << rr3.size () << " levels of regions" << endl;
    VERIFY (rr3.size () == 7);
    /*
    for (size_t i = 0; i < 7; ++i)
    {
        vector<rect<double> > r = m.get_blending_regions (i);
        for (size_t j = 0; j < r.size (); ++j)
        {
            clog << i << ":";
            if (!r[j].empty ())
            {
                clog << "x: " << r[j].get_x () << "\t";
                clog << "y: " << r[j].get_y () << "\t";
                clog << "w: " << r[j].get_width () << "\t";
                clog << "h: " << r[j].get_height () << "\t";
            }
            else
            {
                clog << "empty";
            }
            clog << endl;
        }
    }
    */
}

int main ()
{
    try
    {
        test0 ();
        test1 ();
        test2 ();
        test3 ();
        test4 ();
        test5 ();
        test6 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
