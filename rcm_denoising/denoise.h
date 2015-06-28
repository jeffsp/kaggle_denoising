/// @file denoise.h
/// @brief denoise module
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2011-10-31

#ifndef DENOISE_H
#define DENOISE_H

#include "opp.h"

namespace denoise
{

// const size_t PASSES = 3;
const size_t PASSES = 6;

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
