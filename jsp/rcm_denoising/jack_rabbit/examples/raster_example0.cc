/// @file raster_example0.cc
/// @brief raster example
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "jack_rabbit/jack_rabbit.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
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
    s << endl;
    return s;
}

template<typename T>
struct ManhattanDistance
{
    ManhattanDistance (size_t /*r*/, size_t /*c*/) { }
    T operator() (size_t r, size_t c)
    {
        return static_cast<T> (r + c);
    }
};

template<typename T>
struct DistanceFromCenter
{
    DistanceFromCenter (size_t r, size_t c) : r_ (r), c_ (c) { }
    T operator() (size_t r, size_t c, const T &v)
    {
        float dx = c - (c_ - 1) / 2.0f;
        float dy = r - (r_ - 1) / 2.0f;
        float d = sqrt (dx * dx + dy * dy);
        return static_cast<T> (v * d);
    }
    size_t r_, c_;
};

int main ()
{
    try
    {
        raster<int> a (4, 3, 9);
        cout << a;
        //   9   9   9
        //   9   9   9
        //   9   9   9
        //   9   9   9

        a (2, 1) = 10;
        a (3, 2) = 11;
        cout << a;
        //   9   9   9
        //   9   9   9
        //   9  10   9
        //   9   9  11

        raster<int> b (5, 6);
        subregion s = { 1, 2, 3, 3 };
        for (auto i = b.begin (s); i != b.end (s); ++i)
            *i = 4;
        cout << b;
        //   0   0   0   0   0   0
        //   0   0   4   4   4   0
        //   0   0   4   4   4   0
        //   0   0   4   4   4   0
        //   0   0   0   0   0   0

        raster<int> c (s.rows, s.cols);
        int n = 0;
        for (auto i = 0ul; i < c.size (); ++i)
            c[i] = ++n;
        cout << c;
        //   1   2   3
        //   4   5   6
        //   7   8   9

        copy (c.begin (), c.end (), b.begin (s));
        cout << b;
        //   0   0   0   0   0   0
        //   0   0   1   2   3   0
        //   0   0   4   5   6   0
        //   0   0   7   8   9   0
        //   0   0   0   0   0   0

        b.erase_col (b.loc (0, 1));
        cout << b;
        //   0   0   0   0   0
        //   0   1   2   3   0
        //   0   4   5   6   0
        //   0   7   8   9   0
        //   0   0   0   0   0

        b.insert_cols (b.loc (1, 1), 1, -1);
        b.insert_rows (b.loc (1, 1), 2, -2);
        cout << b;
        //   0  -1   0   0   0   0
        //  -2  -2  -2  -2  -2  -2
        //  -2  -2  -2  -2  -2  -2
        //   0  -1   1   2   3   0
        //   0  -1   4   5   6   0
        //   0  -1   7   8   9   0
        //   0  -1   0   0   0   0

        // Compute Manhattan distance from (0,0)
        raster<size_t> d (7, 5);
        generate (d.begin (), d.end (),
            subscript_generator<size_t,ManhattanDistance> (d.rows (), d.cols ()));
        cout << d;
        //   0   1   2   3   4
        //   1   2   3   4   5
        //   2   3   4   5   6
        //   3   4   5   6   7
        //   4   5   6   7   8
        //   5   6   7   8   9
        //   6   7   8   9  10

        // Compute distance from center
        raster<int> e (9, 9, 1);
        subscript_unary_function<int,DistanceFromCenter> f (e.rows (), e.cols ());
        transform (e.begin (), e.end (), e.begin (), f);
        cout << e;
        //   5   5   4   4   4   4   4   5   5
        //   5   4   3   3   3   3   3   4   5
        //   4   3   2   2   2   2   2   3   4
        //   4   3   2   1   1   1   2   3   4
        //   4   3   2   1   0   1   2   3   4
        //   4   3   2   1   1   1   2   3   4
        //   4   3   2   2   2   2   2   3   4
        //   5   4   3   3   3   3   3   4   5
        //   5   5   4   4   4   4   4   5   5

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
