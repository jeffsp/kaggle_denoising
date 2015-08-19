/// @file subregion_algo.h
/// @brief subregion algorithms
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef SUBREGION_ALGO_HPP
#define SUBREGION_ALGO_HPP

#include "subregion_iter.h"
#include <cstring>

namespace jack_rabbit
{

/// @brief Overload for fast subregion copies
///
/// This implementation of copy may be faster than std::copy.
template<class T>
inline subregion_iter<T>
copy (const subregion_iter<T> &src1,
    const subregion_iter<T> &src2,
    subregion_iter<T> dest)
{
    // Copy one row at a time
    for (subregion_iter<T> current_src = src1;
        current_src != src2;
        current_src.next_row ())
    {
        typename subregion_iter<T>::pointer dest_p = &*dest.row_begin ();
        typename subregion_iter<T>::pointer src_p = &*current_src.row_begin ();
        std::memmove (dest_p,
            src_p,
            sizeof (typename subregion_iter<T>::value_type)
            * (current_src.row_end () - current_src.row_begin ()));
        dest.next_row ();
    }
    return src2;
}

} // namespace jack_rabbit

#endif // SUBREGION_ALGO_HPP
