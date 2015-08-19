/// @file denoise.h
/// @brief denoise module
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2011-10-31

#ifndef DENOISE_H
#define DENOISE_H

#include "horny_toad.h"
#include "jack_rabbit.h"
#include <fstream>

namespace opp
{
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

    size_t index888 (unsigned a, unsigned b, unsigned c)
    {
        return (a << 16) + (b << 8) + c;
    }

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
};

namespace denoise
{

const size_t PASSES = 3;

typedef jack_rabbit::raster<unsigned char> image_t;
typedef std::vector<image_t> images_t;

/// @brief average two images together
///
/// @tparam T image type
/// @param a first image
/// @param b second image
///
/// @return average of two images
template<typename T>
T avg (const T &a, const T &b)
{
    assert (a.rows () == b.rows ());
    assert (a.cols () == b.cols ());
    T c (a.rows (), a.cols ());
    for (size_t i = 0; i < a.size (); ++i)
        c[i] = round ((a[i] + b[i]) / 2.0);
    return c;
}

class context
{
    public:
    static size_t indexh (const image_t &p, size_t i, size_t j)
    {
        assert (i >= 0);
        assert (j >= 1);
        assert (i < p.rows ());
        assert (j + 1 < p.cols ());
        return opp::index888 (p (i, j - 1), p (i, j), p (i, j + 1));
    }
    static size_t indexv (const image_t &p, size_t i, size_t j)
    {
        assert (i >= 1);
        assert (j >= 0);
        assert (i + 1 < p.rows ());
        assert (j < p.cols ());
        return opp::index888 (p (i - 1, j), p (i, j), p (i + 1, j));
    }
    static size_t kernel_size ()
    {
        return 3;
    }
    static opp::lut1<size_t> default_lut (size_t count)
    {
        opp::lut1<size_t> l (1 << 24);
        for (size_t a = 0; a < 256; ++a)
        for (size_t b = 0; b < 256; ++b)
        for (size_t c = 0; c < 256; ++c)
            l.update (opp::index888 (a, b, c), b, count);
        return l;
    }
};

template<typename T>
const T rescale (const T &p, const double scale)
{
    T q (p);
    for (size_t i = 0; i < p.rows (); ++i)
    {
        for (size_t j = 0; j < p.cols (); ++j)
        {
            const int p0 = p (i, j);
            int scaled = round (p0 * scale);
            if (scaled > 255)
                scaled = 255;
            q (i, j) = scaled;
        }
    }
    return q;
}

template<typename C>
class codec
{
    private:
    opp::lut1<size_t> l;
    public:
    codec ()
        : l (C::default_lut (1))
    {
    }
    void update (const image_t &p, const image_t &q, const bool h)
    {
        update2 (p, q, h);
        if (h)
            update2 (horny_toad::fliplr (p), horny_toad::fliplr (q), h);
        else
            update2 (horny_toad::flipud (p), horny_toad::flipud (q), h);
    }
    void update2 (const image_t &p, const image_t &q, const bool h)
    {
        const size_t K = C::kernel_size ();
        if (h)
        {
            for (size_t i = K; i + K < p.rows (); ++i)
                for (size_t j = K; j + K < p.cols (); ++j)
                    l.update (C::indexh (q, i, j), p (i, j));
        }
        else
        {
            for (size_t i = K; i + K < p.rows (); ++i)
                for (size_t j = K; j + K < p.cols (); ++j)
                    l.update (C::indexv (q, i, j), p (i, j));
        }
    }
    image_t denoise (const image_t &p, const bool h) const
    {
        image_t q (p.rows (), p.cols ());
        const size_t K = C::kernel_size() / 2; // offset to center
        for (size_t i = K; i + K < p.rows (); ++i)
        {
            for (size_t j = K; j + K < p.cols (); ++j)
            {
                const size_t n = h ? C::indexh (p, i, j) : C::indexv (p, i, j);
                // luts should have been preloaded
                assert (l.total (n) != 0);
                const double x = static_cast<double> (l.sum (n)) / l.total (n);
                assert (x >= 0.0);
                assert (x <= 255.0);
                q (i, j) = round (x);
            }
        }
        return q;
    }
    private:
    friend std::ostream& operator<< (std::ostream &s, const codec &c)
    {
        s << c.l;
        return s;
    }
    friend std::istream& operator>> (std::istream &s, codec &c)
    {
        s >> c.l;
        return s;
    }
};

template<size_t N>
class multi_codec
{
    private:
    codec<context> c[N];
    public:
    size_t lut_passes () const { return N; }
    void update (const image_t &p, const image_t &q, size_t pass)
    {
        image_t t (q);
        // restore q up to this pass
        for (size_t n = 0; n < pass; ++n)
            t = c[n].denoise (t, !(n & 1));
        // update using restored image
        c[pass].update (p, t, !(pass & 1));
    }
    image_t denoise (const image_t &q) const
    {
        image_t p (q);
        for (size_t n = 0; n < N; ++n)
            p = c[n].denoise (p, !(n & 1));
        return p;
    }
    private:
    friend std::ostream& operator<< (std::ostream &s, const multi_codec &c)
    {
        for (auto i : c.c)
            s << i;
        return s;
    }
    friend std::istream& operator>> (std::istream &s, multi_codec &c)
    {
        for (auto &i : c.c)
            s >> i;
        return s;
    }
};

}

#endif
