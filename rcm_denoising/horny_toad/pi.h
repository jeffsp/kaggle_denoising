/// @file pi.h
/// @brief pi
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef PI_H
#define PI_H

#include <cmath>

namespace horny_toad
{

/// @brief PI
/// @return 3.141592653589...
static inline double PI () { return 2.0 * asin (1.0); }

} // namespace horny_toad

#endif // PI_H
