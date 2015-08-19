/// @file about_equal.h
/// @brief about equal
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef ABOUT_EQUAL_H
#define ABOUT_EQUAL_H

#include <cmath>

namespace horny_toad
{

/// @brief Return true if two values are about equal
template<typename Ty>
inline bool about_equal (Ty a, Ty b, double PREC = 0.001)
{
    return round (a * 1.0 / PREC) == round (b * 1.0 / PREC);
}

} // namespace horny_toad

#endif // ABOUT_EQUAL_H
