/// @file mse.h
/// @brief mse
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-17

#ifndef MSE_H
#define MSE_H

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace horny_toad
{

template<typename T>
double sse (const T &a, const T &b)
{
    assert (a.size () == b.size ());
    double sse = 0.0;
    for (size_t i = 0; i < a.size (); ++i)
    {
        double d = double (a[i]) - double (b[i]);
        double d2 = d * d;
        sse += d2;
    }
    return sse;
}

template<typename A,typename B>
double mse (const A &a, const B &b)
{
    return sse (a, b) / a.size ();
}

template<typename T>
double psnr (T mse)
{
    return 10*log10 (255*255/mse);
}

} // namespace horny_toad

#endif // MSE_H
