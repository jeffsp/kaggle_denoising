/// @file bicubic.h
/// @brief bicubic interp
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14
///
/// This came straight out of http://en.wikipedia.org/wiki/Bicubic_interpolation

#ifndef BICUBIC_H
#define BICUBIC_H

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace horny_toad
{


/// @brief Use finite differences to determine df/dx of f(x,y)
///
/// @tparam R return type
/// @tparam T type of f(x,y)
/// @param p f(x,y)
///
/// @return derivatives evaluated at each point of f(x,y)
template<typename R,typename T>
R fxy_dx (const T &p)
{
    R df (p.rows (), p.cols ());
    for (size_t i = 0; i + 2 < p.rows (); ++i)
        for (size_t j = 0; j + 2 < p.cols (); ++j)
            df (i + 1, j + 1) = (static_cast<typename R::value_type> (p (i + 1, j + 2))
                - static_cast<typename R::value_type> (p (i + 1, j + 0))) / 2.0;
    return df;
}

/// @brief Use finite differences to determine df/dy of f(x,y)
///
/// @tparam R return type
/// @tparam T type of f(x,y)
/// @param p f(x,y)
///
/// @return derivatives evaluated at each point of f(x,y)
template<typename R,typename T>
R fxy_dy (const T &p)
{
    R df (p.rows (), p.cols ());
    for (size_t i = 0; i + 2 < p.rows (); ++i)
        for (size_t j = 0; j + 2 < p.cols (); ++j)
            df (i + 1, j + 1) = (static_cast<typename R::value_type> (p (i + 2, j + 1))
                - static_cast<typename R::value_type> (p (i + 0, j + 1))) / 2.0;
    return df;
}

/// @brief Use finite differences to determine df/dxy of f(x,y)
///
/// @tparam R return type
/// @tparam T type of f(x,y)
/// @param p f(x,y)
///
/// @return cross derivatives evaluated at each point of f(x,y)
template<typename R,typename T>
R fxy_dxy (const T &p)
{
    R df (p.rows (), p.cols ());
    for (size_t i = 0; i + 2 < p.rows (); ++i)
        for (size_t j = 0; j + 2 < p.cols (); ++j)
            df (i + 1, j + 1) = (static_cast<typename R::value_type> (p (i + 2, j + 2))
                - static_cast<typename R::value_type> (p (i + 2, j + 0))
                - static_cast<typename R::value_type> (p (i + 0, j + 2))
                + static_cast<typename R::value_type> (p (i + 0, j + 0))) / 4.0;
    return df;
}

/// @brief p(x,y)=sum(i=0,3)[sum(j=0,3)[c(i,j)*x^i*y^j]]
///
/// @tparam T coordinate type
/// @tparam U coefficient type
/// @param x x coordinate
/// @param y y coordinate
/// @param c coefficients
///
/// @return surface value evaluated at x, y
template<typename T,typename U>
typename U::value_type surface (T x, T y, const U &c)
{
    assert (c.size () == 16);

    typename U::value_type sum = 0.0;
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            sum += c[j * 4 + i] * std::pow (x, i) * std::pow (y, j);
    return sum;
}

/// @brief solve for c in x=Ac
///
/// @tparam T known function values type
/// @param x known function values
///
/// @return the coefficients
template<typename T>
T get_coeffs (const T &x)
{
    const int A_inverse[256] = {
         1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        -3,  3,  0,  0, -2, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         2, -2,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0, -3,  3,  0,  0, -2, -1,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  1,  1,  0,  0,
        -3,  0,  3,  0,  0,  0,  0,  0, -2,  0, -1,  0,  0,  0,  0,  0,
         0,  0,  0,  0, -3,  0,  3,  0,  0,  0,  0,  0, -2,  0, -1,  0,
         9, -9, -9,  9,  6,  3, -6, -3,  6, -6,  3, -3,  4,  2,  2,  1,
        -6,  6,  6, -6, -3, -3,  3,  3, -4,  4, -2,  2, -2, -2, -1, -1,
         2,  0, -2,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  2,  0, -2,  0,  0,  0,  0,  0,  1,  0,  1,  0,
        -6,  6,  6, -6, -4, -2,  4,  2, -3,  3, -3,  3, -2, -1, -2, -1,
         4, -4, -4,  4,  2,  2, -2, -2,  2, -2,  2, -2,  1,  1,  1,  1
    };
    assert (x.size () == 16);
    T c (x.size ());
    for (size_t i = 0; i < 16; ++i)
    {
        for (size_t j = 0; j < 16; ++j)
        {
            size_t index = i * 16 + j;
            assert (index < 256);
            c[i] += A_inverse[index] * x[j];
        }
    }
    return c;
}

/// @brief bicubic interpolation at a single point
///
/// @tparam T image type
/// @tparam U derivative type
/// @param p image
/// @param dx df/dx
/// @param dy df/dy
/// @param dxy df/dxdy
/// @param x point to interpolate
/// @param y point to interpolate
///
/// @return interpolated image point value
template<typename T,typename U>
typename U::value_type bicubic_interp (const T &p, const U &dx, const U &dy, const U &dxy, double x, double y)
{
    assert (p.rows () == dx.rows ());
    assert (p.rows () == dy.rows ());
    assert (p.rows () == dxy.rows ());
    assert (p.cols () == dx.cols ());
    assert (p.cols () == dy.cols ());
    assert (p.cols () == dxy.cols ());
    // get the boundary indexes of p
    const unsigned i1 = y;
    const unsigned j1 = x;
    const unsigned i2 = i1 + 1;
    const unsigned j2 = j1 + 1;
    // if we are too near the image's edges, we can't interpolate
    if (i1 < 1)
        return 0;
    if (j1 < 1)
        return 0;
    if (i2 + 2 > p.rows ())
        return 0;
    if (j2 + 2 > p.cols ())
        return 0;
    // get known function values of the linear equation: pixel values and three
    // derivatives at all four corners
    std::vector<typename U::value_type> d (16);
    d[0] = p (i1, j1);
    d[1] = p (i1, j2);
    d[2] = p (i2, j1);
    d[3] = p (i2, j2);
    d[4] = dx (i1, j1);
    d[5] = dx (i1, j2);
    d[6] = dx (i2, j1);
    d[7] = dx (i2, j2);
    d[8] = dy (i1, j1);
    d[9] = dy (i1, j2);
    d[10] = dy (i2, j1);
    d[11] = dy (i2, j2);
    d[12] = dxy (i1, j1);
    d[13] = dxy (i1, j2);
    d[14] = dxy (i2, j1);
    d[15] = dxy (i2, j2);
    // solve for the coefficients
    std::vector<typename U::value_type> c = get_coeffs (d);
    // evaluate at the point
    typename U::value_type v = surface (fmod (x, 1.0), fmod (y, 1.0), c);
    return v;
}

/// @brief interpolate the points in one image given another image
///
/// @tparam T type of input image
/// @tparam U type of output image
/// @param p input image
/// @param q output image
template<typename T,typename U>
void bicubic_interp (const T &p, U &q)
{
    // compute derivatives at each point of p
    U dx = fxy_dx<U> (p);
    U dy = fxy_dy<U> (p);
    U dxy = fxy_dxy<U> (p);
    const double xscale = static_cast<double> (p.cols ()) / q.cols ();
    const double yscale = static_cast<double> (p.rows ()) / q.rows ();
    // for each point in q
#pragma omp parallel for
    for (size_t i = 0; i < q.rows (); ++i)
    {
        for (size_t j = 0; j < q.cols (); ++j)
        {
            // where does the center of the pixel align with the centers of pixels in p?
            double x = (j + 0.5) * xscale - 0.5;
            double y = (i + 0.5) * yscale - 0.5;
            // interpolate the point
            q (i, j) = bicubic_interp (p, dx, dy, dxy, x, y);
        }
    }
}

/// @brief interpolate values at specified coordinates given an image
///
/// @tparam T type of input image
/// @tparam U coordinate type
/// @param p input image
/// @param x coordinates
/// @param y coordinates
/// @param q interpolated points
template<typename T,typename U>
void bicubic_interp (const T &p, const U &x, const U &y, U &q)
{
    assert (x.size () == y.size ());
    assert (x.size () == q.size ());
    // compute derivatives at each point of p
    U dx = fxy_dx<U> (p);
    U dy = fxy_dy<U> (p);
    U dxy = fxy_dxy<U> (p);
    // for each point
#pragma omp parallel for
    for (size_t i = 0; i < x.size (); ++i)
        q[i] = bicubic_interp (p, dx, dy, dxy, x[i], y[i]);
}

} // namespace horny_toad

#endif // BICUBIC_H
