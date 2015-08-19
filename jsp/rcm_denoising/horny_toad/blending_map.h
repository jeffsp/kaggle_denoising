/// @file blending_map.h
/// @brief blending map
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef BLENDING_MAP_HPP
#define BLENDING_MAP_HPP

#include "rect.h"
#include "superpixel.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace // anonymous
{

// Helper function
template<typename T>
inline T logbase2 (T x)
{
    const T LOG2 = log (2.0);
    return log (x) / LOG2;
}

} // namespace anonymous

namespace horny_toad
{

/// @brief Base class for a blending map
///
/// Specifies how image pixels should be blended across pyramid
/// levels in a multiresolution pyramid.
template<typename T=float> // Precision
class blending_map
{
    public:
    /// @brief Destructor
    virtual ~blending_map ()
    { }
    /// @brief Get blending regions
    /// @param level Image pyramid level
    /// @return Blending regions
    /// @see get_blending_level
    ///
    /// Get rectangles that specify regions over which the
    /// image should be blended at a given pyramid level.
    ///
    /// The regions are specified relative to the image
    /// pixels where the top left image pixel covers the
    /// area in the image from (0,0) to (1,1).  The center
    /// of the top left pixel is therefore at (0.5,0.5).
    ///
    /// The regions will get broken up into superpixels
    /// for subsequent calls to 'get_blending_level'.
    std::vector<rect<T> > get_blending_regions (size_t level) const
    { return do_get_blending_regions (level); }
    /// @brief Set the fixation point
    /// @param fx Fixation x coordinate
    /// @param fy Fixation y coordinate
    ///
    /// The fixation point is specified relative to the area
    /// taken up by the image, not relative to the pixel
    /// coordinates.
    ///
    /// So, if you want to be precise, the fixation point
    /// for the pixel in the top left corner of the image
    /// should be specified as (0.5,0.5), not (0,0)
    void set_fixation (T fx, T fy)
    {
        do_set_fixation (fx, fy);
    }
    /// @brief Get the blending level given a superpixel
    /// @param s The superpixel
    /// @return The blending level
    ///
    /// The blending level is a floating point number that
    /// specifies a pyramid level.  A non-integer level
    /// indicates that a superpixel should be blended
    /// between two pyramid levels.
    ///
    /// For example, blending level 1.5 indicates that a
    /// superpixel should be blended between pyramid levels
    /// 1 and 2.
    ///
    /// Low resolution superpixels cover large areas in the
    /// high resolution image.  Ideally, the blending map
    /// would return the lowest (highest resolution)
    /// blending level for a superpixel that covers a large
    /// area.  However, this might not be efficient.  For
    /// example, if the blending map were to calculate the
    /// blending level for each high resolution pixel
    /// covered by a superpixel, this would defeat the
    /// whole purpose of doing the caluculation within a
    /// pyramid.
    ///
    /// Instead, if the blending map is relatively smooth,
    /// the blending map can return the blending level for
    /// the pixel in the center of a superpixel.  The
    /// superpixel will be broken up into smaller and
    /// smaller superpixels as the foveator works is way
    /// down the pyramid.
    T get_blending_level (const superpixel &s) const
    { return do_get_blending_level (s); }
    /// @brief Get the blending level at a point
    /// @param x x coordinate
    /// @param y y coordinate
    /// @return The blending level
    /// @see get_blending_level(const superpixel &)
    T get_blending_level (T x, T y) const
    { return do_get_blending_level (x, y); }
    private:
    /// @brief Template method for set_fixation
    ///
    /// See 'template method' and 'non-virtual interface'
    virtual void do_set_fixation (T fx, T fy) = 0;
    /// @brief Template method for get_blending_regions
    ///
    /// See 'template method' and 'non-virtual interface'
    virtual std::vector<rect<T> > do_get_blending_regions (size_t level) const = 0;
    /// @brief Template method for get_blending_level
    ///
    /// See 'template method' and 'non-virtual interface'
    virtual T do_get_blending_level (const superpixel &s) const = 0;
    /// @brief Template method for get_blending_level
    ///
    /// See 'template method' and 'non-virtual interface'
    virtual T do_get_blending_level (T x, T y) const = 0;
};

/// @brief A blending map based upon half resolution
///
/// The contrast threshold of a sine wave grating with
/// a certain frequency at a certain eccentricity is
/// described by this formula:
///
///     ct(f,e)=ct0*exp(alpha*f*((e+e2)/e2))
///
/// If we solve for 'f', we can see that frequency is
/// proportional to e2/(e+e2) for a given contrast
/// threshold.
///
///     f=ln(ct(f,e)/ct0)*(1/alpha)*((e+e2)/e2)
///
/// We will therefore use 'f' to find the pyramid level
/// that corresponds to a given eccentricity by setting
/// 'e2' to the eccentricity at which the Nyquist
/// frequency exactly matches pyramid level 0.
///
/// If a point lies within e2, the Nyquist frequency at
/// that point is higher than the highest resolution
/// image available -- this reflects the fact that the
/// highest resolution image available is itself a
/// downsampled representation of the real world.
template<typename T=float> // Precision
class e2_blending_map : public blending_map<T>
{
    public:
    /// @brief Constructor
    e2_blending_map () : fx_ (0), fy_ (0)
    {
        set_e2 (10);
    }
    /// @brief Get the blending_map's half resolution
    T get_e2 () const { return e2_; }
    /// @brief Set the blending_map's half resolution
    /// @param e2 The half resolution in pixels
    void set_e2 (T e2)
    {
        e2_ = e2;
        init_blending_level_table ();
    }
    /// @brief Set the fixation point
    /// @param fx Fixation x coordinate
    /// @param fy Fixation y coordinate
    /// @see blending_map::set_fixation
    void do_set_fixation (T fx, T fy)
    { fx_ = fx; fy_ = fy; }
    /// @brief Get the eccentricity given a pyramid level
    /// @param level pyramid level
    ///
    /// Determine the distance from the fixation required in
    /// order to have a particular pyramid level.
    T get_eccentricity (T level) const
    {
        //return 2 * e2_ * exp (level * log (2)) - e2_;
        return e2_ * (pow (2, level + 1) - 1);
    }
    /// @brief Get blending regions
    /// @param level Image pyramid level
    /// @return Blending regions
    /// @see get_blending_regions
    std::vector<rect<T> > do_get_blending_regions (size_t level) const
    {
        // Don't blend anything that is at pyramid level
        // 'max_level' or above.  The resolution at these
        // levels is too low.
        size_t max_level = static_cast<size_t> (level + 1);
        T max_ecc = get_eccentricity (max_level);
        assert (max_ecc >= 0.0);
        // Fit a square region around the fixation point
        T x1 = fx_ - max_ecc;
        T y1 = fy_ - max_ecc;
        T x2 = fx_ + max_ecc;
        T y2 = fy_ + max_ecc;
        assert (x1 <= x2);
        assert (y1 <= y2);
        // Return the region
        rect<T> r (x1, y1, x2 - x1, y2 - y1);
        // This version just returns a single region since
        // it has a single maximum.  You could return several
        // regions though, if you wanted.
        return std::vector<rect<T> > (1, r);
    }
    /// @brief Get the blending level given a superpixel
    /// @param s The superpixel
    /// @return The blending level
    /// @see get_blending_level
    T do_get_blending_level (const superpixel &s) const
    {
        T cx = s.get_col () + s.get_size () / 2.0;
        T cy = s.get_row () + s.get_size () / 2.0;
        return do_get_blending_level (cx, cy);
    }
    private:
    /// @brief Get the pyramid blending level at a point
    /// @param x x coordinate
    /// @param y y coordinate
    T do_get_blending_level (T x, T y) const
    {
        T dx = x - fx_;
        T dy = y - fy_;
        T e = sqrt (dx * dx + dy * dy);
        return do_get_blending_level (e);
    }
    /// @brief Get the blending level
    /// @param e eccentricity
    T do_get_blending_level (T e) const
    {
        size_t i = static_cast<size_t> (round (e));
        if (i >= BLENDING_LEVEL_TABLE_SIZE)
            return logbase2 ((e + e2_) / (2 * e2_));
        else
            return blending_level_table[i];
    }
    /// @brief Avoid calling log() if you can
    void init_blending_level_table ()
    {
        blending_level_table.resize (BLENDING_LEVEL_TABLE_SIZE);
        for (size_t e = 0; e < BLENDING_LEVEL_TABLE_SIZE; ++e)
            blending_level_table[e] = logbase2 ((e + e2_) / (2 * e2_));
    }
    // Properties
    T e2_; // half resolution
    T fx_; // fixation
    T fy_;
    static const size_t BLENDING_LEVEL_TABLE_SIZE = 1000;
    std::vector<T> blending_level_table;
};

/// @brief A blending map that references a bitmap
template<
    typename B, // Bitmap
    typename T=float> // Precision
class bitmap_blending_map : public blending_map<T>
{
    public:
    /// @brief Constructor
    bitmap_blending_map ()
        : divisor_ (1)
        , fx_ (0)
        , fy_ (0)
    {
        set_bitmap (bitmap_);
    }
    /// @brief Get the blending_map's divisor
    T get_divisor () const { return divisor_; }
    /// @brief Set the blending_map's divisor
    /// @param divisor The divisor
    void set_divisor (T divisor)
    {
        divisor_ = divisor;
        init_blending_region_rects ();
    }
    /// @brief Get the blending_map's bitmap
    const B &get_bitmap () const { return bitmap_; }
    /// @brief Set the blending_map's bitmap
    /// @param bitmap A bitmap containing pyramid levels
    ///
    /// The fixation point will be aligned with the center
    /// of the bitmap.
    void set_bitmap (const B &bitmap)
    {
        bitmap_ = bitmap;
        init_blending_region_rects ();
    }
    /// @brief Set the fixation point
    /// @param fx Fixation x coordinate
    /// @param fy Fixation y coordinate
    /// @see blending_map::set_fixation
    void do_set_fixation (T fx, T fy)
    { fx_ = fx; fy_ = fy; }
    /// @brief Get blending regions
    /// @param level Image pyramid level
    /// @return Blending regions
    /// @see get_blending_level
    std::vector<rect<T> > do_get_blending_regions (size_t level) const
    {
        // If the level is too low resolution, return an
        // empty vector
        if (level >= blending_region_rects.size ())
            return std::vector<rect<T> > ();
        // Otherwise, translate the rect by the fixation
        rect<T> r (blending_region_rects[level]);
        r.set_x (static_cast<T> (round (r.get_x () + fx_)));
        r.set_y (static_cast<T> (round (r.get_y () + fy_)));
        return std::vector<rect<T> > (1, r);
    }
    /// @brief Get the blending level given a superpixel
    /// @param s The superpixel
    /// @return The blending level
    T do_get_blending_level (const superpixel &s) const
    {
        T cx = s.get_col () + s.get_size () / 2.0;
        T cy = s.get_row () + s.get_size () / 2.0;
        return do_get_blending_level (cx, cy);
    }
    private:
    /// @brief Get the pyramid blending level at a point
    /// @param x x coordinate
    /// @param y y coordinate
    T do_get_blending_level (T x, T y) const
    {
        // Make x, y relative to the fixation
        x -= fx_ + bitmap_.cols () / 2;
        y -= fy_ + bitmap_.rows () / 2;
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        if (x >= bitmap_.cols ())
            x = bitmap_.cols () - 1;
        if (y >= bitmap_.rows ())
            y = bitmap_.rows () - 1;
        return bitmap_ (static_cast<size_t> (round (y))
            , static_cast<size_t> (round (x))) / divisor_;
    }
    /// @brief Helper for determining region rects
    void init_blending_region_rects ()
    {
        blending_region_rects.clear ();
        // There are no region rects if there is no bitmap
        if (bitmap_.empty ())
            return;
        // Find the highest level in pyramid
        const T max_level = *max_element (bitmap_.begin (), bitmap_.end ()) / divisor_;
        size_t levels = static_cast<size_t> (floor (max_level + 1));
        blending_region_rects.resize (levels);
        // Iterate over the entire bitmap, and find the
        // bounding rectangles that includes all pixels
        // in the range [level,level+1]
        std::vector<T> min_r (levels, bitmap_.rows ());
        std::vector<T> min_c (levels, bitmap_.cols ());
        std::vector<T> max_r (levels, -1);
        std::vector<T> max_c (levels, -1);
        for (size_t i = 0; i < bitmap_.rows (); ++i)
        {
            for (size_t j = 0; j < bitmap_.cols (); ++j)
            {
                const T current_level = bitmap_ (i, j) / divisor_;
                for (size_t l = 0; l < levels; ++l)
                {
                    if (current_level >= l && current_level < l + 1)
                    {
                        min_r[l] = (std::min<T>) (i, min_r[l]);
                        min_c[l] = (std::min<T>) (j, min_c[l]);
                        max_r[l] = (std::max<T>) (i, max_r[l]);
                        max_c[l] = (std::max<T>) (j, max_c[l]);
                    }
                }
            }
        }
        // Get the center of the bitmap
        T cx = bitmap_.cols () / 2.0;
        T cy = bitmap_.rows () / 2.0;
        // Now set the rects
        for (size_t l = 0; l < levels; ++l)
        {
            // If there was one pixel at this level
            if (max_r[l] != -1)
            {
                // Set it
                assert (min_r[l] <= max_r[l]);
                assert (min_c[l] <= max_c[l]);
                blending_region_rects[l] = rect<T> (
                    min_c[l] - cx, // x,y
                    min_r[l] - cy,
                    max_c[l] - min_c[l] + 1, // w,h
                    max_r[l] - min_r[l] + 1);
            }
            // Otherwise it remains an empty rect
        }
    }
    // Properties
    T divisor_; // bitmap divisor
    T fx_; // fixation
    T fy_;
    B bitmap_; // bitmap
    // This implementation only allows one rect per level,
    // but you could have more.  The interface allows for
    // multiple rects.
    std::vector<rect<T> > blending_region_rects;
};

} // namespace horny_toad

#endif // BLENDING_MAP_HPP
