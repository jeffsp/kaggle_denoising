/// @file mlr.h
/// @brief multiple linear regression support
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-06-13

#ifndef MLR_H
#define MLR_H

#include "raster_utils.h"
//#include <lapacke.h>

namespace horny_toad
{

    /// @brief helper function
    template<typename T>
    T matrix_multiply (const T &a, const T &b)
    {
        assert (a.cols () == b.rows ());
        const size_t N = a.rows ();
        const size_t M = a.cols ();
        const size_t P = b.cols ();
        T y (a.rows (), b.cols ());
        for (size_t i = 0; i < N; ++i)
            for (size_t j = 0; j < P; ++j)
                for (size_t k = 0; k < M; ++k)
                    y (i, j) += a (i, k) * b (k, j);
        return y;
    }

    /// @brief multiple linear regression
    ///
    /// @tparam T matrix types
    /// @param y responses
    /// @param x predictors
    ///
    /// @return linear estimates of y=b*x
    template<typename T>
    T mlr_inverse (const T &y, const T &x)
    {
        // add a column of 1's to x on the left
        T xx (x.rows (), x.cols () + 1);
        for (size_t i = 0; i < x.rows (); ++i)
        {
            xx (i, 0) = 1;
            for (size_t j = 0; j < x.cols (); ++j)
            {
                xx (i, j + 1) = x (i, j);
            }
        }
        // b = (x^T * x)^-1 * x^T * y
        T xt = transpose (xx);
        T tmp = matrix_multiply (xt, xx);
        tmp = invert (tmp);
        tmp = matrix_multiply (tmp, xt);
        tmp = matrix_multiply (tmp, y);
        return tmp;
    }

    /// @brief multiple linear regression
    ///
    /// @tparam T matrix types
    /// @param y responses
    /// @param x predictors
    ///
    /// @return linear estimates of y=b*x
    /*
    template<typename T>
    T mlr_lapack (const T &y, const T &x)
    {
        assert (x.cols () <= x.rows ());
        assert (y.rows () == x.rows ());
        // result gets stored in b
        T b (y);
        // add a column of 1's to x on the left
        T xx (x.rows (), x.cols () + 1);
        for (size_t i = 0; i < x.rows (); ++i)
        {
            xx (i, 0) = 1;
            for (size_t j = 0; j < x.cols (); ++j)
            {
                xx (i, j + 1) = x (i, j);
            }
        }
        const size_t M = xx.rows ();
        const size_t N = xx.cols ();
        // solve it
        if (LAPACKE_dgels (LAPACK_ROW_MAJOR, 'N', M, N, 1, &xx[0], N, &b[0], 1) != 0)
            throw std::runtime_error ("LAPACK error: can't solve mlr");
        // copy result into matrix with correct dimensions
        T z (xx.cols (), 1);
        for (size_t i = 0; i < z.rows (); ++i)
            z[i] = b[i];
        return z;
    }
    */

    /// @brief multiple linear regression
    ///
    /// @tparam T matrix types
    /// @param y responses
    /// @param x predictors
    ///
    /// @return linear estimates of y=b*x
    template<typename T>
    T mlr (const T &y, const T &x)
    {
        return mlr_inverse (y, x);
        //return mlr_lapack (y, x);
    }
} // namespace horny_toad

#endif // MLR_H
