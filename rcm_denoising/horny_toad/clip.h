/// @file clip.h
/// @brief clip
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef CLIP_H
#define CLIP_H

#include <functional>

namespace horny_toad
{

    /// @brief clip a value
    ///
    /// @tparam T type to clip
    /// @param x value to clip
    /// @param min min value
    /// @param max max value
    ///
    /// @return the clipped value
    template<typename T>
    T clip (T x, T min, T max)
    {
        return x < min ? min : (x > max) ? max : x;
    }

    /// @brief functor version of clip
    ///
    /// @tparam T type of value to clip
    template<typename T>
    struct clip_functor : std::unary_function <T,T>
    {
        clip_functor (const T &min, const T &max)
            : min (min)
            , max (max)
        { }
        T operator() (const T &x) const
        { return clip (x, min, max); }
        private:
        const T min;
        const T max;
    };

} // namespace horny_toad

#endif // CLIP_H
