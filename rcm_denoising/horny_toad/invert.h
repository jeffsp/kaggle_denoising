/// @file invert.h
/// @brief invert a matrix
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-06-12

#ifndef INVERT_H
#define INVERT_H

#include <stdexcept>
#include <vector>

namespace horny_toad
{

    /// @brief invert a matrix
    ///
    /// @tparam T matrix type
    /// @param x matrix
    ///
    /// @return inverted matrix
    ///
    /// adapted from NRC
    template<typename T>
    T invert (const T &x)
    {
        if (x.rows () != x.cols ())
            throw std::runtime_error ("this is not a square matrix");
        T y (x);
        const size_t N = y.rows ();
        using std::vector;
        vector<int> c (N);
        vector<int> r (N);
        vector<int> p (N);
        for (size_t i = 0; i < N; ++i)
        {
            double max = 0.0;
            size_t row = 0;
            size_t col = 0;
            for (size_t j = 0; j < N; ++j)
            {
                if (p[j] != 1)
                {
                    for (size_t k = 0; k < N; ++k)
                    {
                        if (p[k] == 0)
                        {
                            if (fabs (y (j, k)) > max)
                            {
                                max = fabs (y (j, k));
                                row = j;
                                col = k;
                            }
                        }
                        else if (p[k] > 1)
                        {
                            throw std::runtime_error ("gaussj error: singular matrix");
                        }
                    }
                }
            }
            ++p[col];
            if (row != col)
            {
                for (size_t l = 0; l < N; ++l)
                    std::swap (y (row, l), y (col, l));
            }
            r[i] = row;
            c[i] = col;
            if (y (col, col) == 0.0)
                throw std::runtime_error ("gaussj error: singular matrix");
            double pivinv = 1.0 / y (col, col);
            y (col, col) = 1.0;
            for (size_t l = 0; l < N; ++l)
                y (col, l) *= pivinv;
            for (size_t m = 0; m < N; ++m)
            {
                if (m != col)
                {
                    double tmp = y (m, col);
                    y (m, col) = 0.0;
                    for (size_t l = 0; l < N; ++l)
                        y (m, l) -= y (col, l) * tmp;
                }
            }
        }
        for (size_t l = N - 1; l < N; --l)
            if (r[l] != c[l])
                for (size_t k = 0; k < N; ++k)
                    std::swap (y (k, r[l]), y (k, c[l]));
        return y;
    }

} // namespace horny_toad

#endif // INVERT_H
