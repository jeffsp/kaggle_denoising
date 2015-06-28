/// @file color.h
/// @brief color
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef COLOR_H
#define COLOR_H

#include <cmath>

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

// CIE 1931 D65 (midday sun in Western Europe) in xy
inline double CIE1931D65x () { return 0.31271; }
inline double CIE1931D65y () { return 0.32902; }

// Convert from CIE 1931 xyY to CIE 1931 XYZ
inline double CIE1931xyYX (double x, double y, double Y) { return x * Y / y; }
inline double CIE1931xyYY (double  , double  , double Y) { return Y; }
inline double CIE1931xyYZ (double x, double y, double Y) { return (1 - x - y) * Y / y; }

// CIE 1931 D65 in XYZ
inline double CIE1931D65X (double Y) { return CIE1931xyYX (CIE1931D65x (), CIE1931D65y (), Y); }
inline double CIE1931D65Y (double Y) { return CIE1931xyYY (CIE1931D65x (), CIE1931D65y (), Y); }
inline double CIE1931D65Z (double Y) { return CIE1931xyYZ (CIE1931D65x (), CIE1931D65y (), Y); }

// Helper functions for XYZ/L*a*b* conversion
template<typename Ty>
inline Ty LabFunc (Ty t)
{
    if (t > 0.008856452)
        return std::pow (t, 1.0 / 3.0); // cubed root
    else
        return 7.787037 * t + 16.0 / 116.0;
}
inline double Sigma () { return 6.0 / 29.0; }
inline double Yref () { return 1.00; }
inline double Xref () { return CIE1931xyYX (CIE1931D65x (), CIE1931D65y (), Yref ()); } // 0.950429
inline double Zref () { return CIE1931xyYZ (CIE1931D65x (), CIE1931D65y (), Yref ()); } // 1.0889

// Convert between CIE 1931 XYZ and CIE L*a*b*
template<typename Ty>
inline Ty CIE1976LabL (Ty  , Ty Y, Ty  ) { return 116 * LabFunc (Y / Yref ()) - 16; }
template<typename Ty>
inline Ty CIE1976Laba (Ty X, Ty Y, Ty  ) { return 500 * (LabFunc (X / Xref ()) - LabFunc (Y / Yref ())); }
template<typename Ty>
inline Ty CIE1976Labb (Ty  , Ty Y, Ty Z) { return 200 * (LabFunc (Y / Yref ()) - LabFunc (Z / Zref ())); }
template<typename Ty>
inline Ty CIE1976LabX (Ty L, Ty a, Ty  )
{
    Ty fy = (L + 16.0) / 116.0;
    Ty fx = fy + a / 500.0;
    if (fx > Sigma ())
        return Xref () * fx * fx * fx;
    else
        return Xref () * (fx - 16.0 / 116.0) * 3 * Sigma () * Sigma ();
}
template<typename Ty>
inline Ty CIE1976LabY (Ty L, Ty  , Ty  )
{
    Ty fy = (L + 16.0) / 116.0;
    if (fy > Sigma ())
        return Yref () * fy * fy * fy;
    else
        return Yref () * (fy - 16.0 / 116.0) * 3 * Sigma () * Sigma ();
}
template<typename Ty>
inline Ty CIE1976LabZ (Ty L, Ty  , Ty b)
{
    Ty fy = (L + 16.0) / 116.0;
    Ty fz = fy - b / 200.0;
    if (fz > Sigma ())
        return Zref () * fz * fz * fz;
    else
        return Zref () * (fz - 16.0 / 116.0) * 3 * Sigma () * Sigma ();
}

// Convert between Linear RGB and sRGB
//
// Linear RGB must be in the range [0.0, 1.0]
inline double RGB709tosRGB (double c)
{
    if (c <= 0.0031308)
        return 12.92 * c;
    else
        return (1 + 0.055) * std::pow (c, 1.0 / 2.2) - 0.055;
}
inline double sRGBtoRGB709 (double c)
{
    if (c <= 0.04045)
        return c / 12.92;
    else
        return std::pow (((c + 0.055) / (1 + 0.055)), 2.2);
}

// Convert between CIE 1931 XYZ and Rec 709 RGB (linear RGB) with D65 whitepoint
template<typename Ty>
inline Ty RGB709X  (Ty r, Ty g, Ty b) { return 0.412453 * r + 0.357580 * g + 0.180423 * b; }
template<typename Ty>
inline Ty RGB709Y  (Ty r, Ty g, Ty b) { return 0.212671 * r + 0.715160 * g + 0.072169 * b; }
template<typename Ty>
inline Ty RGB709Z  (Ty r, Ty g, Ty b) { return 0.019334 * r + 0.119193 * g + 0.950227 * b; }
template<typename Ty>
inline Ty RGB709R  (Ty X, Ty Y, Ty Z) { return  3.240479 * X + -1.537150 * Y + -0.498535 * Z; }
template<typename Ty>
inline Ty RGB709G  (Ty X, Ty Y, Ty Z) { return -0.969256 * X +  1.875992 * Y +  0.041556 * Z; }
template<typename Ty>
inline Ty RGB709B  (Ty X, Ty Y, Ty Z) { return  0.055648 * X + -0.204043 * Y +  1.057311 * Z; }

// To go from sRGB[0,255] to L*a*b*[0.0,1.0], you need to perform the following:
//
//  sRGB[0,255] -> sRGB[0.0,1.0] -> Linear RGB -> XYZ -> L*a*b*
template<typename Ty>
inline void sRGBtoLab (unsigned char r,
    unsigned char g,
    unsigned char b,
    Ty *lr,
    Ty *ar,
    Ty *br)
{
    // Convert to linear RGB[0.0,1.0];
    Ty tr = sRGBtoRGB709 (r / 255.0);
    Ty tg = sRGBtoRGB709 (g / 255.0);
    Ty tb = sRGBtoRGB709 (b / 255.0);
    Ty x = RGB709X (tr, tg, tb);
    Ty y = RGB709Y (tr, tg, tb);
    Ty z = RGB709Z (tr, tg, tb);
    *lr = CIE1976LabL (x, y, z);
    *ar = CIE1976Laba (x, y, z);
    *br = CIE1976Labb (x, y, z);
}

// ITU.BT-709 HDTV studio production in Y'CbCr
//
// NOTE that these values are different than the standard for CRTs, ITU-R
// BT.601, which uses the constants k_r=0.299, k_b=0.114
template<typename Ty>
inline double YCbCr709Y  (Ty r, Ty g, Ty b) { return   0.2215 * r + 0.7154 * g + 0.0721 * b; }
template<typename Ty>
inline double YCbCr709Cb (Ty r, Ty g, Ty b) { return  -0.1145 * r - 0.3855 * g + 0.5000 * b; }
template<typename Ty>
inline double YCbCr709Cr (Ty r, Ty g, Ty b) { return   0.5016 * r - 0.4556 * g - 0.0459 * b; }
template<typename Ty>
inline double YCbCr709R  (Ty y, Ty cb, Ty cr) { return  y + 0.0000 * cb + 1.5701 * cr; }
template<typename Ty>
inline double YCbCr709G  (Ty y, Ty cb, Ty cr) { return  y - 0.1870 * cb - 0.4664 * cr; }
template<typename Ty>
inline double YCbCr709B  (Ty y, Ty cb, Ty cr) { return  y + 1.8556 * cb + 0.0000 * cr; }

// ITU.BT-709 HDTV studio production in Y'CbCr functors
//
// NOTE that these values are different than the standard for CRTs, ITU-R
// BT.601, which uses the constants k_r=0.299, k_b=0.114
template<typename R,typename T>
struct YCbCr709YFunc {
inline R operator()  (T r, T g, T b) { return   0.2215 * r + 0.7154 * g + 0.0721 * b; } };
template<typename R,typename T>
struct YCbCr709CbFunc {
inline R operator() (T r, T g, T b) { return  -0.1145 * r - 0.3855 * g + 0.5000 * b; } };
template<typename R,typename T>
struct YCbCr709CrFunc {
inline R operator() (T r, T g, T b) { return   0.5016 * r - 0.4556 * g - 0.0459 * b; } };

template<typename R,typename T>
struct YCbCr709RFunc {
inline R operator() (T y, T cb, T cr) { return  y + 0.0000 * cb + 1.5701 * cr; } };
template<typename R,typename T>
struct YCbCr709GFunc {
inline R operator() (T y, T cb, T cr) { return  y - 0.1870 * cb - 0.4664 * cr; } };
template<typename R,typename T>
struct YCbCr709BFunc {
inline R operator() (T y, T cb, T cr) { return  y + 1.8556 * cb + 0.0000 * cr; } };

// SMTPE-240M Y'PbPr
template<typename Ty>
inline double YPbPrY  (Ty r, Ty g, Ty b) { return  0.2122 * r + 0.7013 * g + 0.0865 * b; }
template<typename Ty>
inline double YPbPrPb (Ty r, Ty g, Ty b) { return -0.1162 * r - 0.3838 * g + 0.5000 * b; }
template<typename Ty>
inline double YPbPrPr (Ty r, Ty g, Ty b) { return  0.5000 * r - 0.4451 * g - 0.0549 * b; }
template<typename Ty>
inline double YPbPrR (Ty y, Ty pb, Ty pr) { return  y + 0.0000 * pb + 1.5756 * pr; }
template<typename Ty>
inline double YPbPrG (Ty y, Ty pb, Ty pr) { return  y - 0.2253 * pb - 0.5000 * pr; }
template<typename Ty>
inline double YPbPrB (Ty y, Ty pb, Ty pr) { return  y + 1.8270 * pb + 0.0000 * pr; }

} // namespace horny_toad

#endif // COLOR_H
