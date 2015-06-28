/// @file opp.h
/// @brief optimal point prediction utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2011-03-23

static_assert(sizeof(long) >= 8, "64-bit code generation not enabled/supported.");

#ifndef OPP_H
#define OPP_H

#include "horny_toad.h"
#include "jack_rabbit.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <time.h>
#include <vector>

namespace opp
{
    /// @brief wavelet functions
    template<typename T, typename U>
    void wavelet_transform (const T &p, U &A, U &B, U &C, U &D)
    {
        U tmpA (p.rows () / 2, p.cols () / 2);
        U tmpB (p.rows () / 2, p.cols () / 2);
        U tmpC (p.rows () / 2, p.cols () / 2);
        U tmpD (p.rows () / 2, p.cols () / 2);
        for (size_t i = 0; i + 1 < p.rows (); i += 2)
        {
            for (size_t j = 0; j + 1 < p.cols (); j += 2)
            {
                const typename U::value_type a = p (i + 0, j + 0);
                const typename U::value_type b = p (i + 0, j + 1);
                const typename U::value_type c = p (i + 1, j + 0);
                const typename U::value_type d = p (i + 1, j + 1);
                tmpA (i / 2, j / 2) = ( a + b + c + d) / 4.0;
                tmpB (i / 2, j / 2) = (-a - b + c + d) / 4.0;
                tmpC (i / 2, j / 2) = ( a - b + c - d) / 4.0;
                tmpD (i / 2, j / 2) = (-a + b + c - d) / 4.0;
            }
        }
        // commit changes
        A = tmpA;
        B = tmpB;
        C = tmpC;
        D = tmpD;
    }

    template<typename T, typename U>
    void inverse_wavelet_transform (const T &A, const T &B, const T &C, const T &D, U &q)
    {
        assert (A.rows () == B.rows () && A.cols () == B.cols ());
        assert (A.rows () == C.rows () && A.cols () == C.cols ());
        assert (A.rows () == D.rows () && A.cols () == D.cols ());
        U tmpq (A.rows () * 2, A.cols () * 2);
        for (size_t i = 0; i < A.rows (); ++i)
        {
            for (size_t j = 0; j < A.cols (); ++j)
            {
                const typename T::value_type AV = A (i, j);
                const typename T::value_type BV = B (i, j);
                const typename T::value_type CV = C (i, j);
                const typename T::value_type DV = D (i, j);
                tmpq (i * 2 + 0, j * 2 + 0) = (AV - BV + CV - DV);
                tmpq (i * 2 + 0, j * 2 + 1) = (AV - BV - CV + DV);
                tmpq (i * 2 + 1, j * 2 + 0) = (AV + BV + CV + DV);
                tmpq (i * 2 + 1, j * 2 + 1) = (AV + BV - CV - DV);
            }
        }
        // commit changes
        q = tmpq;
    }

    const std::vector<double> cdf97_coeffs_lo = { 0.602949018236, 0.266864118443, -0.078223266529, -0.016864118443,  0.026748757411 };
    const std::vector<double> cdf97_coeffs_hi = { 1.11508705, -0.591271763114, -0.057543526229, 0.091271763114, 0.0 };

    template<typename T,bool H>
    T apply (const T &p, const std::vector<double> &c)
    {
        T q (p.rows (), p.cols ());
        for (size_t i = (!H) * (c.size () - 1); i + (!H) * (c.size () - 1) < p.rows (); ++i)
        {
            for (size_t j = (H) * (c.size () - 1); j + (H) * (c.size () - 1) < p.cols (); ++j)
            {
                double sum = p (i, j) * c[0];
                for (size_t k = 1; k < c.size (); ++k)
                    sum += p (i + (!H) * k, j + (H) * k) * c [k] + p (i - (!H) * k, j - (H) * k) * c[k];
                q (i, j) = sum;
            }
        }
        return q;
    }

    /// @brief Cohen-Daubechies-Feauveau wavelet
    template<typename T, typename U>
    void cdf97_transform (const T &p, U &A, U &B, U &C, U &D)
    {
        U tmpA = apply<U,1> (p, cdf97_coeffs_lo); tmpA = apply<U,0> (tmpA, cdf97_coeffs_lo);
        U tmpB = apply<U,1> (p, cdf97_coeffs_lo); tmpB = apply<U,0> (tmpB, cdf97_coeffs_hi);
        U tmpC = apply<U,1> (p, cdf97_coeffs_hi); tmpC = apply<U,0> (tmpC, cdf97_coeffs_lo);
        U tmpD = apply<U,1> (p, cdf97_coeffs_hi); tmpD = apply<U,0> (tmpD, cdf97_coeffs_hi);
        // commit changes
        A = tmpA;
        B = tmpB;
        C = tmpC;
        D = tmpD;
    }

    /// @brief inverse Cohen-Daubechies-Feauveau wavelet
    template<typename T, typename U>
    void inverse_cdf97_transform (const T &A, const T &B, const T &C, const T &D, U &q)
    {
        assert (A.rows () == B.rows () && A.cols () == B.cols ());
        assert (A.rows () == C.rows () && A.cols () == C.cols ());
        assert (A.rows () == D.rows () && A.cols () == D.cols ());
        U tmpA = apply<U,1> (A, cdf97_coeffs_hi); tmpA = apply<U,0> (tmpA, cdf97_coeffs_hi);
        U tmpB = apply<U,1> (B, cdf97_coeffs_hi); tmpB = apply<U,0> (tmpB, cdf97_coeffs_lo);
        U tmpC = apply<U,1> (C, cdf97_coeffs_lo); tmpC = apply<U,0> (tmpC, cdf97_coeffs_hi);
        U tmpD = apply<U,1> (D, cdf97_coeffs_lo); tmpD = apply<U,0> (tmpD, cdf97_coeffs_lo);
        U tmpq (A.rows (), A.cols ());
        for (size_t i = 0; i < A.rows (); ++i)
        {
            for (size_t j = 0; j < A.cols (); ++j)
            {
                const typename T::value_type AV = tmpA (i, j);
                const typename T::value_type BV = tmpB (i, j);
                const typename T::value_type CV = tmpC (i, j);
                const typename T::value_type DV = tmpD (i, j);
                tmpq (i, j) = (AV + BV + CV + DV);// / 4.0;
            }
        }
        // commit changes
        q = tmpq;
    }

    /// @brief helper functions
    ///
    /// Convert to and from opponent colorspace
    template<typename T>
    inline double OPPY (T r, T g, T b) { return r/3.0 + g/3.0 + b/3.0; }
    template<typename T>
    inline double OPPU (T r, T g, T  ) { return r/2.0 - g/2.0; }
    template<typename T>
    inline double OPPV (T r, T g, T b) { return r/4.0 + g/4.0 - b/2.0; }
    template<typename T>
    inline double OPPR (T y, T u, T v) { return y + u + v*2.0/3.0; }
    template<typename T>
    inline double OPPG (T y, T u, T v) { return y - u + v*2.0/3.0; }
    template<typename T>
    inline double OPPB (T y, T  , T v) { return y + 0 - v*4.0/3.0; }

    /// @brief helper function
    ///
    /// convert to opponent colorspace
    template<typename T>
    std::vector<T> rgb_to_opp (const std::vector<T> &rgb)
    {
        assert (rgb.size () == 3);
        // the images should all be the same size
        assert (rgb[0].rows () == rgb[1].rows ());
        assert (rgb[0].rows () == rgb[2].rows ());
        assert (rgb[0].cols () == rgb[1].cols ());
        assert (rgb[0].cols () == rgb[2].cols ());
        std::vector<T> yuv (rgb.size ());
        yuv[0].resize (rgb[0].rows (), rgb[0].cols ());
        yuv[1].resize (rgb[1].rows (), rgb[1].cols ());
        yuv[2].resize (rgb[2].rows (), rgb[2].cols ());
        // convert rgb to opp
        for (size_t i = 0; i < rgb[0].size (); ++i)
        {
            yuv[0][i] = horny_toad::clip (  0 + round (OPPY (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
            yuv[1][i] = horny_toad::clip (128 + round (OPPU (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
            yuv[2][i] = horny_toad::clip (128 + round (OPPV (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
        }
        return yuv;
    }

    /// @brief helper function
    ///
    /// convert from opponent colorspace
    template<typename T>
    std::vector<T> opp_to_rgb (const std::vector<T> &yuv)
    {
        assert (yuv.size () == 3);
        // the images should all be the same size
        assert (yuv[0].rows () == yuv[1].rows ());
        assert (yuv[0].rows () == yuv[2].rows ());
        assert (yuv[0].cols () == yuv[1].cols ());
        assert (yuv[0].cols () == yuv[2].cols ());
        std::vector<T> rgb (yuv.size ());
        rgb[0].resize (yuv[0].rows (), yuv[0].cols ());
        rgb[1].resize (yuv[1].rows (), yuv[1].cols ());
        rgb[2].resize (yuv[2].rows (), yuv[2].cols ());
        // convert opp to rgb
        for (size_t i = 0; i < yuv[0].size (); ++i)
        {
            int y = yuv[0][i];
            int u = yuv[1][i] - 128;
            int v = yuv[2][i] - 128;
            rgb[0][i] = horny_toad::clip (round (OPPR (y, u, v)), 0.0, 255.0);
            rgb[1][i] = horny_toad::clip (round (OPPG (y, u, v)), 0.0, 255.0);
            rgb[2][i] = horny_toad::clip (round (OPPB (y, u, v)), 0.0, 255.0);
        }
        return rgb;
    }

    /// @brief 1D fast block sums in horizontal direction
    ///
    /// compute sums over 1xN blocks of an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    ///
    /// @return image containing block sums
    template<typename T>
    jack_rabbit::raster<size_t> fast_block_sums (const T &p, const size_t N)
    {
        jack_rabbit::raster<size_t> q (p.rows (), p.cols ());
        for (size_t i = 0; i < p.rows (); ++i)
        {
            size_t sum = 0;
            size_t added = 0;
            for (size_t j = 0; j < p.cols (); ++j)
            {
                sum += p (i, j);
                ++added;
                if (added > N)
                {
                    sum -= p (i, j - N);
                    q (i, j - N / 2) = sum;
                }
            }
        }
        return q;
    }

    /// @brief 1D fast block sum of squares in horizontal direction
    ///
    /// compute sum of squares over 1xN blocks of an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    ///
    /// @return image containing block sum of squares
    template<typename T>
    jack_rabbit::raster<size_t> fast_block_sum_squares (const T &p, const size_t N)
    {
        jack_rabbit::raster<size_t> q (p.rows (), p.cols ());
        for (size_t i = 0; i < p.rows (); ++i)
        {
            size_t sum = 0;
            size_t added = 0;
            for (size_t j = 0; j < p.cols (); ++j)
            {
                sum += p (i, j) * p (i, j);
                ++added;
                if (added > N)
                {
                    sum -= p (i, j - N) * p (i, j - N);
                    q (i, j - N / 2) = sum;
                }
            }
        }
        return q;
    }

    /// @brief Fast block average
    ///
    /// compute averages over NxN blocks of an image, clipping the edges
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    ///
    /// @return image containing block averages
    template<typename T>
    T fast_block_average (const T &p, const size_t N)
    {
        // add a mirrored border
        const unsigned BORDER = N / 2 + 1;
        const T m = horny_toad::mborder<jack_rabbit::subregion> (p, BORDER);
        // get sums in 1d
        jack_rabbit::raster<size_t> q = horny_toad::transpose (fast_block_sums (m, N));
        // get sum of sums
        q = horny_toad::transpose (fast_block_sums (q, N));
        T r (q.rows (), q.cols ());
        for (size_t i = 0; i < q.size (); ++i)
            r[i] = round (q[i] / (N * N));
        return horny_toad::crop (r, BORDER);
    }

    /// @brief Fast block standard deviation
    ///
    /// compute standard deviations over NxN blocks of an image, clipping the edges
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    ///
    /// @return image containing block standard deviations
    template<typename T>
    T fast_block_stddev (const T &p, const size_t N)
    {
        const unsigned BORDER = N / 2 + 1;
        const T m = horny_toad::mborder<jack_rabbit::subregion> (p, BORDER);
        const T a = fast_block_average (m, N);
        // get sum2s in 1d
        jack_rabbit::raster<size_t> q = horny_toad::transpose (fast_block_sum_squares (m, N));
        // get sum of sum2s
        q = horny_toad::transpose (fast_block_sums (q, N));
        T r (q.rows (), q.cols ());
        for (size_t i = 0; i < q.size (); ++i)
        {
            double s = q[i] / (N * N) - a[i] * a[i];
            if (s < 0.0)
                r[i] = 0;
            else
                r[i] = round (sqrt (s));
        }
        return horny_toad::crop (r, BORDER);
    }

    /// @brief Block average
    ///
    /// compute averages over NxN blocks of an image, clipping the edges
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    /// @param c include center pixel
    ///
    /// @return image containing block averages
    template<typename T>
    T block_average (const T &p, const size_t N, bool c = true)
    {
        T q (p.rows (), p.cols ());
        for (size_t i = 0; i < p.rows (); ++i)
        {
            for (size_t j = 0; j < p.cols (); ++j)
            {
                size_t total = 0;
                double sum = 0;
                for (size_t i2 = 0; i2 < N; ++i2)
                {
                    size_t i3 = i - N / 2 + i2;
                    if (i3 >= p.rows ())
                        continue;
                    for (size_t j2 = 0; j2 < N; ++j2)
                    {
                        size_t j3 = j - N / 2 + j2;
                        if (j3 >= p.cols ())
                            continue;
                        // if specified, don't include the center
                        if (c == false && i3 == 0 && j3 == 0)
                            continue;
                        ++total;
                        assert (i3 < p.rows ());
                        assert (j3 < p.cols ());
                        sum += p (i3, j3);
                    }
                }
                q (i, j) = round (sum / total);
            }
        }
        return q;
    }

    /// @brief Block standard deviation
    ///
    /// compute standard deviations over NxN blocks of an image, clipping the edges
    ///
    /// @tparam T image type
    /// @param p image
    /// @param N block size
    /// @param c include center pixel
    ///
    /// @return image containing block standard deviations
    template<typename T>
    T block_stddev (const T &p, const size_t N, bool c = true)
    {
        T q (p.rows (), p.cols ());
        for (size_t i = 0; i < q.rows (); ++i)
        {
            for (size_t j = 0; j < q.cols (); ++j)
            {
                size_t total = 0;
                double sum = 0;
                double sum2 = 0;
                for (size_t i2 = 0; i2 < N; ++i2)
                {
                    size_t i3 = i - N / 2 + i2;
                    if (i3 >= p.rows ())
                        continue;
                    for (size_t j2 = 0; j2 < N; ++j2)
                    {
                        size_t j3 = j - N / 2 + j2;
                        if (j3 >= p.cols ())
                            continue;
                        // if specified, don't include the center
                        if (c == false && i3 == 0 && j3 == 0)
                            continue;
                        ++total;
                        assert (i3 < p.rows ());
                        assert (j3 < p.cols ());
                        sum += p (i3, j3);
                        sum2 += p (i3, j3) * p (i3, j3);
                    }
                }
                double avg = sum / total;
                double variance = sum2 / total - avg * avg;
                assert (variance >= 0.0);
                q (i, j) = round (sqrt (variance));
            }
        }
        return q;
    }

    /// @brief divide a container into quantiles
    ///
    /// @param counts container of counts
    /// @param bins number of quantile bins
    ///
    /// @return a map of container bins to quantile bins
    template<typename T>
    std::vector<size_t> quantile (const T &counts, double bins)
    {
        assert (bins > 0);

        const typename T::value_type total = accumulate (counts.begin (), counts.end (), typename T::value_type ());
        // special case for when there are no samples
        if (total == 0)
            return std::vector<size_t> (counts.size (), 0);

        // linearly interpolate to get the cumulative probability of
        // landing in bin 'i'
        std::vector<double> p (counts.size ());
        double c1 = 0.0;
        for (unsigned i = 0; i < p.size (); ++i)
        {
            const double c2 = c1 + counts[i] * 1.0 / total;
            p[i] = (c1 + c2) / 2.0;
            c1 = c2;
        }

        // Create a mapping from a count bin to a quantile bin
        std::vector<size_t> map (counts.size ());
        for (unsigned i = 0; i < map.size (); ++i)
        {
            map[i] = round (p[i] * (bins - 1));
            assert (map[i] < bins);
        }
        return map;
    }

    /// @brief compute variance
    ///
    /// @param total
    /// @param sum
    /// @param sum2
    ///
    /// @return the variance
    double variance (size_t total, double sum, double sum2)
    {
        if (total < 2)
            return std::numeric_limits<float>::max ();
        double m = sum / total;
        double v = sum2 / total - m * m;
        return v;
    }

    /// @brief get optimal linear weight of two predictions
    ///
    /// @param v1 variance of first prediction
    /// @param v2 variance of second prediction
    ///
    /// @return weight of first prediction (subtract from 1.0 to get weight of second prediction)
    double reliability_weight (double v1, double v2)
    {
        if (v1 > std::numeric_limits<float>::max ()
            && v2 > std::numeric_limits<float>::max ())
            return 0.5; // both are unreliable
        if (v1 < std::numeric_limits<float>::epsilon ()
            && v2 < std::numeric_limits<float>::epsilon ())
            return 0.5; // both are reliable
        if (v1 < std::numeric_limits<float>::epsilon ())
            return 1.0; // only first is reliable
        if (v2 < std::numeric_limits<float>::epsilon ())
            return 0.0; // only second is reliable
        const double r1 = 1.0 / v1;
        const double r2 = 1.0 / v2;
        const double w = r1 / (r1 + r2);
        assert (!std::isnan (w));
        assert (!std::isinf (w));
        assert (w >= 0.0);
        assert (w <= 1.0);
        return w;
    }

    /// @brief turn two 8 bit indexes into a 16 bit index
    ///
    /// @param a first 8 bit index
    /// @param b second 8 bit index
    ///
    /// @return 16 bit index
    size_t index88 (unsigned a, unsigned b)
    {
        return (a << 8) + b;
    }

    /// @brief turn three 8 bit indexes into a 15 bit index
    ///
    /// @param a first 8 bit index
    /// @param b second 8 bit index
    /// @param c third 8 bit index
    ///
    /// @return 15 bit index
    size_t index555 (unsigned a, unsigned b, unsigned c)
    {
        return ((a >> 3) << 10) + ((b >> 3) << 5) + (c >> 3);
    }

    /// @brief turn three 8 bit indexes into a 24 bit index
    ///
    /// @param a first 8 bit index
    /// @param b second 8 bit index
    /// @param c third 8 bit index
    ///
    /// @return 24 bit index
    size_t index888 (unsigned a, unsigned b, unsigned c)
    {
        return (a << 16) + (b << 8) + c;
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index6666 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return ((a&0xfc)<<16)|((b&0xfc)<<10)|((c&0xfc)<<4)|(d>>2);
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index4884 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return ((a&0xf0)<<16)|(b<<12)|(c<<4)|(d>>4);
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index8844 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return index4884 (c, a, b, d);
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index8448 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return index4884 (b, a, d, c);
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index8664 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return (a<<16)|((b&0xfc)<<8)|((c&0xfc)<<2)|(d>>4);
    }

    /// @brief turn four 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    ///
    /// @return 24 bit index
    size_t index8754 (unsigned a, unsigned b, unsigned c, unsigned d)
    {
        return (a<<16)|((b&0xfe)<<8)|((c&0xf8)<<1)|(d>>4);
    }

    /// @brief turn five 8 bit indexes into a 25 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    /// @param e 8 bit index
    ///
    /// @return 25 bit index
    size_t index55555 (unsigned a, unsigned b, unsigned c, unsigned d, unsigned e)
    {
        return ((a&0xf8)<<17)|((b&0xf8)<<12)|((c&0xf8)<<7)|((d&0xf8)<<2)|(e>>3);
    }

    /// @brief turn six 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    /// @param e 8 bit index
    /// @param f 8 bit index
    ///
    /// @return 24 bit index
    size_t index444444 (unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f)
    {
        return ((a&0xf0)<<16)|((b&0xf0)<<12)|((c&0xf0)<<8)|((d&0xf0)<<4)|(e&0xf0)|(f>>4);
    }

    /// @brief turn 8, 8 bit indexes into a 24 bit index
    ///
    /// @param a 8 bit index
    /// @param b 8 bit index
    /// @param c 8 bit index
    /// @param d 8 bit index
    /// @param e 8 bit index
    /// @param f 8 bit index
    /// @param g 8 bit index
    /// @param h 8 bit index
    ///
    /// @return 24 bit index
    size_t index33333333 (unsigned a, unsigned b, unsigned c, unsigned d, unsigned e, unsigned f, unsigned g, unsigned h)
    {
        return ((a&0xe0)<<16)|((b&0xe0)<<13)|((c&0xe0)<<10)|((d&0xe0)<<7)|((e&0xe0)<<4)|((f&0xe0)<<1)|((g&0xe0)>>2)|(h>>5);
    }

    /// @brief lookup table
    ///
    /// @tparam T value type of whatever lut is summing
    template<typename T=size_t>
    class lut
    {
        public:
        lut (size_t SZ = 0)
            : totals (SZ)
            , sums (SZ)
            , sums2 (SZ)
        { }
        size_t size () const
        {
            return totals.size ();
        }
        void resize (size_t n)
        {
            totals.resize (n);
            sums.resize (n);
            sums2.resize (n);
        }
        void update (size_t i, T x)
        {
            assert (i < totals.size ());
            assert (i < sums.size ());
            assert (i < sums2.size ());
#pragma omp atomic
            ++totals[i];
#pragma omp atomic
            sums[i] += x;
#pragma omp atomic
            sums2[i] += x * x;
        }
        void update (size_t i, T x, size_t n)
        {
            assert (i < totals.size ());
            assert (i < sums.size ());
            assert (i < sums2.size ());
#pragma omp atomic
            totals[i] += n;
#pragma omp atomic
            sums[i] += x * n;
#pragma omp atomic
            sums2[i] += x * x * n;
        }
        T sum (size_t i) const
        {
            assert (i < sums.size ());
            return sums[i];
        }
        T sum2 (size_t i) const
        {
            assert (i < sums2.size ());
            return sums2[i];
        }
        size_t total (size_t i) const
        {
            assert (i < totals.size ());
            return totals[i];
        }
        std::ostream& write (std::ostream &s) const
        {
            // not portable
            s.write (reinterpret_cast<const char *> (&totals[0]), totals.size () * sizeof (size_t));
            s.write (reinterpret_cast<const char *> (&sums[0]), sums.size () * sizeof (T));
            s.write (reinterpret_cast<const char *> (&sums2[0]), sums2.size () * sizeof (T));
            return s;
        }
        std::istream& read (std::istream &s)
        {
            // not portable
            s.read (reinterpret_cast<char *> (&totals[0]), totals.size () * sizeof (size_t));
            s.read (reinterpret_cast<char *> (&sums[0]), sums.size () * sizeof (T));
            s.read (reinterpret_cast<char *> (&sums2[0]), sums2.size () * sizeof (T));
            return s;
        }
        private:
        std::vector<size_t> totals;
        std::vector<T> sums;
        std::vector<T> sums2;
    };

    /// @brief single moment lookup table
    ///
    /// @tparam T value type of whatever lut is summing
    template<typename T=size_t>
    class lut1
    {
        public:
        lut1 (size_t SZ = 0)
            : totals (SZ)
            , sums (SZ)
        { }
        size_t size () const
        {
            return totals.size ();
        }
        void resize (size_t n)
        {
            totals.resize (n);
            sums.resize (n);
        }
        void update (size_t i, T x)
        {
            assert (i < totals.size ());
            assert (i < sums.size ());
#pragma omp atomic
            ++totals[i];
#pragma omp atomic
            sums[i] += x;
        }
        void update (size_t i, T x, T count)
        {
            assert (i < totals.size ());
            assert (i < sums.size ());
#pragma omp atomic
            totals[i] += count;
#pragma omp atomic
            sums[i] += x * count;
        }
        T sum (size_t i) const
        {
            assert (i < sums.size ());
            return sums[i];
        }
        size_t total (size_t i) const
        {
            assert (i < totals.size ());
            return totals[i];
        }
        std::ostream& write (std::ostream &s) const
        {
            // not portable
            s.write (reinterpret_cast<const char *> (&totals[0]), totals.size () * sizeof (size_t));
            s.write (reinterpret_cast<const char *> (&sums[0]), sums.size () * sizeof (T));
            return s;
        }
        std::istream& read (std::istream &s)
        {
            // not portable
            s.read (reinterpret_cast<char *> (&totals[0]), totals.size () * sizeof (size_t));
            s.read (reinterpret_cast<char *> (&sums[0]), sums.size () * sizeof (T));
            return s;
        }
        private:
        std::vector<size_t> totals;
        std::vector<T> sums;
    };

    /// @brief 4 moment lookup table
    ///
    /// @tparam T value type of whatever lut is summing
    template<typename T=size_t>
    class lut4
    {
        public:
        /// @brief constructor
        ///
        /// @param SZ number of entries
        lut4 (size_t SZ = 0)
            : totals (SZ)
            , sums (SZ)
            , sums2 (SZ)
            , sums3 (SZ)
            , sums4 (SZ)
        { }
        /// @brief get number of entries
        ///
        /// @return the number of entries
        size_t size () const
        {
            return totals.size ();
        }
        void resize (size_t n)
        {
            total.resize (n);
            sums.resize (n);
            sums2.resize (n);
            sums3.resize (n);
            sums4.resize (n);
        }
        /// @brief update the lut
        ///
        /// @param i the table index
        /// @param x the pixel value
        void update (size_t i, size_t x)
        {
            assert (i < size ());
#pragma omp atomic
            ++totals[i];
#pragma omp atomic
            sums[i] += x;
#pragma omp atomic
            sums2[i] += x * x;
#pragma omp atomic
            sums3[i] += x * x * x;
#pragma omp atomic
            sums4[i] += x * x * x * x;
        }
        /// @brief get the sum of first moments
        ///
        /// @param i index
        ///
        /// @return sum of the first moments
        size_t sum (size_t i) const
        {
            assert (i < sums.size ());
            return sums[i];
        }
        /// @brief get the sum of the second moments
        ///
        /// @param i index
        ///
        /// @return sum of the second moments
        size_t sum2 (size_t i) const
        {
            assert (i < sums2.size ());
            return sums2[i];
        }
        /// @brief get the sum of the third moments
        ///
        /// @param i index
        ///
        /// @return sum of the third moments
        size_t sum3 (size_t i) const
        {
            assert (i < sums3.size ());
            return sums3[i];
        }
        /// @brief get the sum of the fourth moments
        ///
        /// @param i index
        ///
        /// @return sum of the fourth moments
        size_t sum4 (size_t i) const
        {
            assert (i < sums4.size ());
            return sums4[i];
        }
        /// @brief get total occurrances with this index
        ///
        /// @param i index
        ///
        /// @return total occurrances of index 'i'
        size_t total (size_t i) const
        {
            assert (i < totals.size ());
            return totals[i];
        }
        /// @brief write to a stream
        ///
        /// @param s the stream
        /// @param l the lut
        ///
        /// @return the stream
        friend std::ostream& operator<< (std::ostream &s, const lut4 &l)
        {
            for (size_t i = 0; i < l.size (); ++i)
            {
                s << " " << l.total (i);
                s << " " << l.sum (i);
                s << " " << l.sum2 (i);
                s << " " << l.sum3 (i);
                s << " " << l.sum4 (i);
                s << std::endl;
            }
            return s;
        }
        private:
        std::vector<size_t> totals;
        std::vector<T> sums;
        std::vector<T> sums2;
        std::vector<T> sums3;
        std::vector<T> sums4;
    };

    /// @brief helper I/O function for lut<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param l lut
    ///
    /// @return istream
    template<typename T>
    std::istream& operator>> (std::istream &s, lut<T> &l)
    {
        return l.read (s);
    }

    /// @brief helper I/O function for lut<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param l lut
    ///
    /// @return ostream
    template<typename T>
    std::ostream& operator<< (std::ostream &s, const lut<T> &l)
    {
        return l.write (s);
    }

    /// @brief helper I/O function for lut1<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param l lut
    ///
    /// @return istream
    template<typename T>
    std::istream& operator>> (std::istream &s, lut1<T> &l)
    {
        return l.read (s);
    }

    /// @brief helper I/O function for lut1<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param l lut
    ///
    /// @return ostream
    template<typename T>
    std::ostream& operator<< (std::ostream &s, const lut1<T> &l)
    {
        return l.write (s);
    }

    /// @brief helper I/O function for vector<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param v vector
    ///
    /// @return istream
    template<typename T>
    std::istream& read (std::istream &s, std::vector<T> &v)
    {
        size_t sz;
        s >> sz;
        v.resize (sz);
        for (size_t i = 0; i < sz; ++i)
            s >> v[i];
        return s;
    }

    /// @brief helper I/O function for vector<T>
    ///
    /// @tparam T
    /// @param s stream
    /// @param v vector
    ///
    /// @return ostream
    template<typename T>
    std::ostream& write (std::ostream &s, const std::vector<T> &v)
    {
        s << v.size () << std::endl;
        for (size_t i = 0; i < v.size (); ++i)
            s << '\t' << v[i];
        s << std::endl;
        return s;
    }

    /// @brief nearest neighbor downsample an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param s scale
    /// @param r row offset
    /// @param c column offset
    ///
    /// @return the downsampled image
    template<typename T>
    T downsample (const T &p, size_t s, size_t r, size_t c)
    {
        T q ((p.rows () - r + 1) / s, (p.cols () - c + 1) / s);
        for (size_t i = r; i < p.rows (); i += s)
        {
            for (size_t j = c; j < p.cols (); j += s)
            {
                size_t ii = (i - r) / s;
                size_t jj = (j - c) / s;
                assert (i < p.rows ());
                assert (j < p.cols ());
                assert (ii < q.rows ());
                assert (jj < q.cols ());
                q (ii, jj) = p (i, j);
            }
        }
        return q;
    }

    /// @brief nearest neighbor downsample an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param s scale
    ///
    /// @return the downsampled image
    template<typename T>
    T downsample (const T &p, size_t s)
    {
        return downsample (p, s, 0, 0);
    }

    /// @brief nearest neighbor upsample an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param s scale
    /// @param q image
    template<typename T>
    void upsample (const T &p, size_t s, T &q)
    {
        for (size_t i = 0; i < p.rows (); ++i)
        {
            for (size_t j = 0; j < p.cols (); ++j)
            {
                assert (i < p.rows ());
                assert (j < p.cols ());
                assert (i * s < q.rows ());
                assert (j * s < q.cols ());
                q (i * s, j * s) = p (i, j);
            }
        }
    }

    /// @brief nearest neighbor upsample an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param s scale
    ///
    /// @return the upsampled image
    template<typename T>
    T upsample (const T &p, size_t s)
    {
        T q (p.rows () * s, p.cols () * s);
        upsample (p, s, q);
        return q;
    }

    /// @brief gaussian blur and rescale an image
    ///
    /// @tparam T image type
    /// @param p image
    /// @param kernel size of kernel in pixels
    /// @param stddev standard deviation of gaussian kernel
    /// @param scale downsampling scale
    ///
    /// @return the blurred and rescaled image
    template<typename T>
    T gaussian_blur (const T &p, size_t kernel, double stddev, size_t scale)
    {
        const size_t ROWS = p.rows () / scale;
        const size_t COLS = p.cols () / scale;
        T q (ROWS, COLS);
        // create gaussian kernel
        jack_rabbit::raster<double> g (kernel, kernel, 1.0);
        jack_rabbit::subscript_unary_function<double,horny_toad::gaussian_window> f (g.rows (), g.cols ());
        f.stddev (stddev);
        std::transform (g.begin (), g.end (), g.begin (), f);
        //horny_toad::print2d (std::clog, g);
        // note that you can't divide by 1/(2*pi*stddev^2) because the tails are
        // clipped, and the kernel therefore won't sum to 1.0
        double sum = accumulate (g.begin (), g.end (), 0.0);
        for (size_t i = 0; i < g.size (); ++i)
            g[i] /= sum;
        // blur and downsample at each point
        for (size_t i = 0; i < ROWS; ++i)
        {
            unsigned i2 = i * scale;
            for (size_t j = 0; j < COLS; ++j)
            {
                unsigned j2 = j * scale;
                assert (i < q.rows ());
                assert (j < q.cols ());
                assert (i2 < p.rows ());
                assert (j2 < p.cols ());
                q (i, j) = horny_toad::mirrored_dot_product (g, p, i2 - (kernel - 1) / 2, j2 - (kernel - 1) / 2);
            }
        }
        return q;
    }

    /// @brief Bilinearly upsample an image that was downsampled with an even kernel
    ///
    /// @tparam T input image type
    /// @tparam U output image type
    /// @param p input image
    /// @param q output image
    template<typename T,typename U>
    void bilinear_upsample_even (const T &p, U &q)
    {
        assert (q.rows () / 2 == p.rows ());
        assert (q.cols () / 2 == p.cols ());
        // do the corners
        if (p.rows () > 0 && p.cols () > 0)
        {
            q (0,             0)             = p (0,             0);
            q (q.rows () - 1, 0)             = p (p.rows () - 1, 0);
            q (0,             q.cols () - 1) = p (0,             p.cols () - 1);
            q (q.rows () - 1, q.cols () - 1) = p (p.rows () - 1, p.cols () - 1);
        }
        // do the edges
        if (p.cols () > 0)
        for (size_t i = 0; i + 1 < p.rows (); ++i)
        {
            unsigned char a = p (i,     0);
            unsigned char b = p (i + 1, 0);
            q (i * 2 + 1, 0) = round (a * 0.75 + b * 0.25);
            q (i * 2 + 2, 0) = round (a * 0.25 + b * 0.75);
            a = p (i,     p.cols () - 1);
            b = p (i + 1, p.cols () - 1);
            q (i * 2 + 1, q.cols () - 1) = round (a * 0.75 + b * 0.25);
            q (i * 2 + 2, q.cols () - 1) = round (a * 0.25 + b * 0.75);
        }
        if (p.rows () > 1)
        for (size_t j = 0; j + 1 < p.cols (); ++j)
        {
            unsigned char a = p (0, j);
            unsigned char b = p (0, j + 1);
            q (0, j * 2 + 1) = round (a * 0.75 + b * 0.25);
            q (0, j * 2 + 2) = round (a * 0.25 + b * 0.75);
            a = p (p.rows () - 1, j);
            b = p (p.rows () - 1, j + 1);
            q (q.rows () - 1, j * 2 + 1) = round (a * 0.75 + b * 0.25);
            q (q.rows () - 1, j * 2 + 2) = round (a * 0.25 + b * 0.75);
        }
        // do the middle
        for (size_t i = 0; i + 1 < p.rows (); ++i)
        {
            for (size_t j = 0; j + 1 < p.cols (); ++j)
            {
                unsigned char a = p (i + 0, j + 0);
                unsigned char b = p (i + 0, j + 1);
                unsigned char c = p (i + 1, j + 0);
                unsigned char d = p (i + 1, j + 1);
                q (i * 2 + 1, j * 2 + 1) = round (a * 0.5625 + b * 0.1875 + c * 0.1875 + d * 0.0625);
                q (i * 2 + 1, j * 2 + 2) = round (a * 0.1875 + b * 0.5625 + c * 0.0625 + d * 0.1875);
                q (i * 2 + 2, j * 2 + 1) = round (a * 0.1875 + b * 0.0625 + c * 0.5625 + d * 0.1875);
                q (i * 2 + 2, j * 2 + 2) = round (a * 0.0625 + b * 0.1875 + c * 0.1875 + d * 0.5625);
            }
        }
    }

    /// @brief Bilinearly upsample an image that was downsampled with an even kernel
    ///
    /// @tparam T image type
    /// @param p input image
    ///
    /// @return upsampled image
    template<typename T>
    T bilinear_upsample_even (const T &p)
    {
        T q (p.rows () * 2, p.cols () * 2);
        bilinear_upsample_even (p, q);
        return q;
    }

    /// @brief Bilinearly upsample an image that was downsampled with an odd kernel
    ///
    /// @tparam T input image type
    /// @tparam U output image type
    /// @param p input image
    /// @param q output image
    ///
    /// The center of the kernel is at 0, 0 in the upsampled image.
    template<typename T,typename U>
    void bilinear_upsample_odd (const T &p, U &q)
    {
        assert ((q.rows () + 1) / 2 == p.rows ());
        assert ((q.cols () + 1) / 2 == p.cols ());
        // fill in uninterpolated pixels
        for (size_t i = 0; i < p.rows (); ++i)
            for (size_t j = 0; j < p.cols (); ++j)
                q (i * 2, j * 2) = p (i, j);
        // do rows 0, 2, 4, ...
        for (size_t i = 0; i < q.rows (); i += 2)
            for (size_t j = 0; j + 2 < q.cols (); j += 2)
                q (i, j + 1) = round ((q (i, j) + q (i, j + 2)) / 2.0);
        // copy last column if needed
        if (!(q.cols () & 1) && q.cols () > 1)
            for (size_t i = 0; i < q.rows (); i += 2)
                q (i, q.cols () - 1) = q (i, q.cols () - 2);
        // do all columns
        for (size_t i = 0; i + 2 < q.rows (); i += 2)
            for (size_t j = 0; j < q.cols (); ++j)
                q (i + 1, j) = round ((q (i, j) + q (i + 2, j)) / 2.0);
        // copy last row if needed
        if (!(q.rows () & 1) && q.rows () > 1)
            for (size_t j = 0; j < q.cols (); ++j)
                q (q.rows () - 1, j) = q (q.rows () - 2, j);
    }

    /// @brief Bilinearly upsample an image that was downsampled with an odd kernel
    ///
    /// @tparam T input image type
    /// @param p input image
    /// @param q output image
    ///
    /// The center of the kernel is at 0, 0 in the upsampled image.
    template<typename T>
    T bilinear_upsample_odd (const T &p)
    {
        T q (2 * p.rows (), 2 * p.cols ());
        bilinear_upsample_odd (p, q);
        return q;
    }

    /// @brief Bilinearly upsample an image that was downsampled by a factor of 4 with an odd kernel
    ///
    /// @tparam T image type
    /// @param p input image
    ///
    /// @return output image
    template<typename T>
    T bilinear_upsample_odd4 (const T &p)
    {
        T q (p.rows () * 4 - 3, p.cols () * 4 - 3);
        // fill in uninterpolated pixels
        for (size_t i = 0; i < p.rows (); ++i)
        {
            for (size_t j = 0; j < p.cols (); ++j)
            {
                assert (i * 4 < q.rows ());
                assert (j * 4 < q.cols ());
                q (i * 4, j * 4) = p (i, j);
            }
        }
        // do rows 0, 4, 8, ...
        for (size_t i = 0; i < q.rows (); i += 4)
        {
            for (size_t j = 0; j + 4 < q.cols (); j += 4)
            {
                assert (i + 0 < q.rows ());
                assert (j + 4 < q.cols ());
                typename T::value_type p1 = q (i, j + 0);
                typename T::value_type p2 = q (i, j + 4);
                q (i, j + 1) = round (p1 * 0.75 + p2 * 0.25);
                q (i, j + 2) = round (p1 * 0.50 + p2 * 0.50);
                q (i, j + 3) = round (p1 * 0.25 + p2 * 0.75);
            }
        }
        // do cols
        for (size_t i = 0; i + 4 < q.rows (); i += 4)
        {
            for (size_t j = 0; j < q.cols (); ++j)
            {
                assert (i + 4 < q.rows ());
                assert (j + 0< q.cols ());
                typename T::value_type p1 = q (i + 0, j);
                typename T::value_type p2 = q (i + 4, j);
                q (i + 1, j) = round (p1 * 0.75 + p2 * 0.25);
                q (i + 2, j) = round (p1 * 0.50 + p2 * 0.50);
                q (i + 3, j) = round (p1 * 0.25 + p2 * 0.75);
            }
        }
        return q;
    }

    /// @brief helper function
    template<typename T,typename U>
    void rgb_to_yuv (const std::vector<T> &rgb, std::vector<U> &yuv)
    {
        assert (rgb.size () == 3);
        yuv.resize (3);
        // the images should all be the same size
        assert (rgb[0].rows () == rgb[1].rows ());
        assert (rgb[0].rows () == rgb[2].rows ());
        assert (rgb[0].cols () == rgb[1].cols ());
        assert (rgb[0].cols () == rgb[2].cols ());
        yuv[0].resize (rgb[0].rows (), rgb[0].cols ());
        yuv[1].resize (rgb[1].rows (), rgb[1].cols ());
        yuv[2].resize (rgb[2].rows (), rgb[2].cols ());
        // convert rgb to YCbCr
        for (size_t i = 0; i < rgb[0].size (); ++i)
        {
            yuv[0][i] = horny_toad::clip (round (horny_toad::YCbCr709Y (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
            yuv[1][i] = horny_toad::clip (128 + round (horny_toad::YCbCr709Cb (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
            yuv[2][i] = horny_toad::clip (128 + round (horny_toad::YCbCr709Cr (rgb[0][i], rgb[1][i], rgb[2][i])), 0.0, 255.0);
        }
    }

    /// @brief helper function
    template<typename T>
    T rgb_to_yuv (const T &rgb)
    {
        T yuv;
        rgb_to_yuv (rgb, yuv);
        return yuv;
    }

    /// @brief helper function
    template<typename T>
    std::vector<T> yuv_to_rgb (const std::vector<T> &yuv)
    {
        if (yuv.size () != 3)
            throw std::runtime_error ("number of yuv planes must equal 3");
        // the images should all be the same size
        assert (yuv[0].rows () == yuv[1].rows ());
        assert (yuv[0].rows () == yuv[2].rows ());
        assert (yuv[0].cols () == yuv[1].cols ());
        assert (yuv[0].cols () == yuv[2].cols ());
        std::vector<T> rgb (yuv.size ());
        rgb[0].resize (yuv[0].rows (), yuv[0].cols ());
        rgb[1].resize (yuv[1].rows (), yuv[1].cols ());
        rgb[2].resize (yuv[2].rows (), yuv[2].cols ());
        // convert YCbCr to rgb
        for (size_t i = 0; i < yuv[0].size (); ++i)
        {
            int y = yuv[0][i];
            int u = yuv[1][i] - 128;
            int v = yuv[2][i] - 128;
            rgb[0][i] = horny_toad::clip (round (horny_toad::YCbCr709R (y, u, v)), 0.0, 255.0);
            rgb[1][i] = horny_toad::clip (round (horny_toad::YCbCr709G (y, u, v)), 0.0, 255.0);
            rgb[2][i] = horny_toad::clip (round (horny_toad::YCbCr709B (y, u, v)), 0.0, 255.0);
        }
        return rgb;
    }

    /// @brief helper function
    ///
    /// Stretch image distribution to use the full range of gray levels.
    template<typename T,typename U>
    void stretch (const T &p, U &q)
    {
        q.resize (p.rows (), p.cols ());
        typename T::value_type a = *min_element (p.begin (), p.end ());
        typename T::value_type b = *max_element (p.begin (), p.end ());
        typename T::value_type r = b - a;
        if (r < 1)
            q = p;
        else
        {
            for (size_t i = 0; i < p.size (); ++i)
            {
                int tmp = round ((p[i] - a) * 255 / r);
                assert (tmp >= 0);
                assert (tmp <= 255);
                q[i] = tmp;
            }
        }
    }

    /// @brief helper function
    ///
    /// Unstretch image distribution to use a smaller range of gray levels.
    template<typename T,typename U>
    T unstretch (const T &p, U a, U b)
    {
        U r = b - a;
        if (r < 1)
            return p;
        T q (p.rows (), p.cols ());
        for (size_t i = 0; i < p.size (); ++i)
        {
            double u = round (p[i] * r / 255.0 + a);
            assert (u >= 0);
            assert (u <= 255);
            q[i] = u;
        }
        return q;
    }

template<typename T>
T get_rgb_plane (const T &p, size_t offset)
{
    assert (offset < 3);
    T r (p.rows (), p.cols () / 3);
    const size_t N = p.size () / 3;
    for (size_t i = 0; i < N; ++i)
        r[i] = p[i * 3 + offset];
    return r;
}

// BT.709 Gamma Encode
template<typename T>
struct gamma_encode : public std::unary_function<T, T>
{
    gamma_encode (double slope = 4.5, double exponent = 0.45)
        : slope (slope)
        , exponent (exponent)
    {
    }
    T operator() (const T &l) const
    {
        if (l <= 0.018)
            return slope * l;
        else
            return 1.099 * pow (l, exponent) - 0.099;
    }
    const double slope;
    const double exponent;
};

// BT.709 Gamma Decode
template<typename T>
struct gamma_decode : public std::unary_function<T, T>
{
    gamma_decode (double slope = 4.5, double exponent = 0.45)
        : slope (slope)
        , exponent (exponent)
    {
    }
    T operator() (const T &e) const
    {
        if (e <= 0.081)
            return e / slope;
        else
            return pow ((e + 0.099) / 1.099, 1 / exponent);
    }
    const double slope;
    const double exponent;
};

jack_rabbit::raster<unsigned char> read_pnm8 (std::istream &ifs, bool &rgb)
{
    if (!ifs)
        throw std::runtime_error ("error reading from file");
    bool lrgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, lrgb, bpp16, w, h);
    if (bpp16)
        throw std::runtime_error ("the file is not 8 bit");
    jack_rabbit::raster<unsigned char> p;
    if (lrgb)
        p.resize (h, w * 3);
    else
        p.resize (h, w);
    horny_toad::read_pnm_pixels (ifs, p);
    rgb = lrgb;
    return p;
}

template<typename T>
T block_downsample (const T &p, const size_t N)
{
    const size_t ROWS = p.rows () / N;
    const size_t COLS = p.cols () / N;
    const size_t N2 = N * N;
    T q (ROWS, COLS);
    for (size_t i = 0; i < ROWS; ++i)
    {
        size_t i2 = i * N;
        for (size_t j = 0; j < COLS; ++j)
        {
            size_t j2 = j * N;
            assert (i < q.rows ());
            assert (j < q.cols ());
            assert (i2 < p.rows ());
            assert (j2 < p.cols ());
            float sum = 0.0;
            for (size_t ii = 0; ii < N; ++ii)
            {
                for (size_t jj = 0; jj < N; ++jj)
                {
                    assert (i2 + ii < p.rows ());
                    assert (j2 + jj < p.cols ());
                    sum += p (i2 + ii, j2 + jj);
                }
            }
            q (i, j) = round (sum / N2);
        }
    }
    return q;
}

template<typename T>
T downsample2x2 (const T &p)
{
    const size_t ROWS = p.rows () / 2;
    const size_t COLS = p.cols () / 2;
    T q (ROWS, COLS);
    for (size_t i = 0; i < ROWS; ++i)
    {
        size_t i2 = i * 2;
        for (size_t j = 0; j < COLS; ++j)
        {
            size_t j2 = j * 2;
            assert (i < q.rows ());
            assert (j < q.cols ());
            assert (i2 < p.rows ());
            assert (j2 < p.cols ());
            q (i, j) = round (
                (p (i2 + 0, j2 + 0) +
                 p (i2 + 1, j2 + 0) +
                 p (i2 + 0, j2 + 1) +
                 p (i2 + 1, j2 + 1))
                    / 4.0);
        }
    }
    return q;
}

template<typename T>
T downsample2x2 (const T &p, const size_t K, const size_t XOFFSET, const size_t YOFFSET)
{
    assert (K > 0);
    const size_t ROW_OFFSET = YOFFSET;
    const size_t COL_OFFSET = XOFFSET;
    assert (ROW_OFFSET < K);
    assert (COL_OFFSET < K);
    const size_t ROWS = p.rows ();
    const size_t COLS = p.cols () / 3;
    T q ((ROWS - ROW_OFFSET) / K, (COLS - COL_OFFSET) / K * 3);
    for (size_t i = ROW_OFFSET; i + K <= ROWS; i += K)
    {
        for (size_t j = COL_OFFSET; j + K <= COLS; j += K)
        {
            size_t rsum = 0;
            size_t gsum = 0;
            size_t bsum = 0;
            double rtotal = 0.0;
            double gtotal = 0.0;
            double btotal = 0.0;
            for (size_t m = 0; m < K; ++m)
            {
                for (size_t n = 0; n < K; ++n)
                {
                    size_t ii = i + m;
                    size_t jj = j + n;
                    rsum += p (ii, jj * 3 + 0);
                    ++rtotal;
                    gsum += p (ii, jj * 3 + 1);
                    ++gtotal;
                    bsum += p (ii, jj * 3 + 2);
                    ++btotal;
                }
            }
            q (i / K, (j / K) * 3 + 0) = round (rsum / rtotal);
            q (i / K, (j / K) * 3 + 1) = round (gsum / gtotal);
            q (i / K, (j / K) * 3 + 2) = round (bsum / btotal);
        }
    }
    return q;
}

size_t nsec (const timespec &t)
{
    return t.tv_nsec + t.tv_sec * size_t (1000000000);
}

}

#endif
