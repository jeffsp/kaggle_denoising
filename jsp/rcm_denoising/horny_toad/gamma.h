/// @file gamma.h
/// @brief gamma
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-17

#ifndef GAMMA_H
#define GAMMA_H

#include "clip.h"
#include <cmath>
#include <cstdlib>
#include <vector>

namespace horny_toad
{

std::vector<unsigned short> sRGB_gamma_curve ()
{
    std::vector<unsigned short> lut (65536);
    // see http://en.wikipedia.org/wiki/SRGB
    const double a = 0.055;
    const double m = 12.92;
    const double l = 0.0031308;
    for (size_t i = 0; i < lut.size (); ++i)
    {
        const double c = i / 65535.0;
        if (c <= l)
            lut[i] = round (clip (m * c, 0.0, 1.0) * 65535.0);
        else
            lut[i] = round (clip ((1 + a) * pow (c, 1.0 / 2.4) - a, 0.0, 1.0) * 65535.0);
    }
    return lut;
}

} // namespace horny_toad

#endif // GAMMA_H
