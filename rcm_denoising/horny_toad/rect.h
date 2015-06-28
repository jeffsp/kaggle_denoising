/// @file rect.h
/// @brief rect functions
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef RECT_HPP
#define RECT_HPP

#include <cassert>

namespace horny_toad
{

/// @brief A rectangular region
template<typename T>
class rect
{
    public:
    /// @brief Constructor
    /// @param x x coordinate
    /// @param y y coordinate
    /// @param width width
    /// @param height height
    ///
    /// The width and height must be non-zero.
    rect (T x = 0, T y = 0, T width = 0, T height = 0)
        : x_ (x) , y_ (y) , width_ (width) , height_ (height)
    {
        //assert (check ());
    }
    /// @brief Copy constructor
    /// @param r rect to copy
    rect (const rect<T> &r)
        : x_ (r.x_) , y_ (r.y_) , width_ (r.width_) , height_ (r.height_)
    {
        //assert (check ());
    }
    /// @brief Check invariants
    //bool check ()
    //{
    //    return (width_ >= 0) && (height_ >= 0);
    //}
    /// @brief Clip a rect
    /// @param r the bounding rect
    void clip (const rect<T> &r)
    {
        clip (x_, width_, r.x_, r.width_);
        clip (y_, height_, r.y_, r.height_);
        //assert (check ());
    }
    /// @brief Get the rect's x coordinate
    T get_x () const { return x_; }
    /// @brief Get the rect's y coordinate
    T get_y () const { return y_; }
    /// @brief Get the rect's width
    T get_width () const { return width_; }
    /// @brief Get the rect's height
    T get_height () const { return height_; }
    /// @brief Set the rect's x coordinate
    /// @param x x
    void set_x (T x) { x_ = x; }
    /// @brief Set the rect's y coordinate
    /// @param y y
    void set_y (T y) { y_ = y; }
    /// @brief Set the rect's width
    /// @param w width
    void set_width (T w) { width_ = w; }
    /// @brief Set the rect's height
    /// @param h height
    void set_height (T h) { height_ = h; }
    /// @brief Determine if the rect has zero area
    bool empty () const { return width_ == 0 || height_ == 0; }
    private:
    /// @brief Clip helper function
    void clip (T &p, T &len, T clip_p, T clip_len)
    {
        T p1 = p;
        T p2 = p1 + len;
        T clip_p1 = clip_p;
        T clip_p2 = clip_p1 + clip_len;
        if (p1 < clip_p1) p1 = clip_p1;
        if (p2 > clip_p2) p2 = clip_p2;
        p = p1;
        len = p2 - p1;
        if (len < 0) len = 0;
    }
    // rect properties
    T x_;
    T y_;
    T width_;
    T height_;
};

} // namespace horny_toad

#endif // RECT_HPP
