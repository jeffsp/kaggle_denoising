/// @file jack_rabbit.h
/// @brief raster container adapter and related functions
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

/// \mainpage Raster Utilities
///
/// \section intro_section Introduction
///
/// Raster is an STL compliant container adapter that works like an STL vector but also allows 2D
/// indexing.
///
/// Example:
///
/// \code
///        raster<int> a (4, 3, 9);
///        cout << a;
///        //   9   9   9
///        //   9   9   9
///        //   9   9   9
///        //   9   9   9
///
///        a (2, 1) = 10;
///        a (3, 2) = 11;
///        cout << a;
///        //   9   9   9
///        //   9   9   9
///        //   9  10   9
///        //   9   9  11
///
///        raster<int> b (5, 6);
///        subregion s = { 1, 2, 3, 3 };
///        for (auto i = b.begin (s); i != b.end (s); ++i)
///            *i = 4;
///        cout << b;
///        //   0   0   0   0   0   0
///        //   0   0   4   4   4   0
///        //   0   0   4   4   4   0
///        //   0   0   4   4   4   0
///        //   0   0   0   0   0   0
///
///        raster<int> c (s.rows, s.cols);
///        int n = 0;
///        for (auto i = 0ul; i < c.size (); ++i)
///            c[i] = ++n;
///        cout << c;
///        //   1   2   3
///        //   4   5   6
///        //   7   8   9
///
///        copy (c.begin (), c.end (), b.begin (s));
///        cout << b;
///        //   0   0   0   0   0   0
///        //   0   0   1   2   3   0
///        //   0   0   4   5   6   0
///        //   0   0   7   8   9   0
///        //   0   0   0   0   0   0
///        b.insert_cols (b.loc (1, 1), 1, -1);
///        b.insert_rows (b.loc (1, 1), 2, -2);
///        cout << b;
///        //   0  -1   0   0   0   0
///        //  -2  -2  -2  -2  -2  -2
///        //  -2  -2  -2  -2  -2  -2
///        //   0  -1   1   2   3   0
///        //   0  -1   4   5   6   0
///        //   0  -1   7   8   9   0
///        //   0  -1   0   0   0   0
///
///        // Compute Manhattan distance from (0,0)
///        raster<size_t> d (7, 5);
///        generate (d.begin (), d.end (),
///        subscript_generator<size_t,ManhattanDistance> (d.rows (), d.cols ()));
///        cout << d;
///        //   0   1   2   3   4
///        //   1   2   3   4   5
///        //   2   3   4   5   6
///        //   3   4   5   6   7
///        //   4   5   6   7   8
///        //   5   6   7   8   9
///        //   6   7   8   9  10
///
///        // Compute distance from center
///        raster<int> e (9, 9, 1);
///        subscript_unary_function<int,DistanceFromCenter> f (e.rows (), e.cols ());
///        transform (e.begin (), e.end (), e.begin (), f);
///        cout << e;
///        //   5   5   4   4   4   4   4   5   5
///        //   5   4   3   3   3   3   3   4   5
///        //   4   3   2   2   2   2   2   3   4
///        //   4   3   2   1   1   1   2   3   4
///        //   4   3   2   1   0   1   2   3   4
///        //   4   3   2   1   1   1   2   3   4
///        //   4   3   2   2   2   2   2   3   4
///        //   5   4   3   3   3   3   3   4   5
///        //   5   5   4   4   4   4   4   5   5

/// \endcode
///
/// If you want more dimensions, use boost MultiArray.

#ifndef JACK_RABBIT_H
#define JACK_RABBIT_H

#include "raster.h"
#include "subregion_algo.h"
#include "subregion.h"
#include "subregion_iter.h"
#include "subscript_function.h"

#endif // JACK_RABBIT_H
