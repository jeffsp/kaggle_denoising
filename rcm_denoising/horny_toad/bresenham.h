/// @file bresenham.h
/// @brief line drawing
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-04-26

#ifndef BRESENHAM_H
#define BRESENHAM_H

namespace horny_toad
{
    /// @brief get pixel coordinates of a line drawn for p1 to p2
    ///
    /// @tparam T coordinate type
    /// @param x0 start of line
    /// @param y0 start of line
    /// @param x1 end of line
    /// @param y1 end of line
    /// @param x container of line's x values
    /// @param y container of line's y values
    ///
    /// @return the clipped value
    template<typename T,typename U>
    void line (T x0, T y0, T x1, T y1, U &x, U &y)
    {
        U tx;
        U ty;
        auto dx = abs (x1 - x0);
        auto dy = abs (y1 - y0);
        auto sx = (x0 < x1) ? 1 : -1;
        auto sy = (y0 < y1) ? 1 : -1;
        auto err = dx - dy;
        for (;;) // ever
        {
            tx.push_back (x0);
            ty.push_back (y0);
            if (x0 == x1 && y0 == y1)
                break;
            auto e2 = 2 * err;
            if (e2 > -dy)
            {
                err = err - dy;
                x0 = x0 + sx;
            }
            if (e2 < dx)
            {
                err = err + dx;
                y0 = y0 + sy;
            }
        }
        // commit
        x = tx;
        y = ty;
    }
} // namespace horny_toad

#endif // BRESENHAM_H
