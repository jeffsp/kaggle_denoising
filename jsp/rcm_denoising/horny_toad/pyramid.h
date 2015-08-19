/// @file pyramid.h
/// @brief pyramid container
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef PYRAMID_H
#define PYRAMID_H

#include "jack_rabbit/raster.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>

namespace // anonymous
{

// Log base 2
inline double log2 (double x) { return log (x) / log (2.0); }
// Next lowest power of 2
inline size_t next_lowest_pow2 (double x) { return static_cast<size_t> (floor (log2 (x))); }
// Next highest power of 2
inline size_t next_highest_pow2 (double x) { return static_cast<size_t> (ceil (log2 (x))); }

// A traits class for accumulation
template<typename T>
class Acc
{
    public:
    typedef T acc_type;
};

// A traits class for accumulation
template<>
class Acc<unsigned char>
{
    public:
    typedef unsigned acc_type;
};

// A traits class for accumulation
template<>
class Acc<char>
{
    public:
    typedef int acc_type;
};

// Expand a single row
template<typename Src,typename Dest>
inline void ExpandRow3x3 (const Src &src, Dest &dest, size_t dest_row)
{
    // Fill column 0 and 1
    size_t dr = dest_row;
    size_t sr = dest_row / 2;
    assert (dest.index (dr, 0) < dest.size ());
    assert (dest.index (dr, 1) < dest.size ());
    assert (src.index (sr, 0) < src.size ());
    dest (dr, 0) = src (sr, 0);
    dest (dr, 1) = src (sr, 0);
    // Fill columns in the middle
    for (size_t c = 3; c < dest.cols (); c += 2)
    {
        assert (dest.index (dr, c) < dest.size ());
        assert (src.index (sr, c / 2) < src.size ());
        assert (dest.index (dr, c - 1) < dest.size ());
        assert (dest.index (dr, c - 2) < dest.size ());
        dest (dr, c) = src (sr, c / 2);
        dest (dr, c - 1) =
            (dest (dr, c) + dest (dr, c - 2)) / 2;
    }
    // Fill the last column, if needed
    if (dest.cols () & 1)
    {
        assert (dest.index (dr, dest.cols () - 1) < dest.size ());
        assert (dest.index (dr, dest.cols () - 2) < dest.size ());
        dest (dr, dest.cols () - 1) = dest (dr, dest.cols () - 2);
    }
}

// Expand a single row with jitter
template<typename Src,typename Dest>
inline void ExpandRow3x3 (const Src &src, Dest &dest, size_t dest_row, unsigned jitter)
{
    // Fill column 0 and 1
    size_t dr = dest_row;
    size_t sr = dest_row / 2;
    assert (dest.index (dr, 0) < dest.size ());
    assert (dest.index (dr, 1) < dest.size ());
    assert (src.index (sr, 0) < src.size ());
    dest (dr, 0) = src (sr, 0);
    dest (dr, 1) = src (sr, 0);
    // Fill columns in the middle
    for (size_t c = 3; c < dest.cols (); c += 2)
    {
        assert (dest.index (dr, c) < dest.size ());
        assert (src.index (sr, c / 2) < src.size ());
        assert (dest.index (dr, c - 1) < dest.size ());
        assert (dest.index (dr, c - 2) < dest.size ());
        dest (dr, c) = src (sr, c / 2);
        // Add jitter
        unsigned j1 = 100 - rand () % (jitter + 1);
        unsigned j2 = 200 - j1;
        if (rand () % 2)
            std::swap (j1, j2);
        dest (dr, c - 1) =
            (dest (dr, c) * j1 + dest (dr, c - 2) * j2) / 200;
    }
    // Fill the last column, if needed
    if (dest.cols () & 1)
    {
        assert (dest.index (dr, dest.cols () - 1) < dest.size ());
        assert (dest.index (dr, dest.cols () - 2) < dest.size ());
        dest (dr, dest.cols () - 1) = dest (dr, dest.cols () - 2);
    }
}

// Copy from one row to another
template<typename T>
inline void CopyRow (T &m, size_t src_row, size_t dest_row)
{
    assert (m.index (src_row, m.cols () - 1) < m.size ());
    assert (m.index (dest_row, m.cols () - 1) < m.size ());
    memcpy (&m (dest_row, 0), &m (src_row, 0), m.cols () * sizeof (typename T::value_type));
}

// Average two rows together
template<typename T>
inline void AverageRows (T &m, size_t sr1, size_t sr2, size_t dr)
{
    for (size_t i = 0; i < m.cols (); ++i)
    {
        assert (m.index (dr, i) < m.size ());
        assert (m.index (sr1, i) < m.size ());
        assert (m.index (sr2, i) < m.size ());
        m (dr, i) = (m (sr1, i) + m (sr2, i)) / 2;
    }
}

// Average two rows together with jitter
template<typename T>
inline void AverageRows (T &m, size_t sr1, size_t sr2, size_t dr, unsigned jitter)
{
    for (size_t i = 0; i < m.cols (); ++i)
    {
        assert (m.index (dr, i) < m.size ());
        assert (m.index (sr1, i) < m.size ());
        assert (m.index (sr2, i) < m.size ());
        // Add jitter
        unsigned j1 = 100 - rand () % (jitter + 1);
        unsigned j2 = 200 - j1;
        if (rand () % 2)
            std::swap (j1, j2);
        m (dr, i) = (m (sr1, i) * j1 + m (sr2, i) * j2) / 200;
    }
}

// Perform an expand and blur on a single pixel
template<class Src,class Dest>
inline void expand2x2_and_blur3x3_clipped (const Src &src,
    Dest &dest,
    size_t dest_row,
    size_t dest_col)
{
    // Clip the boundaries if you need to
    size_t dest_r_begin = (dest_row == 0 ? 0 : dest_row - 1);
    size_t dest_r_end = (dest_row + 1 == dest.rows () ? dest.rows () : dest_row + 2);
    size_t dest_c_begin = (dest_col == 0 ? 0 : dest_col - 1);
    size_t dest_c_end = (dest_col + 1 == dest.cols () ? dest.cols () : dest_col + 2);
    size_t n = 0;
    float acc = 0;
    for (size_t r = dest_r_begin; r < dest_r_end; ++r)
    {
        size_t src_r = r / 2;
        for (size_t c = dest_c_begin; c < dest_c_end; ++c)
        {
            size_t src_c = c / 2;
            assert (src.index (src_r, src_c) < src.size ());
            acc += src (src_r, src_c);
            ++n;
        }
    }
    assert (dest.index (dest_row, dest_col) < dest.size ());
    dest (dest_row, dest_col) = static_cast<typename Dest::value_type> (round (acc / n));
}

} // namespace anonymous

namespace horny_toad
{
/// \example pyramid_example0.cc
/// This is a simple example of how to use the pyramid class.

/// \example pyramid_example1.cc
/// This example generates multiresolution noise.
/// \image html pyramid_example1_color.png "pyramid example1 color output"
/// \image html pyramid_example1_grayscale.png "pyramid example1 grayscale output"

/// @brief Reduce using a 2x2 filter kernel
/// @param src source image
/// @param dest dest image
///
/// The destination must be a factor of two smaller than the
/// source in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The destination must
/// contain at least one pixel.
template<class Src,class Dest>
inline void reduce2x2 (const Src &src, Dest &dest)
{
    assert (dest.rows () == (src.rows () + 1) / 2);
    assert (dest.cols () == (src.cols () + 1) / 2);
    assert (dest.rows () != 0);
    assert (dest.cols () != 0);
    const size_t R = src.rows () - 1;
    const size_t C = src.cols () - 1;
    typedef typename Dest::value_type T;
    for (size_t y = 0; y < R; y += 2)
    {
        for (size_t x = 0; x < C; x += 2)
        {
            assert (src.index (y + 1, x + 1) < src.size ());
            T p1 = src (y, x);
            T p2 = src (y, x + 1);
            T p3 = src (y + 1, x);
            T p4 = src (y + 1, x + 1);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) = static_cast<T> ((p1 + p2 + p3 + p4) / 4);
        }
    }
    // Fix the right column, if needed
    if (src.cols () & 1)
    {
        for (size_t y = 0; y < R; y += 2)
        {
            size_t x = src.cols () - 1;
            assert (src.index (y + 1, x) < src.size ());
            T p1 = src (y, x);
            T p3 = src (y + 1, x);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) = static_cast<T> ((p1 + p3) / 2);
        }
    }
    // Fix the bottom row, if needed
    if (src.rows () & 1)
    {
        size_t y = src.rows () - 1;
        for (size_t x = 0; x < C; x += 2)
        {
            assert (src.index (y, x + 1) < src.size ());
            T p1 = src (y, x);
            T p2 = src (y, x + 1);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) = static_cast<T> ((p1 + p2) / 2);
        }
    }
    // Fix the bottom right corner, if needed
    if ((src.cols () & 1) && (src.rows () & 1))
    {
        size_t y = src.rows () - 1;
        size_t x = src.cols () - 1;
        T p1 = src (y, x);
        assert (src.index (y, x) < src.size ());
        assert (dest.index (y / 2, x / 2) < dest.size ());
        dest (y / 2, x / 2) = static_cast<T> (p1);
    }
}

/// @brief Expand image that was reduced with a 2x2 filter kernel
/// @param src source image
/// @param dest dest image
///
/// The source must be a factor of two smaller than the
/// destination in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The source must
/// contain at least one pixel.
template<class Src,class Dest>
inline void expand2x2 (const Src &src, Dest &dest)
{
    assert (src.rows () == (dest.rows () + 1) / 2);
    assert (src.cols () == (dest.cols () + 1) / 2);
    assert (src.rows () != 0);
    assert (src.cols () != 0);
    const size_t R = dest.rows ();
    const size_t C = dest.cols ();
    for (size_t y = 0; y < R; ++y)
    {
        for (size_t x = 0; x < C; ++x)
        {
            assert (dest.index (y, x) < dest.size ());
            assert (src.index (y / 2, x / 2) < src.size ());
            dest (y, x) = src (y / 2, x / 2);
        }
    }
}

/// @brief Expand and blur an image
/// @param src source image
/// @param dest dest image
///
/// A 2x2 expand produces block artifacts.  To minimize the
/// artifacts, we can apply a low pass filter to the
/// expanded image.  Here, we apply the filter as we go for
/// speed.
///
/// The source must be a factor of two smaller than the
/// destination in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The source must
/// contain at least one pixel.
template<class Src,class Dest>
inline void expand2x2_and_blur3x3 (const Src &src, Dest &dest)
{
    assert (src.rows () == (dest.rows () + 1) / 2);
    assert (src.cols () == (dest.cols () + 1) / 2);
    assert (src.rows () != 0);
    assert (src.cols () != 0);
    const size_t R = dest.rows ();
    const size_t C = dest.cols ();
    // Do the edges with the slow, clipping function
    for (size_t y = 0; y < R; ++y)
    {
        expand2x2_and_blur3x3_clipped (src, dest, y, 0);
        expand2x2_and_blur3x3_clipped (src, dest, y, C - 1);
    }
    for (size_t x = 0; x < C; ++x)
    {
        expand2x2_and_blur3x3_clipped (src, dest, 0, x);
        expand2x2_and_blur3x3_clipped (src, dest, R - 1, x);
    }
    // Do the rest with the fast, non-clipping function
    const size_t R2 = dest.rows () - 1;
    const size_t C2 = dest.cols () - 1;
    for (size_t y = 1; y < R2; y += 2)
    {
        const typename Src::value_type *src_p0 = &src (y / 2, 0);
        const typename Src::value_type *src_p2 = &src (y / 2 + 1, 0);
        typename Dest::value_type *dest_p0 = &dest (y, 1);
        typename Dest::value_type *dest_p2 = &dest (y + 1, 1);
        for (size_t x = 1; x < C2; x += 2)
        {
            assert (src.index (y / 2, x / 2) < src.size ());
            assert (src.index (y / 2 + 1, x / 2 + 1) < src.size ());
            // p?'s type here makes a huge difference on an
            // intel dual core x64-- don't try anything
            // silly like going to float and then casting
            // back to int-- you will take a huge
            // performance hit... (10x)
            typename Acc<typename Src::value_type>::acc_type p0 = *src_p0++;
            typename Acc<typename Src::value_type>::acc_type p1 = *src_p0;
            typename Acc<typename Src::value_type>::acc_type p2 = *src_p2++;
            typename Acc<typename Src::value_type>::acc_type p3 = *src_p2;
            *dest_p0++ = static_cast<typename Dest::value_type> ((p0 * 2 + p1 + p2) / 4);
            *dest_p0++ = static_cast<typename Dest::value_type> ((p1 * 2 + p0 + p3) / 4);
            *dest_p2++ = static_cast<typename Dest::value_type> ((p2 * 2 + p0 + p3) / 4);
            *dest_p2++ = static_cast<typename Dest::value_type> ((p3 * 2 + p1 + p2) / 4);
        }
    }
}

/// @brief Reduce using a 3x3 filter kernel
/// @param src source image
/// @param dest dest image
///
/// The destination must be a factor of two smaller than the
/// source in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The destination must
/// contain at least one pixel.
template<class Src,class Dest>
inline void reduce3x3 (const Src &src, Dest &dest)
{
    assert (dest.rows () == (src.rows () + 1) / 2);
    assert (dest.cols () == (src.cols () + 1) / 2);
    assert (src.rows () > 1);
    assert (src.cols () > 1);
    assert (dest.rows () > 0);
    assert (dest.cols () > 0);
    typedef typename Dest::value_type DestT;
    for (size_t y = 0; y < src.rows () - 2; y += 2)
    {
        for (size_t x = 0; x < src.cols () - 2; x += 2)
        {
            assert (src.index (y + 2, x + 2) < src.size ());
            DestT p1 = src (y + 0, x + 0);
            DestT p2 = src (y + 0, x + 1);
            DestT p3 = src (y + 0, x + 2);
            DestT p4 = src (y + 1, x + 0);
            DestT p5 = src (y + 1, x + 1);
            DestT p6 = src (y + 1, x + 2);
            DestT p7 = src (y + 2, x + 0);
            DestT p8 = src (y + 2, x + 1);
            DestT p9 = src (y + 2, x + 2);
            // A Gaussian weighting function with a standard
            // deviation of 1 would look a lot like this:
            //
            // *d++ = static_cast<T> (
            //           (p1*13 + p2*37  + p3*13 +
            //            p4*37 + p5*100 + p6*37 +
            //            p7*13 + p8*37  + p9*13) / 300);
            //
            // But this one is much faster -- sometimes over
            // twice as fast:
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) =
                (p1   + p2*2 + p3   +
                 p4*2 + p5*4 + p6*2 +
                 p7   + p8*2 + p9   + 8) / 16;
        }
    }
    // If the number of columns is even, we have to fix the
    // right hand side by mirroring it...
    if (!(src.cols () & 1))
    {
        size_t x = src.cols () - 2;
        for (size_t y = 0; y < src.rows () - 2; y += 2)
        {
            assert (src.index (y + 2, x + 1) < src.size ());
            DestT p1 = src (y + 0, x + 0);
            DestT p2 = src (y + 0, x + 1);
            DestT p4 = src (y + 1, x + 0);
            DestT p5 = src (y + 1, x + 1);
            DestT p7 = src (y + 2, x + 0);
            DestT p8 = src (y + 2, x + 1);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) =
                (p1   + p2*2 + p1   +
                 p4*2 + p5*4 + p4*2 +
                 p7   + p8*2 + p7   + 8) / 16;
        }
    }
    // Otherwise, we have to do this...
    else
    {
        size_t x = src.cols () - 1;
        for (size_t y = 0; y < src.rows () - 2; y += 2)
        {
            assert (src.index (y + 2, x + 0) < src.size ());
            DestT p1 = src (y + 0, x + 0);
            DestT p4 = src (y + 1, x + 0);
            DestT p7 = src (y + 2, x + 0);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) =
                (p1   + p1*2 + p1   +
                 p4*2 + p4*4 + p4*2 +
                 p7   + p7*2 + p7   + 8) / 16;
        }
    }
    // If the number of rows is even, we have to fix the
    // bottom side by mirroring it...
    if (!(src.rows () & 1))
    {
        size_t y = src.rows () - 2;
        for (size_t x = 0; x < src.cols () - 2; x += 2)
        {
            assert (src.index (y + 1, x + 2) < src.size ());
            DestT p1 = src (y + 0, x + 0);
            DestT p2 = src (y + 0, x + 1);
            DestT p3 = src (y + 0, x + 2);
            DestT p4 = src (y + 1, x + 0);
            DestT p5 = src (y + 1, x + 1);
            DestT p6 = src (y + 1, x + 2);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) =
                (p1   + p2*2 + p3   +
                 p4*2 + p5*4 + p6*2 +
                 p1   + p2*2 + p3   + 8) / 16;
        }
    }
    // Otherwise, we have to do this...
    else
    {
        size_t y = src.rows () - 1;
        for (size_t x = 0; x < src.cols () - 2; x += 2)
        {
            assert (src.index (y + 0, x + 2) < src.size ());
            DestT p1 = src (y + 0, x + 0);
            DestT p2 = src (y + 0, x + 1);
            DestT p3 = src (y + 0, x + 2);
            assert (dest.index (y / 2, x / 2) < dest.size ());
            dest (y / 2, x / 2) =
                (p1   + p2*2 + p3   +
                 p1*2 + p2*4 + p3*2 +
                 p1   + p2*2 + p3   + 8) / 16;
        }
    }
    // We have to fix the bottom right corner...
    dest.back () = src.back ();
}

/// @brief Expand image that was reduced with a 3x3 filter kernel
/// @param src source image
/// @param dest dest image
///
/// The source must be a factor of two smaller than the
/// destination in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The source must
/// contain at least one pixel.
/*
template<class Src,class Dest>
inline void expand3x3 (const Src &src, Dest &dest)
{
    assert (src.rows () == (dest.rows () + 1) / 2);
    assert (src.cols () == (dest.cols () + 1) / 2);
    assert (src.rows () != 0);
    assert (src.cols () != 0);
    // Upsample
    for (size_t i = 1; i < dest.rows (); i += 2)
    {
        for (size_t j = 1; j < dest.cols (); j += 2)
        {
            assert (src.index (i / 2, j / 2) < src.size ());
            assert (dest.index (i, j) < dest.size ());
            dest (i, j) = src (i / 2, j / 2);
        }
    }
    // Interpolate between 4 X-shaped pixels
    for (size_t i = 1; i + 2 < dest.rows (); i += 2)
    {
        for (size_t j = 1; j + 2 < dest.cols (); j += 2)
        {
            assert (dest.index (i + 2, j + 2) < dest.size ());
            dest (i + 1, j + 1) =
                (
                dest (i, j) +
                dest (i + 2, j) +
                dest (i, j + 2) +
                dest (i + 2, j + 2)
                + 2
                ) / 4;
        }
    }
    // Interpolate between 4 +-shaped pixels on even rows
    for (size_t i = 1; i + 2 < dest.rows (); i += 2)
    {
        for (size_t j = 2; j + 2 < dest.cols (); j += 2)
        {
            assert (dest.index (i + 2, j + 2) < dest.size ());
            dest (i + 1, j + 1) =
                (
                dest (i, j + 1) +
                dest (i + 1, j) +
                dest (i + 1, j + 2) +
                dest (i + 2, j + 1)
                + 2
                ) / 4;
        }
    }
    // Interpolate between 4 +-shaped pixels on odd rows
    for (size_t i = 2; i + 2 < dest.rows (); i += 2)
    {
        for (size_t j = 1; j + 2 < dest.cols (); j += 2)
        {
            assert (dest.index (i + 2, j + 2) < dest.size ());
            dest (i + 1, j + 1) =
                (
                dest (i, j + 1) +
                dest (i + 1, j) +
                dest (i + 1, j + 2) +
                dest (i + 2, j + 1)
                + 2
                ) / 4;
        }
    }
    // Fill in empty spaces on 2nd row
    for (size_t j = 1; j + 2 < dest.cols (); j += 2)
    {
        assert (dest.index (1, j + 2) < dest.size ());
        dest (1, j + 1) = (dest (1, j) + dest (1, j + 2) + 1) / 2;
    }
    // Fill in empty spaces on 2nd column
    for (size_t i = 1; i + 2 < dest.rows (); i += 2)
    {
        assert (dest.index (i + 2, 1) < dest.size ());
        dest (i + 1, 1) = (dest (i, 1) + dest (i + 2, 1) + 1) / 2;
    }
    // Fix last 2 rows
    if (dest.rows () > 2)
    {
        if (dest.rows () & 1)
        {
            // Fill in empty space on 2nd to last row
            size_t i = dest.rows () - 2;
            for (size_t j = 1; j + 2 < dest.cols (); j += 2)
            {
                assert (dest.index (i, j + 2) < dest.size ());
                dest (i, j + 1) = (dest (i, j) + dest (i, j + 2) + 1) / 2;
            }
            // Then copy 2nd to last row to last row
            std::copy (&dest (i, 0), &dest (i + 1, 0), &dest (i + 1, 0));
        }
        else
        {
            // Only fill in empties, no copy needed
            size_t i = dest.rows () - 1;
            for (size_t j = 1; j + 2 < dest.cols (); j += 2)
            {
                assert (dest.index (i, j + 2) < dest.size ());
                dest (i, j + 1) = (dest (i, j) + dest (i, j + 2) + 1) / 2;
            }
        }
    }
    // Fix last 2 columns
    if (dest.cols () > 2)
    {
        if (dest.cols () & 1)
        {
            // Fill in empty space on 2nd to last column
            size_t j = dest.cols () - 2;
            for (size_t i = 1; i + 2 < dest.rows (); i += 2)
            {
                assert (dest.index (i + 2, j) < dest.size ());
                dest (i + 1, j) = (dest (i, j) + dest (i + 2, j) + 1) / 2;
            }
            // Then copy 2nd to last column to last column
            j = dest.cols () - 1;
            for (size_t i = 0; i < dest.rows (); ++i)
                dest (i, j) = dest (i, j - 1);
        }
        else
        {
            // Only fill in empties, no copy needed
            size_t j = dest.cols () - 1;
            for (size_t i = 1; i + 2 < dest.rows (); i += 2)
            {
                assert (dest.index (i + 2, j) < dest.size ());
                dest (i + 1, j) = (dest (i, j) + dest (i + 2, j) + 1) / 2;
            }
        }
    }
    // Copy 2nd row to 1st row
    std::copy (&dest (1, 0), &dest (2, 0), &dest (0, 0));
    // Copy 2nd column to 1st column
    for (size_t i = 0; i < dest.rows (); ++i)
        dest (i, 0) = dest (i, 1);
}
*/

/// @brief Expand image that was reduced with a 3x3 filter kernel
/// @param src source image
/// @param dest dest image
///
/// The source must be a factor of two smaller than the
/// destination in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The source must
/// contain at least one pixel.
template<class Src,class Dest>
inline void expand3x3 (const Src &src, Dest &dest)
{
    assert (src.rows () == (dest.rows () + 1) / 2);
    assert (src.cols () == (dest.cols () + 1) / 2);
    assert (src.rows () != 0);
    assert (src.cols () != 0);
    // Do row 1
    ExpandRow3x3 (src, dest, 1);
    // Copy it to row 0
    CopyRow (dest, 1, 0);
    for (size_t r2 = 3; r2 < dest.rows (); r2 += 2)
    {
        size_t r0 = r2 - 2;
        size_t r1 = r2 - 1;
        ExpandRow3x3 (src, dest, r2);
        AverageRows (dest, r0, r2, r1);
    }
    // If odd number of rows, we missed the last row
    if (dest.rows () & 1)
        CopyRow (dest, dest.rows () - 2, dest.rows () - 1);
}

/// @brief Expand image that was reduced with a 3x3 filter kernel
/// @param src source image
/// @param dest dest image
/// @param jitter amount of jitter [1,100]
///
/// The source must be a factor of two smaller than the
/// destination in both dimensions (rounding up when the
/// dimension is not divisible by 2).  The source must
/// contain at least one pixel.
template<class Src,class Dest>
inline void expand3x3 (const Src &src, Dest &dest, unsigned jitter)
{
    assert (src.rows () == (dest.rows () + 1) / 2);
    assert (src.cols () == (dest.cols () + 1) / 2);
    assert (src.rows () != 0);
    assert (src.cols () != 0);
    // Do row 1
    ExpandRow3x3 (src, dest, 1, jitter);
    // Copy it to row 0
    CopyRow (dest, 1, 0);
    for (size_t r2 = 3; r2 < dest.rows (); r2 += 2)
    {
        size_t r0 = r2 - 2;
        size_t r1 = r2 - 1;
        ExpandRow3x3 (src, dest, r2, jitter);
        AverageRows (dest, r0, r2, r1, jitter);
    }
    // If odd number of rows, we missed the last row
    if (dest.rows () & 1)
        CopyRow (dest, dest.rows () - 2, dest.rows () - 1);
}

/// @brief pyramid container adapter
///
/// Implements a multi-resolution pyramid.  The base of the
/// pyramid contains a high resolution image.  Higher levels
/// contain lower resolution images.
template<
    typename T,
    class M = jack_rabbit::raster<T>,
    class Cont = std::vector<M>
>
class pyramid
{
    public:
    //@{
    typedef pyramid<T,M,Cont> self_type;
    typedef typename Cont::value_type value_type;
    typedef typename Cont::pointer pointer;
    typedef typename Cont::const_pointer const_pointer;
    typedef typename Cont::reference reference;
    typedef typename Cont::const_reference const_reference;
    typedef typename Cont::size_type size_type;
    typedef typename Cont::difference_type difference_type;
    typedef typename Cont::allocator_type allocator_type;
    typedef typename Cont::iterator iterator;
    typedef typename Cont::const_iterator const_iterator;
    //@}
    /// @brief Size constructor
    /// @param rows number of rows in base raster
    /// @param cols number of columns in base raster
    /// @param v optional initialization value
    pyramid (size_type rows = 0, size_type cols = 0, const T &v = T ())
    {
        resize (rows, cols, v);
    }
    /// @brief Size constructor
    /// @param rows number of rows in base raster
    /// @param cols number of columns in base raster
    /// @param levels number of levels in pyramid
    /// @param v initialization value
    pyramid (size_type rows, size_type cols, size_type levels, const T &v)
    {
        resize (rows, cols, levels, v);
    }
    /// @brief Copy constructor
    /// @param p pyramid to copy
    pyramid (const pyramid<T,M> &p)
        : m_ (p.m_)
    { }
    /// @brief Size constructor
    /// @param m base raster
    pyramid (const M &m)
    {
        size_type rows = m.rows ();
        size_type cols = m.cols ();
        size_type levels = get_levels (rows, cols);
        m_.resize (levels);
        if (!m_.empty ())
            m_[0] = m;
        // Create the rest
        for (size_type l = 1; l < m_.size (); ++l)
        {
            rows = (rows + 1) / 2;
            cols = (cols + 1) / 2;
            m_[l].resize (rows, cols);
        }
    }
    /// @brief Size constructor
    /// @param m base raster
    /// @param levels number of pyramid levels
    pyramid (const M &m, size_type levels)
    {
        size_type rows = m.rows ();
        size_type cols = m.cols ();
        m_.resize (levels);
        if (!m_.empty ())
            m_[0] = m;
        // Create the rest
        for (size_type l = 1; l < m_.size (); ++l)
        {
            rows = (rows + 1) / 2;
            cols = (cols + 1) / 2;
            m_[l].resize (rows, cols);
        }
    }
    /// @brief Copy constructor
    /// @param p pyramid to copy
    template<typename SrcT,class SrcM,class SrcCont>
    pyramid (const pyramid<SrcT,SrcM,SrcCont> &p)
        : m_ (p.size ())
    {
        typename self_type::iterator dest = this->begin ();
        typename pyramid<SrcT,SrcM,SrcCont>::const_iterator src = p.begin ();
        const typename pyramid<SrcT,SrcM,SrcCont>::const_iterator src_end = p.end ();
        while (src != src_end)
            *dest++ = *src++;
    }
    ~pyramid () { }
    /// @brief Change base image size
    /// @param rows number of rows in base image
    /// @param cols number of columns in base image
    /// @param v optional initialization value
    void resize (size_type rows, size_type cols, const T &v = T ())
    {
        size_type levels = get_levels (rows, cols);
        m_.resize (levels);
        for (size_type l = 0; l < levels; ++l)
        {
            m_[l].resize (rows, cols, v);
            rows = (rows + 1) / 2;
            cols = (cols + 1) / 2;
        }
    }
    /// @brief Change base image size
    /// @param rows number of rows in base image
    /// @param cols number of columns in base image
    /// @param levels number of levels in pyramid
    /// @param v initialization value
    void resize (size_type rows, size_type cols, size_type levels, const T &v)
    {
        m_.resize (levels);
        for (size_type l = 0; l < levels; ++l)
        {
            m_[l].resize (rows, cols, v);
            rows = (rows + 1) / 2;
            cols = (cols + 1) / 2;
        }
    }
    /// @brief Reduce operation
    ///
    /// Reduce all levels
    void reduce2x2 ()
    {
        for (size_t i = 0; i + 1 < levels (); ++i)
            reduce2x2 (i);
    }
    /// @brief Reduce operation
    ///
    /// Reduce all levels
    void reduce3x3 ()
    {
        for (size_t i = 0; i + 1 < levels (); ++i)
            reduce3x3 (i);
    }
    /// @brief Reduce operation
    /// @param i src level index
    ///
    /// The image at i will get reduced to the image at i+1
    void reduce2x2 (size_type i)
    {
        assert (i + 1 < levels ());
        horny_toad::reduce2x2 (m_[i], m_[i + 1]);
    }
    /// @brief Expand operation
    /// @param i src level index
    ///
    /// The image at i will get expanded to the image at i-1
    void expand2x2 (size_type i)
    {
        assert (i > 0);
        assert (i < levels ());
        horny_toad::expand2x2 (m_[i], m_[i - 1]);
    }
    /// @brief Expand and blur operation
    /// @param i src level index
    ///
    /// The image at i will get expanded to the image at i-1
    void expand2x2_and_blur3x3 (size_type i)
    {
        assert (i > 0);
        assert (i < levels ());
        horny_toad::expand2x2_and_blur3x3 (m_[i], m_[i - 1]);
    }
    /// @brief Reduce operation
    /// @param i src level index
    ///
    /// The image at i will get reduced to the image at i+1
    void reduce3x3 (size_type i)
    {
        assert (i + 1 < levels ());
        horny_toad::reduce3x3 (m_[i], m_[i + 1]);
    }
    /// @brief Expand operation
    /// @param i src level index
    ///
    /// The image at i will get expanded to the image at i-1
    void expand3x3 (size_type i)
    {
        assert (i > 0);
        assert (i < levels ());
        horny_toad::expand3x3 (m_[i], m_[i - 1]);
    }
    /// @brief Expand operation with jitter
    /// @param i src level index
    /// @param j jitter [0,100]
    ///
    /// The image at i will get expanded to the image at i-1
    void expand3x3_with_jitter (size_type i, unsigned j)
    {
        assert (i > 0);
        assert (i < levels ());
        horny_toad::expand3x3 (m_[i], m_[i - 1], j);
    }
    /// @brief Get number of levels in the pyramid
    /// @return the total number of levels in the pyramid
    ///
    /// Higher levels have lower resolution.
    size_type levels () const
    { return m_.size (); }
    /// @brief Get number of levels in the pyramid
    /// @return the total number of levels in the pyramid
    size_type size () const
    { return m_.size (); }
    /// @brief Level access
    /// @param i level index
    reference operator[] (size_type i)
    { return m_[i]; }
    /// @brief Level access
    /// @param i level index
    const_reference operator[] (size_type i) const
    { return m_[i]; }
    /// @brief Iterator access
    ///
    /// We begin at the highest resolution image.
    iterator begin ()
    { return m_.begin (); }
    /// @brief Iterator access
    ///
    /// We begin at the highest resolution image.
    const_iterator begin () const
    { return m_.begin (); }
    /// @brief Iterator access
    ///
    /// We end at the lowest resolution image.
    iterator end ()
    { return m_.end (); }
    /// @brief Iterator access
    ///
    /// We end at the lowest resolution image.
    const_iterator end () const
    { return m_.end (); }
    /// @brief Level access
    ///
    /// The bottom of the pyramid is the highest resolution
    /// image.
    reference bottom ()
    { return m_.front (); }
    /// @brief Level access
    ///
    /// The bottom of the pyramid is the highest resolution
    /// image.
    const_reference bottom () const
    { return m_.front (); }
    /// @brief Level access
    ///
    /// The top of the pyramid is the lowest resolution
    /// image.
    reference top ()
    { return m_.back (); }
    /// @brief Level access
    ///
    /// The top of the pyramid is the lowest resolution
    /// image.
    const_reference top () const
    { return m_.back (); }
    /// @brief Copy operator
    pyramid &operator= (const pyramid &p)
    {
        if (this != &p)
        { m_ = p.m_; }
        return *this;
    }
    /// @brief Swap
    void swap (pyramid &p) { m_.swap (p.m_); }
    /// @brief Indicates if the pyramid is empty
    /// @return true if the pyramid is empty
    bool empty() const
    { return m_.empty (); }

    private:
    Cont m_;

    private:
    size_type get_levels (size_type rows, size_type cols)
    {
        if (rows == 0 || cols == 0)
            return 0;
        size_type row_levels = next_highest_pow2 (rows);
        size_type col_levels = next_highest_pow2 (cols);
        return (std::min) (row_levels, col_levels) + 1;
    }
};

} // namespace horny_toad

#endif // PYRAMID_H
