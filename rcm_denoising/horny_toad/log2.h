/// @file log2.h
/// @brief log base 2
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef LOG2_H
#define LOG2_H

#include <cmath>

namespace horny_toad
{

/// @brief Log base 2
/// @return log base 2 of x
template<typename T>
inline T log2 (T x)
{
    const T LOG10_2 = log10 (2.0);
    return log10 (x) / LOG10_2;
}

} // namespace horny_toad

#endif // LOG2_H
