/// @file foveator.h
/// @brief foveator
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef FOVEATOR_HPP
#define FOVEATOR_HPP

#include "blending_map.h"
#include "pyramid.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <set>
#include <vector>

namespace horny_toad
{

/// @brief Kernel policy for image foveator
///
/// The expand operator used in this policy produces plus
/// sign artifacts in the very low resolution levels.
template<typename P> // Pyramid type
class kernel3x3
{
    public:
    /// @brief Reduce a pyramid
    /// @param p Pyramid
    static void reduce (P &p)
    { p.reduce3x3 (); }
    /// @brief Expand a pyramid level
    /// @param p Pyramid
    /// @param level Pyramid level to expand
    static void expand (P &p, size_t level)
    { p.expand3x3 (level); }
};

/// @brief Kernel policy for image foveator
///
/// The expand operator used in this policy produces block
/// artifacts.
template<typename P> // Pyramid type
class kernel2x2
{
    public:
    /// @brief Reduce a pyramid
    /// @param p Pyramid
    static void reduce (P &p)
    { p.reduce2x2 (); }
    /// @brief Expand a pyramid level
    /// @param p Pyramid
    /// @param level Pyramid level to expand
    static void expand (P &p, size_t level)
    { p.expand2x2 (level); }
};

/// @brief Kernel policy for image foveator
///
/// The expand operator does not produce plus sign
/// artifacts.
template<typename P> // Pyramid type
class kernel2x2_with_blur
{
    public:
    /// @brief Reduce a pyramid
    /// @param p Pyramid
    static void reduce (P &p)
    { p.reduce2x2 (); }
    /// @brief Expand a pyramid level
    /// @param p Pyramid
    /// @param level Pyramid level to expand
    static void expand (P &p, size_t level)
    { p.expand2x2_and_blur3x3 (level); }
};

/// @brief An image foveator
/// @param T image pixels
/// @param K kernel policy
/// @see kernel2x2,kernel3x3,kernel2x2_with_blur
///
/// Vary the spatial resolution across an image given a
/// point of gaze.
///
/// The kernel policy affects how the image is
/// blurred.
template<
    typename T,
    template <typename> class K
    >
class foveator
{
    public:
    //@{
    typedef T pixel_type;
    typedef jack_rabbit::raster<T> image_type;
    typedef blending_map<float> blending_map_type;
    typedef horny_toad::pyramid<T> pyramid_type;
    //@}
    /// @brief Constructor
    foveator (size_t rows = 0,
        size_t cols = 0,
        blending_map_type *b = 0)
        : blending_map_ (b)
    {
        resize (rows, cols);
    }
    /// @brief Set the image size
    /// @param rows number of image rows
    /// @param cols number of image columns
    void resize (size_t rows, size_t cols)
    {
        source_pyramid_.resize (rows, cols);
        foveated_pyramid_.resize (rows, cols);
    }
    /// @brief Get the pyramid containing the source image
    /// @return The source pyramid
    ///
    /// This function provides read access to the source
    /// images.
    const pyramid_type &get_source_pyramid () const
    {
        return source_pyramid_;
    }
    /// @brief Set the source image pixels
    /// @param beg Pointer to start of source pixels
    /// @param end Pointer to end of source pixels
    void set_source_pixels (typename image_type::const_iterator beg,
        typename image_type::const_iterator end)
    {
        assert (static_cast<size_t> (end - beg) == source_pyramid_[0].size ());
        copy (beg, end, source_pyramid_[0].begin ());
        K<pyramid_type>::reduce (source_pyramid_);
    }
    /// @brief Preload foveated pyramid
    /// @param last_level Last level to preload
    ///
    /// Preload the foveated pyramid when you want to do
    /// progressive encoding.
    void preload (unsigned last_level)
    {
        //foveated_pyramid_.top () = source_pyramid_.top ();
        for (size_t level = source_pyramid_.levels () - 1; level > 0; --level)
        {
            if (level >= last_level)
                foveated_pyramid_[level] = source_pyramid_[level];
            // Expand to level-1
            K<pyramid_type>::expand (foveated_pyramid_, level);
        }
    }
    /// @brief Get the pyramid containing the foveated image
    /// @return The foveated pyramid
    ///
    /// This function provides read access to the processed
    /// images.
    const pyramid_type &get_foveated_pyramid () const
    {
        return foveated_pyramid_;
    }
    /// @brief Set the blending map
    /// @param b the map used to foveate
    ///
    /// You must set a blending map in order to foveate an
    /// image.
    void set_blending_map (const blending_map_type *b)
    {
        blending_map_ = b;
    }
    /// @brief Foveate the image
    /// @param progressive Flag for progressive encoding
    /// @return The total number of encoded pixels
    /// @see preload
    ///
    /// For progressive foveation, you will need to preload
    /// the foveation pyramid and set the preogressive flag
    /// on this routine so that high resolution portions of
    /// the image are not overwritten.  This only works when
    /// you are foveating a static image.
    size_t foveate (bool progressive = false)
    {
        assert (source_pyramid_.levels () == foveated_pyramid_.levels ());
        assert (source_pyramid_.levels () != 0);
        if (blending_map_ == 0)
            return 0; // nothing to do
        size_t total_pixels = 0;
        // Always copy the lowest resolution image
        foveated_pyramid_.top () = source_pyramid_.top ();
        total_pixels += foveated_pyramid_.top ().rows () * foveated_pyramid_.top ().cols ();
        for (size_t level = source_pyramid_.levels () - 1; level > 0; --level)
        {
            // Expand to level-1
            if (!progressive)
                K<pyramid_type>::expand (foveated_pyramid_, level);
            // Now blend them
            assert (blending_map_ != 0);
            assert (level < source_pyramid_.levels ());
            assert (source_pyramid_[level].size () == foveated_pyramid_[level].size ());
            // Ask the blending map where we should blend
            std::vector<rect<float> > blending_regions =
                blending_map_->get_blending_regions (level - 1);
            // Convert blending regions to pyramid
            // coordinates
            std::vector<rect<size_t> > pyramid_rects =
                get_pyramid_rects (level - 1, blending_regions);
            for (size_t i = 0; i < pyramid_rects.size (); ++i)
            {
                blend_rect (level - 1, pyramid_rects[i]);
                total_pixels += pyramid_rects[i].get_width () * pyramid_rects[i].get_height ();
            }
        }
        return total_pixels;
    }
    private:
    // Convert a blending region to a pyramid rect
    std::vector<rect<size_t> > get_pyramid_rects (
        size_t level,
        std::vector<rect<float> > &blending_regions) const
    {
        assert (level < source_pyramid_.levels ());
        assert (source_pyramid_[level].size () == foveated_pyramid_[level].size ());
        std::vector<rect<size_t> > pyramid_rects;
        for (size_t i = 0; i < blending_regions.size (); ++i)
        {
            rect<float> b = blending_regions[i];
            // NOTE: You have to clip after you quantize and
            // scale because the pyramid rounds image sizes
            // UP when it downsamples.  If you clip first
            // and then rescale, you will get the coords
            // wrong.
            int r1 = static_cast<int> (floor (b.get_y ()));
            int r2 = static_cast<int> (ceil (b.get_y () + b.get_height ()));
            int c1 = static_cast<int> (floor (b.get_x ()));
            int c2 = static_cast<int> (ceil (b.get_x () + b.get_width ()));
            // Scale it
            r1 >>= level;
            r2 >>= level;
            c1 >>= level;
            c2 >>= level;
            assert (r2 >= r1);
            assert (c2 >= c1);
            rect<int> r (c1, r1, c2 - c1, r2 - r1);
            // Clip it so that it lies within the image
            r.clip (rect<int> (0, 0, source_pyramid_[level].cols (), source_pyramid_[level].rows ()));
            if (!r.empty ())
                pyramid_rects.push_back (rect<size_t> (r.get_x (), r.get_y (), r.get_width (), r.get_height ()));
        }
        return pyramid_rects;
    }
    // Blend a pyramid rect
    void blend_rect (size_t level, const rect<size_t> &r)
    {
        size_t r1 = r.get_y ();
        size_t r2 = r.get_y () + r.get_height ();
        // For each pyramid pixel in this rect
        for (size_t row = r1; row < r2; ++row)
        {
            size_t c1 = r.get_x ();
            size_t c2 = r.get_x () + r.get_width ();
            // Break it up into superpixels
            for (size_t col = c1; col < c2; ++col)
            {
                // Scale it up to image coordinates
                superpixel s (row << level, col << level, 1 << level);
                float blending_level = blending_map_->get_blending_level (s);
                blend_pixels (blending_level, level, row, col);
            }
        }
    }
    // Blend two pixels together
    void blend_pixels (float blending_level, size_t level, size_t r, size_t c)
    {
        assert (source_pyramid_[level].index (r, c) < source_pyramid_[level].size ());
        assert (foveated_pyramid_[level].index (r, c) < foveated_pyramid_[level].size ());
        // Is the resolution higher than this level?
        if (blending_level < level)
        {
            // Copy it so that higher resolution
            // levels may use this pixel to
            // blend...
            foveated_pyramid_[level] (r, c) = source_pyramid_[level] (r, c);
        }
        // Is the resolution between this level
        // and the next?
        else if (blending_level < level + 1)
        {
            // Linearly interpolate to get the pixel value
            float weight = blending_level - level;
            pixel_type lores_pixel = foveated_pyramid_[level] (r, c);
            pixel_type hires_pixel = source_pyramid_[level] (r, c);
            pixel_type p = static_cast<pixel_type> (weight * lores_pixel + (1 - weight) * hires_pixel);
            foveated_pyramid_[level] (r, c) = p;
        }
        else
        // Is the resolution lower than this level?
        {
            // Do nothing because it has already
            // been blended...
        }
    }
    pyramid_type source_pyramid_;
    pyramid_type foveated_pyramid_;
    const blending_map_type *blending_map_;
};

} // namespace horny_toad

#endif // FOVEATOR_HPP
