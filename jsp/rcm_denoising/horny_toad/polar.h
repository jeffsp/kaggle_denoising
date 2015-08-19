/// @file polar.h
/// @brief polar coords
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef POLAR_H
#define POLAR_H

#include <cassert>
#include <cmath>
#include <cstdlib>

namespace horny_toad
{

template<typename Ty>
inline void cart2pol (Ty x, Ty y, Ty &r, Ty &th)
{
    th = atan2 (y, x);
    r = sqrt (x * x + y * y);
}

template<typename Ty>
inline void pol2cart (Ty r, Ty th, Ty &x, Ty &y)
{
    x = r * cos (th);
    y = r * sin (th);
}

/// @brief convert from hyperspherical to cartesian coordinates
///
/// @tparam T floating point type
/// @tparam U container type
/// @param r radius of the hypersphere
/// @param phi container of N-1 angular coordinates of an N-sphere
/// @param x return vector of N cartesian coordinates
template<typename T,typename U>
inline void hyper2cart (T r, const U &phi, U &x)
{
    assert (phi.size () > 0);
    // guarantee no side-effects on exceptions
    U tmp (phi.size () + 1);
    for (size_t i = 0; i < tmp.size (); ++i)
    {
        T prod = r;
        for (size_t j = 0; j <= i && j < phi.size (); ++j)
            if (j == i)
                prod *= cos (phi[j]);
            else
                prod *= sin (phi[j]);
        tmp[i] = prod;
    }
    x = tmp;
}

} // namespace horny_toad

#endif // POLAR_H
