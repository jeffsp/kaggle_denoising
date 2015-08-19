/// @file subregion.h
/// @brief rectangular raster subregion
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef SUBREGION_HPP
#define SUBREGION_HPP

#include <cstddef>

namespace jack_rabbit
{

/// @brief Rectangular subregion
struct subregion
{
    /// @brief Subscript of starting element
    size_t r;
    /// @brief Subscript of starting element
    size_t c;
    /// @brief Dimensions of the subregion
    size_t rows;
    /// @brief Dimensions of the subregion
    size_t cols;
};

} // namespace jack_rabbit

#endif // SUBREGION_HPP
