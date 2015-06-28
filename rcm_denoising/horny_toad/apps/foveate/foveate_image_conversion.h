// Convert Image Formats
//
// Copyright (C) 2008
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Mar 25 15:33:05 CDT 2008

#ifndef IMAGE_CONVERSION_H
#define IMAGE_CONVERSION_H

#include "foveator.h"
#include <QImage>

namespace horny_toad
{

template<typename Ty>
inline Ty Clip (Ty v, Ty min, Ty max)
{
    if (v < min)
        return min;
    else if (v > max)
        return max;
    else
        return v;
}

template<typename Ty>
inline Ty YCbCr709Y  (Ty r, Ty g, Ty b) { return   static_cast<Ty> (0.2215 * r + 0.7154 * g + 0.0721 * b); }
template<typename Ty>
inline Ty YCbCr709Cb (Ty r, Ty g, Ty b) { return   static_cast<Ty> (-0.1145 * r - 0.3855 * g + 0.5000 * b); }
template<typename Ty>
inline Ty YCbCr709Cr (Ty r, Ty g, Ty b) { return   static_cast<Ty> (0.5016 * r - 0.4556 * g - 0.0459 * b); }
template<typename Ty>
inline Ty YCbCr709R  (Ty y, Ty cb, Ty cr) { return static_cast<Ty> (y + 0.0000 * cb + 1.5701 * cr); }
template<typename Ty>
inline Ty YCbCr709G  (Ty y, Ty cb, Ty cr) { return static_cast<Ty> (y - 0.1870 * cb - 0.4664 * cr); }
template<typename Ty>
inline Ty YCbCr709B  (Ty y, Ty cb, Ty cr) { return static_cast<Ty> (y + 1.8556 * cb + 0.0000 * cr); }

// Load from a QImage
template<class T>
inline void load_yuv (
    const QImage &image,
    T &m1,
    T &m2,
    T &m3)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    const unsigned char *p = image.bits ();
    assert (m1.size () == W * H);
    assert (m2.size () == W * H);
    assert (m3.size () == W * H);
    for (size_t y = 0; y < H; ++y)
    {
        size_t yindex = y * W * 4;
        for (size_t x = 0; x < W; ++x)
        {
            size_t index = yindex + x * 4;
            int r = p[index + 0];
            int g = p[index + 1];
            int b = p[index + 2];
            m1 (y, x) = static_cast<unsigned char> (Clip (YCbCr709Y (r, g, b), 0, 255));
            m2 (y, x) = static_cast<unsigned char> (Clip (YCbCr709Cb (r, g, b) + 128, 0, 255));
            m3 (y, x) = static_cast<unsigned char> (Clip (YCbCr709Cr (r, g, b) + 128, 0, 255));
        }
    }
}

// Load from a QImage
template<class T>
inline void load_rgb (
    const QImage &image,
    T &m1,
    T &m2,
    T &m3)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    const unsigned char *p = image.bits ();
    assert (m1.size () == W * H);
    assert (m2.size () == W * H);
    assert (m3.size () == W * H);
    for (size_t y = 0; y < H; ++y)
    {
        size_t yindex = y * W * 4;
        for (size_t x = 0; x < W; ++x)
        {
            size_t index = yindex + x * 4;
            m1 (y, x) = p[index + 0];
            m2 (y, x) = p[index + 1];
            m3 (y, x) = p[index + 2];
        }
    }
}

// Load from a QImage
template<class T>
inline void load_grayscale (
    const QImage &image,
    T &m)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    assert (m.size () == W * H);
    copy (image.bits (), image.bits () + W * H, m.begin ());
}

// Unload from raster to a QImage.
template<class T>
inline void unload_grayscale (
    const T &m,
    QImage &image)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    Q_UNUSED (W);
    Q_UNUSED (H);
    assert (m.size () == W * H);
    copy (m.begin (), m.end (), image.bits ());
}

// Unload from matrices to a QImage.
template<class T>
inline void unload_rgb (
    const T &m1,
    const T &m2,
    const T &m3,
    QImage &image)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    unsigned char *p = image.bits ();
    for (size_t y = 0; y < H; ++y)
    {
        size_t yindex = y * W * 4;
        for (size_t x = 0; x < W; ++x)
        {
            size_t index = yindex + x * 4;
            p[index + 0] = m1 (y, x);
            p[index + 1] = m2 (y, x);
            p[index + 2] = m3 (y, x);
        }
    }
}

// Unload from matrices to a QImage.
template<class T>
inline void unload_yuv (
    const T &m1,
    const T &m2,
    const T &m3,
    QImage &image)
{
    const size_t W = image.width ();
    const size_t H = image.height ();
    unsigned char *p = image.bits ();
    for (size_t y = 0; y < H; ++y)
    {
        size_t yindex = y * W * 4;
        for (size_t x = 0; x < W; ++x)
        {
            size_t index = yindex + x * 4;
            int ty = m1 (y, x);
            int tu = m2 (y, x) - 128;
            int tv = m3 (y, x) - 128;
            p[index + 0] = static_cast<unsigned char> (Clip (YCbCr709R (ty, tu, tv), 0, 255));
            p[index + 1] = static_cast<unsigned char> (Clip (YCbCr709G (ty, tu, tv), 0, 255));
            p[index + 2] = static_cast<unsigned char> (Clip (YCbCr709B (ty, tu, tv), 0, 255));
        }
    }
}

} // namespace horny_toad

#endif // IMAGE_CONVERSION_H
