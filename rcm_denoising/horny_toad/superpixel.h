/// @file superpixel.h
/// @brief superpixel
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef SUPERPIXEL_HPP
#define SUPERPIXEL_HPP

#include <cstddef>

namespace horny_toad
{

/// @brief A superpixel is a big pixel
///
/// A superpixel describes a square pixel region in an image.
///
/// The row and column specify the top left corner of the
/// superpixel in a raster of image pixels.  The size
/// specifies the length in pixels of one side of the
/// superpixel.
///
/// The minimum superpixel size is 1.
class superpixel
{
    public:
    /// @brief Constructor
    superpixel ()
        : row_ (0) , col_ (0) , size_ (0)
    { }
    /// @brief Constructor
    /// @param r superpixel row
    /// @param c superpixel column
    /// @param size superpixel size
    superpixel (size_t r, size_t c, size_t size)
        : row_ (r) , col_ (c) , size_ (size)
    { }
    /// @brief Return the superpixel's row
    size_t get_row () const { return row_; }
    /// @brief Return the superpixel's column
    size_t get_col () const { return col_; }
    /// @brief Return the superpixel's size
    size_t get_size () const { return size_; }
    private:
    size_t row_;
    size_t col_;
    size_t size_;
};

} // namespace horny_toad

#endif // SUPERPIXEL_HPP
