/// @file raster_utils.h
/// @brief raster utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef RASTER_UTILS_H
#define RASTER_UTILS_H

#include "pi.h"
#include "pnm.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

/// @brief Horny Toad Utility Functions
namespace horny_toad
{

/// @brief Output a 2d block to a stream
/// @param s The stream
/// @param m The block
template<typename T>
std::ostream& print2d (std::ostream &s, const T &m)
{
    for (size_t i = 0; i < m.rows (); ++i) {
        for (size_t j = 0; j < m.cols (); ++j)
            s << '\t' << m (i, j);
        s << std::endl; }
    return s;
}

/// @brief Output a 2d block to a stream
/// @param s The stream
/// @param m The block
///
/// First cast the element contained in T to a U
template<typename T,typename U>
std::ostream& print2d (std::ostream &s, const T &m, U)
{
    for (size_t i = 0; i < m.rows (); ++i) {
        for (size_t j = 0; j < m.cols (); ++j)
            s << '\t' << static_cast<U> (m (i, j));
        s << std::endl; }
    return s;
}

/// @brief Output a 2d block to a stream
/// @param s The stream
/// @param m The block
/// @param w The width
/// @param p The precision
/// @param f The fill character
template<typename T>
std::ostream& print2d (std::ostream &s, const T &m, int w, int p, char f)
{
    for (size_t i = 0; i < m.rows (); ++i) {
        for (size_t j = 0; j < m.cols (); ++j) {
            s.width (w);
            s.precision (p);
            s.fill (f);
            s << m (i, j);
        }
        s << std::endl; }
    return s;
}

/// @brief Read a 2d block from a stream
/// @tparam T return type
/// @param s The stream
/// @return The block
template<typename T>
T read2d (std::istream &s)
{
    std::string line;
    std::vector<typename T::value_type> tmp;
    size_t rows = 0;
    size_t cols = 0;
    while (getline (s, line))
    {
        std::istringstream iss (line);
        double x;
        bool read_a_value = false;
        while (iss >> x)
        {
            tmp.push_back (x);
            // If we are on first line
            if (rows == 0)
                ++cols;
            // Check for empty lines
            read_a_value = true;
        }
        if (read_a_value)
            ++rows;
    }
    T r (rows, cols, tmp);
    return r;
}

/// @brief Crop a portion from a raster
/// @param p The raster
/// @param r1 Starting row
/// @param c1 Starting col
/// @param r2 Ending row (inclusive)
/// @param c2 Ending col (inclusive)
template<typename T>
T crop (const T &p, size_t r1, size_t c1, size_t r2, size_t c2)
{
    assert (r1 < p.rows ());
    assert (r2 < p.rows ());
    assert (r1 <= r2);
    assert (c1 < p.cols ());
    assert (c2 < p.cols ());
    assert (c1 <= c2);
    T r (r2 - r1 + 1, c2 - c1 + 1);
    for (size_t i = r1; i <= r2; ++i)
        for (size_t j = c1; j <= c2; ++j)
            r (i - r1, j - c1) = p (i, j);
    return r;
}

/// @brief Crop a portion from a raster
/// @tparam T Image type
/// @param p The raster
/// @param c Pixels to crop
template<typename T>
T crop (const T &p, size_t c)
{
    return crop (p, c, c, p.rows () - c - 1, p.cols () - c - 1);
}

/// @brief Add a border to an image
///
/// @tparam S subregion type
/// @tparam T image type
/// @param p The raster
/// @param c Number of pixels to add
///
/// @return the bordered image
template<typename S,typename T>
T border (const T &p, unsigned c)
{
    T q (p.rows () + c * 2, p.cols () + c * 2);
    S s = { c, c, p.rows (), p.cols () };
    copy (p.begin (), p.end (), q.begin (s));
    return q;
}

/// @brief Flip a raster horizontally
///
/// @tparam T image type
/// @param p The raster
///
/// @return the flipped raster
template<typename T>
T fliplr (const T &p)
{
    T q (p.rows (), p.cols ());
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (i, j) = p (i, p.cols () - 1 - j);
    return q;
}

/// @brief Flip a raster vertically
///
/// @tparam T image type
/// @param p The raster
///
/// @return the flipped raster
template<typename T>
T flipud (const T &p)
{
    T q (p.rows (), p.cols ());
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (i, j) = p (p.rows () - 1 - i, j);
    return q;
}

/// @brief Flip a raster vertically and horizontally
///
/// @tparam T image type
/// @param p The raster
///
/// @return the flipped raster
template<typename T>
T fliplrud (const T &p)
{
    T q (p.rows (), p.cols ());
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (i, j) = p (p.rows () - 1 - i, p.cols () - 1 - j);
    return q;
}

/// @brief Transpose a raster
///
/// @tparam T image type
/// @param p The raster
///
/// @return the transposed raster
template<typename T>
T transpose (const T &p)
{
    T q (p.cols (), p.rows ());
    for (size_t i = 0; i < p.rows (); ++i)
        for (size_t j = 0; j < p.cols (); ++j)
            q (j, i) = p (i, j);
    return q;
}

/// @brief Add a mirrored border to an image
///
/// @tparam S subregion type
/// @tparam T Image type
/// @param p Image
/// @param c Number of pixels to add
///
/// @return The mirror bordered image
template<typename S,typename T>
T mborder (const T &p, unsigned c)
{
    T q = border<S> (p, c);
    for (size_t i = 0; i < q.rows (); ++i)
    {
        size_t ii = i;
        if (i < c)
            ii = 2 * c - i;
        else if (i >= q.rows () - c)
            ii = (2 * q.rows () - i) - 2 * (c + 1);
        for (size_t j = 0; j < q.cols (); ++j)
        {
            size_t jj = j;
            if (j < c)
                jj = 2 * c - j;
            else if (j >= q.cols () - c)
                jj = (2 * q.cols () - j) - 2 * (c + 1);
            if (ii == i && jj == j)
                continue;
            assert (ii < q.rows ());
            assert (jj < q.cols ());
            q (i, j) = q (ii, jj);
        }
    }
    return q;
}

/// @brief Compute the local mean at a subregion
/// @param subregion_beg Image begin iterator
/// @param subregion_end Image end iterator
/// @param weights_beg Weights begin iterator
///
/// The local mean is computed by applying the weighting
/// function over the subregion.
template<typename ImageIter,typename WeightsIter>
typename WeightsIter::value_type local_mean (
    const ImageIter &subregion_beg,
    const ImageIter &subregion_end,
    const WeightsIter &weights_beg)
{
    typename WeightsIter::value_type mean = 0;
    ImageIter i;
    WeightsIter w;
    for (i = subregion_beg, w = weights_beg; i != subregion_end; ++i, ++w)
        mean += *w * *i;
    return mean;
}

/// @brief Compute the local mean at a subregion
/// @param subregion_beg Image begin iterator
/// @param subregion_end Image end iterator
template<typename ImageIter>
double local_mean (
    const ImageIter &subregion_beg,
    const ImageIter &subregion_end)
{
    double mean = 0.0;
    ImageIter i;
    size_t total = 0;
    for (i = subregion_beg; i != subregion_end; ++i)
    {
        mean += *i;
        ++total;
    }
    return mean / total;
}

/// @brief Compute the local variance at a subregion
/// @param subregion_beg Image begin iterator
/// @param subregion_end Image end iterator
/// @param weights_beg Weights begin iterator
template<typename ImageIter,typename WeightsIter>
typename WeightsIter::value_type local_variance (
    const ImageIter &subregion_beg,
    const ImageIter &subregion_end,
    const WeightsIter &weights_beg)
{
    typename WeightsIter::value_type mean = local_mean (
            subregion_beg,
            subregion_end,
            weights_beg);
    typename WeightsIter::value_type var = 0;
    ImageIter i;
    WeightsIter w;
    for (i = subregion_beg, w = weights_beg; i != subregion_end; ++i, ++w)
    {
        typename WeightsIter::value_type t = *i - mean;
        var += t * t * *w;
    }
    return var;
}

/// @brief Compute the RMS contrast over a range of pixels
/// @param img_beg beginning of pixel range
/// @param img_end end of pixel range
/// @param w_beg beginning of weighting function
/// @param w_sum sum of weighting pixels
/// @param i_dark optional 'dark light' parameter
/// @invariant pixel range and weighting function range are
/// the same size
///
/// This version uses a precomputed sum of the weighting
/// function values and allows for a 'dark light' parameter.
template<typename Iter1,typename Iter2,typename T,typename U>
T local_rms_contrast_p (
    Iter1 img_beg, Iter1 img_end,
    Iter2 w_beg,
    T w_sum,
    U i_dark)
{
    Iter1 i;
    Iter2 w;
    // Compute local luminance
    T local_lum = 0.0;
    for (i = img_beg, w = w_beg; i != img_end; ++i, ++w)
        local_lum += *w * *i;
    local_lum /= w_sum;
    //std::clog << "local_lum: " << local_lum << std::endl;
    // Compute sum of weighted squares
    T ss = T ();
    for (i = img_beg, w = w_beg; i != img_end; ++i, ++w)
        ss += *w * (*i - local_lum) * (*i - local_lum);
    // Normalize intensities
    ss /= (local_lum + i_dark) * (local_lum + i_dark);
    //std::clog << "ss: " << ss << std::endl;
    // Normalize weights
    //std::clog << "w_sum: " << w_sum << std::endl;
    ss /= w_sum;
    // Return rms contrast
    //std::clog << "rms: " << sqrt (ss) << std::endl;
    return sqrt (ss);
}

/// @brief Compute the RMS contrast over a range of pixels
/// @param img_beg beginning of pixel range
/// @param img_end end of pixel range
/// @param w_beg beginning of weighting function
template<typename ImageIter,typename WeightsIter>
typename WeightsIter::value_type local_rms_contrast (
    ImageIter img_beg, ImageIter img_end,
    WeightsIter w_beg)
{
    typename WeightsIter::value_type v = local_variance (img_beg, img_end, w_beg);
    typename WeightsIter::value_type m = local_mean (img_beg, img_end, w_beg);
    return sqrt (v) / m;
}

/// @brief Compute RMS contrast across an entire image
/// @param S The subregion type
/// @param C The contrast and weighting function type
/// @param T The image type
/// @param img The image
/// @param weights A centered spatial weighting function
/// @param dark_light The dark/light parameter
/// @return The rms contrast output values
/// @note The subregion type must be compatible with the
/// image subregion iterators (e.g.: jack_rabbit::subregion)
/// @note The image type is independent of the contrast and
/// weighting function type (i.e.: unsigned char images
/// work)
/// @note The weighting function type determines the
/// contrast type.
template<typename S,typename C,typename T>
C rms_contrast (const T &img, const C &weights, double dark_light = 0.0)
{
    assert (img.rows () >= weights.rows ());
    assert (img.cols () >= weights.cols ());
    typename C::value_type w_sum = std::accumulate (weights.begin (), weights.end (), 0.0);
    const size_t r_offset = weights.rows () / 2;
    const size_t c_offset = weights.cols () / 2;
    const size_t r_max = img.rows () - r_offset;
    const size_t c_max = img.cols () - c_offset;
    C contrast (img.rows (), img.cols ());
    for (size_t r = r_offset; r < r_max; ++r)
    {
        for (size_t c = c_offset; c < c_max; ++c)
        {
            S s = { r - r_offset, c - c_offset, weights.rows (), weights.cols () };
            typename C::value_type rms = local_rms_contrast_p (img.begin (s), img.end (s), weights.begin (), w_sum, dark_light);
            assert (rms >= 0.0);
            assert (r < contrast.rows ());
            assert (c < contrast.cols ());
            contrast (r, c) = rms;
        }
    }
    return contrast;
}

/// @brief Compute RMS contrast across an entire image
/// @param S The subregion type
/// @param T The image type
/// @param W The weighting function type
/// @param C The contrast type
/// @param M The mean type
/// @param img The image
/// @param w A centered spatial weighting function
/// @param c The returned
/// @param m The mean
/// @note The subregion type must be compatible with the
/// image subregion iterators (e.g.: jack_rabbit::subregion)
///
/// This version returns both the contrast and the mean.
template<typename S,typename T,typename W,typename C,typename M>
void rms_contrast_m (const T &img, const W &w, C &c, M &m)
{
    assert (img.rows () >= w.rows ());
    assert (img.cols () >= w.cols ());
    assert (img.rows () == c.rows ());
    assert (img.cols () == c.cols ());
    assert (img.rows () == m.rows ());
    assert (img.cols () == m.cols ());
    const size_t r_min = w.rows () / 2;
    const size_t c_min = w.cols () / 2;
    const size_t r_max = img.rows () - r_min;
    const size_t c_max = img.cols () - c_min;
    for (size_t row = r_min; row < r_max; ++row)
    {
        for (size_t col = c_min; col < c_max; ++col)
        {
            S s = { row - r_min, col - c_min, w.rows (), w.cols () };
            assert (row < c.rows ());
            assert (col < c.cols ());
            typename W::value_type lv = local_variance (img.begin (s), img.end (s), w.begin ());
            typename W::value_type lm = local_mean (img.begin (s), img.end (s), w.begin ());
            m (row, col) = lm;
            c (row, col) = sqrt (lv) / lm;
        }
    }
}

/// @brief Reflect a number at 0 and some maximum
/// @param i The number to reflect
/// @param n The maximum
/// @return The reflection of i
size_t reflect (int i, size_t n)
{
    // C does not guarantee that the modulus operator return a positive number,
    // otherwise Chris Bradley's trick would work:
    //return abs (n - (i - n) % 2 * n);
    if (i < 0)
        return -i;
    if (i > static_cast<int> (n))
        return 2 * n - i;
    return i;
}

/// @brief Dot product a kernel and an image patch, mirroring if necessary
/// @tparam T Kernel type
/// @tparam U Image type
/// @param k kernel
/// @param p image
/// @param start_i row,column offset within the image
/// @param start_j
/// @return The dot product
///
/// The kernel should have odd dimensions, and the kernel must be smaller than the image.
template<typename T,typename U>
double mirrored_dot_product (const T &k, const U &p, int start_i, int start_j)
{
    assert (k.rows () < p.rows ());
    assert (k.cols () < p.cols ());
    double sum = 0.0;
    for (size_t i = 0; i < k.rows (); ++i)
    {
        int ii = reflect (start_i + i, p.rows () - 1);
        for (size_t j = 0; j < k.cols (); ++j)
        {
            int jj = reflect (start_j + j, p.cols () - 1);
            assert (i < k.rows ());
            assert (j < k.cols ());
            assert (static_cast<size_t> (ii) < p.rows ());
            assert (static_cast<size_t> (jj) < p.cols ());
            sum += k (i, j) * p (ii, jj);
        }
    }
    return sum;
}

/// @brief Convolve a kernel with an image
/// @param S The subregion type
/// @param K The kernel type
/// @param T The image type
/// @param img The image
/// @param kernel A centered kernel
/// @return The output values
/// @note The subregion type must be compatible with the
/// image subregion iterators (e.g.: jack_rabbit::subregion)
/// @note The image type is independent of the kernel type
/// (e.g.: unsigned char images work)
/// @note The kernel type determines the returned type.
template<typename S,typename K,typename T>
K convolve (const T &img, const K &kernel)
{
    const size_t r_max = img.rows () - kernel.rows () + 1;
    const size_t c_max = img.cols () - kernel.cols () + 1;
    const size_t r_offset = kernel.rows () / 2;
    const size_t c_offset = kernel.cols () / 2;
    K ret (img.rows (), img.cols ());
    for (size_t r = r_offset; r < r_max; ++r)
    {
        for (size_t c = c_offset; c < c_max; ++c)
        {
            S s = { r - r_offset, c - c_offset, kernel.rows (), kernel.cols () };
            typename K::value_type v = inner_product (img.begin (s), img.end (s),
                kernel.begin (), 0.0);
            assert (r < ret.rows ());
            assert (c < ret.cols ());
            ret (r, c) = v;
        }
    }
    return ret;
}

/// @brief Convenience function
inline double radians (double degrees)
{ return degrees * PI () / 180.0; }

/// @brief Convenience function object
template<typename T>
class rotation
{
    public:
    /// @brief Constructor
    /// @param theta Counter-clockwise rotation
    rotation (double theta)
        : cos_theta_ (cos (theta))
        , sin_theta_ (sin (theta))
    { }
    /// @brief Get rotated x coord
    /// @param x original x coord
    /// @param y original y coord
    T fx (T x, T y) const
    { return x * cos_theta_ - y * sin_theta_; }
    /// @brief Get rotated y coord
    /// @param x original x coord
    /// @param y original y coord
    T fy (T x, T y) const
    { return x * sin_theta_ + y * cos_theta_; }
    private:
    double cos_theta_;
    double sin_theta_;
};

/// @brief Base helper class for function objects
/// @note This class uses x,y notation instead of row,col
/// notation because it specifies arbitrary points in the x-y
/// plane, not integer coordinates.
template<typename T>
class centered
{
    protected:
    /// @brief Limit access to constructors
    centered (T cx = 0, T cy = 0) : cx_ (cx), cy_ (cy) { }
    public:
    /// @brief Set x coord
    /// @param x x coord
    void center_x (T x) { cx_ = x; }
    /// @brief Set y coord
    /// @param y y coord
    void center_y (T y) { cy_ = y; }
    protected:
    /// @brief x cood
    T cx_;
    /// @brief y coord
    T cy_;
};

/// @brief A circular aperture function object
template<typename T>
class aperture : public centered<T>
{
    public:
    aperture (size_t rows, size_t cols)
        : centered<T> ((cols - 1) / 2, (rows - 1) / 2)
        , radius_ (T ()), ramp_ (T ())
    {
        // Set a default radius
        T diameter = (std::min) (rows, cols);
        radius (diameter / 2.0);
    }
    T radius () const { return radius_; }
    void radius (T r) { radius_ = r; }
    T ramp () const { return ramp_; }
    void ramp (T ramp) { ramp_ = ramp; }
    T operator() (size_t row, size_t col) const
    {
        T dx = col - centered<T>::cx_;
        T dy = row - centered<T>::cy_;
        T d = std::sqrt (dx * dx + dy * dy);
        // Compute aperture value
        if (d > radius_)
            return 0.0; // outside
        if (d <= radius_ - ramp_)
            return 1.0; // inside
        else
            return (radius_ - d) / ramp_; // on the edge
    }
    T operator() (size_t row, size_t col, const T &v) const
    {
        return v * this->operator() (row, col);
    }
    private:
    T radius_, ramp_;
};

/// @brief An oriented sine wave function object
///
/// This functor generates a sine wave function value given x, y
/// coordinates.  The idea is to use it to fill a matrix-like
/// container with a sine wave grating using an algorithm that
/// calls this functor for each element location.
///
/// Note that the matrix that it fills assumes that element 0, 0
/// is in the logical top left of the matrix.
template<typename T>
class oriented_sin : public centered<T>
{
    public:
    oriented_sin (size_t rows, size_t cols)
        : centered<T> ((cols - 1) / 2, (rows - 1) / 2)
        , freq_ (T ()), phase_ (T ()), rot_ (T ())
    {
        // Set a default frequency
        T diameter = (std::min) (rows, cols);
        frequency (1.0 / (diameter - 1));
    }
    T frequency () const { return freq_; }
    void frequency (T f) { freq_ = f; }
    T phase () const { return phase_; }
    void phase (T p) { phase_ = p; }
    void orientation (T theta) { rot_ = rotation<T> (theta); }
    T operator() (size_t row, size_t col) const
    {
        // Translate and rotate it
        T xx = rot_.fx (col - centered<T>::cx_, row - centered<T>::cy_);
        // Create it
        return sin (2.0 * PI () * xx * freq_ - phase_);
    }
    T operator() (size_t row, size_t col, const T &v) const
    {
        return v * this->operator() (row, col);
    }
    private:
    T freq_, phase_;
    rotation<T> rot_;
};

/// @brief A radial cosine wave function object
template<typename T>
class radial_cos : public centered<T>
{
    public:
    radial_cos (size_t rows, size_t cols)
        : centered<T> ((cols - 1) / 2, (rows - 1) / 2)
        , freq_ (T ()), phase_ (T ())
    {
        // Set a default frequency
        T diameter = (std::min) (rows, cols);
        frequency (1.0 / (diameter - 1));
    }
    T frequency () const { return freq_; }
    void frequency (T f) { freq_ = f; }
    T phase () const { return phase_; }
    void phase (T p) { phase_ = p; }
    T operator() (size_t row, size_t col) const
    {
        T tx = col - centered<T>::cx_;
        T ty = row - centered<T>::cy_;
        return cos (2.0 * PI () * sqrt (tx * tx + ty * ty) * freq_ - phase_);
    }
    T operator() (size_t row, size_t col, const T &v) const
    {
        return v * this->operator() (row, col);
    }
    private:
    T freq_, phase_;
};

/// @brief A raised cosine function object
template<typename T>
class raised_cos : public radial_cos<T>, public aperture<T>
{
    public:
    raised_cos (size_t r, size_t c)
        : radial_cos<T> (r, c)
        , aperture<T> (r, c)
        , min_aperture_ (r, c)
    {
        half_height (aperture<T>::radius () / 2.0);
    }
    T half_height () const { return half_height_; }
    void half_height (T hh)
    {
        half_height_ = hh;
        T period = half_height_ * 4.0;
        min_aperture_.radius (aperture<T>::radius () - half_height_ * 2);
        radial_cos<T>::frequency (1 / period);
        radial_cos<T>::phase (2.0 * PI () * (min_aperture_.radius () / period));
    }
    void center_x (T cx)
    {
        radial_cos<T>::center_x (cx);
        aperture<T>::center_x (cx);
        min_aperture_.center_x (cx);
    }
    void center_y (T cy)
    {
        radial_cos<T>::center_y (cy);
        aperture<T>::center_y (cy);
        min_aperture_.center_y (cy);
    }
    T operator() (size_t row, size_t col) const
    {
        if (min_aperture_ (row, col) != 0.0)
            return 1.0;
        else if (aperture<T>::operator() (row, col) != 0.0)
            return radial_cos<T>::operator() (row, col) / 2.0 + 0.5;
        else
            return 0.0;
    }
    T operator() (size_t row, size_t col, const T &v) const
    {
        return v * this->operator() (row, col);
    }
    private:
    T half_height_;
    aperture<T> min_aperture_;
};

/// @brief A Gaussian window function object
template<typename T>
class gaussian_window : public centered<T>
{
    public:
    gaussian_window (size_t rows, size_t cols)
        : centered<double> ((cols - 1.0) / 2.0, (rows - 1.0) / 2.0)
        , stddev_squared_ (T ())
    {
        // Pick some arbitrary, non-zero value for stddev
        T diameter = (std::min) (rows, cols);
        stddev (diameter / 5.0);
    }
    void stddev (T s) { stddev_squared_ = s * s; }
    T operator() (size_t row, size_t col) const
    {
        T tx = col - centered<T>::cx_;
        T ty = row - centered<T>::cy_;
        return exp (-0.5 * (tx * tx / stddev_squared_ + ty * ty / stddev_squared_));
    }
    T operator() (size_t row, size_t col, const T &v) const
    {
        return v * this->operator() (row, col);
    }
    private:
    T stddev_squared_;
};

/// @brief Clip a value to a range
/// @param min Minimum range value
/// @param max Maximum range value
template<typename Ty>
struct ClipFunc {
    ClipFunc (Ty min, Ty max)
        : min_ (min)
        , max_ (max)
    {
    }
    Ty operator() (const Ty &v)
    {
        if (v < min_)
            return min_;
        else if (v > max_)
            return max_;
        else
            return v;
    }
    Ty min_;
    Ty max_;
};

/// @brief split a 3 channel, interlaced data stream
template<typename SrcIter,
    typename DestIter>
void split3 (SrcIter beg,
    SrcIter end,
    DestIter a,
    DestIter b,
    DestIter c)
{
    for (; beg != end; )
    {
        *a++ = *beg++;
        *b++ = *beg++;
        *c++ = *beg++;
    }
}

/// @brief join 3 channels to an interlaced data stream
template<typename SrcIter,
    typename DestIter>
void join3 (SrcIter a_beg,
    SrcIter a_end,
    SrcIter b_beg,
    SrcIter c_beg,
    DestIter dest)
{
    for (; a_beg != a_end; )
    {
        *dest++ = *a_beg++;
        *dest++ = *b_beg++;
        *dest++ = *c_beg++;
    }
}

/// @brief join 3 channels to an interlaced data stream
template<typename SrcIter,
    typename DestIter,
    typename Op>
void transform3 (SrcIter a_beg,
    SrcIter a_end,
    SrcIter b_beg,
    SrcIter c_beg,
    DestIter dest,
    Op op)
{
    for (; a_beg != a_end; )
        *dest++ = op (*a_beg++, *b_beg++, *c_beg++);
}

/// @brief read a grayscale pnm
///
/// @param ifs input stream
///
/// @return the image
jack_rabbit::raster<unsigned char> read_grayscale (std::istream &ifs)
{
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    if (rgb)
        throw std::runtime_error ("the file is not grayscale");
    if (bpp16)
        throw std::runtime_error ("the file is not 8 bit");
    jack_rabbit::raster<unsigned char> p (h, w);
    horny_toad::read_pnm_pixels (ifs, p);
    return p;
}

/// @brief read a grayscale image
///
/// @param fn image name
///
/// @return the image
jack_rabbit::raster<unsigned char> read_grayscale (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    return read_grayscale (ifs);
}

/// @brief read a 16-bit grayscale
///
/// @param ifs input stream
///
/// @return the image
jack_rabbit::raster<unsigned short> read_grayscale16 (std::istream &ifs)
{
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    if (rgb)
        throw std::runtime_error ("the file is not grayscale");
    if (!bpp16)
        throw std::runtime_error ("the file is not 16 bit");
    jack_rabbit::raster<unsigned short> p (h, w);
    horny_toad::read_pnm_pixels16 (ifs, p);
    return p;
}

/// @brief read a 16-bit grayscale
///
/// @param fn input filename
///
/// @return the image
jack_rabbit::raster<unsigned short> read_grayscale16 (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    return read_grayscale16 (ifs);
}

/// @brief check if file is rgb
///
/// @param fn filename
///
/// @return true if it's rgb
bool is_rgb (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    return rgb;
}

/// @brief check if a file is 16-bit
///
/// @param fn filename
///
/// @return true if it's 16-bit
bool is_bpp16 (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    return bpp16;
}

/// @brief read rgb image planes
///
/// @param ifs input file stream
///
/// @return the image
std::vector<jack_rabbit::raster<unsigned char>> read_planes (std::istream &ifs)
{
    if (!ifs)
        throw std::runtime_error ("error reading from file");
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    if (bpp16)
        throw std::runtime_error ("the file is not 8 bit");
    std::vector<jack_rabbit::raster<unsigned char>> r;
    if (rgb)
    {
        jack_rabbit::raster<unsigned char> p (h, w * 3);
        horny_toad::read_pnm_pixels (ifs, p);
        r.resize (3);
        r[0].resize (h, w);
        r[1].resize (h, w);
        r[2].resize (h, w);
        horny_toad::split3 (p.begin (), p.end (), r[0].begin (), r[1].begin (), r[2].begin ());
    }
    else
    {
        r.resize (1);
        r[0].resize (h, w);
        horny_toad::read_pnm_pixels (ifs, r[0]);
    }
    return r;
}

/// @brief read rgb image planes
///
/// @param fn the input filename
///
/// @return the image
std::vector<jack_rabbit::raster<unsigned char>> read_planes (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    return read_planes (ifs);
}

/// @brief read an rgb image
///
/// @param ifs input stream
///
/// @return the image
jack_rabbit::raster<unsigned char> read_rgb (std::istream &ifs)
{
    if (!ifs)
        throw std::runtime_error ("error reading from file");
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    if (!rgb)
        throw std::runtime_error ("the file is not rgb");
    if (bpp16)
        throw std::runtime_error ("the file is not 8 bit");
    jack_rabbit::raster<unsigned char> p (h, w * 3);
    horny_toad::read_pnm_pixels (ifs, p);
    return p;
}

/// @brief read an rgb iamge
///
/// @param fn image filename
///
/// @return the image
jack_rabbit::raster<unsigned char> read_rgb (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    return read_rgb (ifs);
}

/// @brief read a 16-bit image
///
/// @param ifs input stream
///
/// @return the image
jack_rabbit::raster<unsigned short> read_rgb16 (std::istream &ifs)
{
    if (!ifs)
        throw std::runtime_error ("error reading from file");
    bool rgb, bpp16;
    size_t w, h;
    horny_toad::read_pnm_header (ifs, rgb, bpp16, w, h);
    if (!rgb)
        throw std::runtime_error ("the file is not rgb");
    if (!bpp16)
        throw std::runtime_error ("the file is not 16 bit");
    jack_rabbit::raster<unsigned short> p (h, w * 3);
    horny_toad::read_pnm_pixels16 (ifs, p);
    return p;
}

/// @brief read a 16-bit image
///
/// @param fn image filename
///
/// @return the image
jack_rabbit::raster<unsigned short> read_rgb16 (const char *fn)
{
    std::ifstream ifs (fn);
    if (!ifs)
        throw std::runtime_error ("could not open file for reading");
    return read_rgb16 (ifs);
}

/// @brief read an image from a file
///
/// @param fn the filename
///
/// @return the image
jack_rabbit::raster<unsigned char> read (const char *fn)
{
    jack_rabbit::raster<unsigned char> p;
    if (!is_rgb (fn))
        p = read_grayscale (fn);
    else
        p = read_rgb (fn);
    return p;
}

} // namespace horny_toad

#endif // RASTER_UTILS_H
