/// @file pnm.h
/// @brief pnm utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef PNM_H
#define PNM_H

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace horny_toad
{

// PNM file writer helper
void write_pnm_header (std::ostream &os, size_t w, size_t h, bool rgb = false, bool bpp16 = false)
{
    // Write a pnm header
    os << (rgb ? "P6" : "P5") << "\n";
    //os << "# jsp\n";
    os << w << ' ' << h << '\n';
    if (!bpp16)
    {
        // 8 bits per pixel
        os << "255\n";
    }
    else
    {
        // 16 bits per pixel
        os << "65535\n";
    }
}

// PNM file writer helper
template<class T>
void write_pnm (std::ostream &os, size_t w, size_t h, const T &m, bool rgb = false, bool bpp16 = false)
{
    write_pnm_header (os, w, h, rgb, bpp16);

    if (!bpp16)
    {
        os.write (reinterpret_cast<const char *> (&m[0]), static_cast<std::streamsize> (m.size ()));
    }
    else
    {
        std::vector<char> mm;
        // Transform to a vector of chars
        mm.resize (m.size () * 2);
        char *mmp = &mm[0];
        for (size_t i = 0; i < m.size (); ++i)
        {
            // Big endian
            *mmp++ = static_cast<char> (m[i] >> 8);
            *mmp++ = static_cast<char> (m[i]);
        }
        // Write the pnm pixels
        os.write (&mm[0], static_cast<std::streamsize> (mm.size ()));
    }
}

// PNM file header reader helper
inline void read_pnm_comment (std::istream &s)
{
    // Ignore whitespace
    s >> std::ws;
    // A '#' marks a comment line
    while (s.peek () == '#')
    {
        const unsigned LINE_LENGTH = 256;
        char line[LINE_LENGTH];
        s.getline (line, LINE_LENGTH);
        // Ignore whitespace
        s >> std::ws;
    }
}

// PNM file reader helper
inline void read_pnm_header (std::istream &is, bool &rgb, bool &bpp16, size_t &w, size_t &h)
{
    char ch;
    is >> ch;
    if (ch != 'P')
        throw std::runtime_error ("Invalid PNM magic number");
    is >> ch;
    switch (ch)
    {
        case '5':
        rgb = false;
        break;
        case '6':
        rgb = true;
        break;
        default:
        throw std::runtime_error ("Unknown PNM magic number");
    }
    read_pnm_comment (is);
    is >> w;
    read_pnm_comment (is);
    is >> h;
    unsigned maxval;
    is >> maxval;
    if (maxval < 256)
        bpp16 = false;
    else if (maxval < 65536)
        bpp16 = true;
    else
        throw std::runtime_error ("Only 8 or 16 bits per pixel is supported.");
    // Read a single WS
    is.get (ch);
}

// PNM file reader helper: deprecated
inline void read_pnm_header (std::istream &is, size_t &bytes_per_pixel, size_t &w, size_t &h)
{
    bool rgb;
    bool bpp16;
    read_pnm_header (is, rgb, bpp16, w, h);
    if (!bpp16)
        bytes_per_pixel = rgb ? 3 : 1;
    else
        bytes_per_pixel = rgb ? 6 : 2;
}

// PNM file reader helper
template <typename T>
void read_pnm_pixels (std::istream &is, T &pixels)
{
    assert (sizeof (typename T::value_type) == 1);
    const std::streamsize sz =
        static_cast<std::streamsize> (pixels.size ());
    is.read (reinterpret_cast<char *> (&pixels[0]), sz);
}

// PNM file reader helper
//
// Read big endian 16 bit pixels into a 16 bit little endian buffer
template <typename T>
void read_pnm_pixels16 (std::istream &is, T &pixels)
{
    assert (sizeof (typename T::value_type) == 2);
    const size_t sz = pixels.size () * 2;
    // read them
    is.read (reinterpret_cast<char *> (&pixels[0]), sz);
    char *q = reinterpret_cast<char *> (&pixels[0]);
    // swap them
    for (size_t i = 0; i < sz; i += 2)
        std::swap (q[i], q[i + 1]);
}

// PNM file reader helper
template <typename SRC, typename DEST>
void convert_pnm_8bit_to_16bit (const SRC &src, DEST &dest)
{
    assert (src.size () == dest.size () * 2);
    const typename SRC::value_type *p = &src[0];
    for (size_t i = 0; i < dest.size (); ++i)
    {
        dest[i] = *p++;
        dest[i] = (dest[i] << 8) + *p++;
    }
}

} // namespace horny_toad

#endif // PNM_H
