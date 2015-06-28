/// @file subregion_iter.h
/// @brief subregion iterator
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef SUBREGION_ITER_HPP
#define SUBREGION_ITER_HPP

#include "subregion.h"
#include <iterator>

namespace jack_rabbit
{

/// @brief General subregion iterator traits
template<typename C>
struct subregion_iterator_traits
{
    //@{ @brief Traits type definitions
    typedef std::forward_iterator_tag iterator_category;
    typedef typename C::value_type value_type;
    typedef typename C::size_type size_type;
    typedef typename C::difference_type difference_type;
    typedef typename C::pointer pointer;
    typedef typename C::reference reference;
    //@}
};

/// @brief Subregion iterator traits specialization for
/// const containers
template<typename C>
struct subregion_iterator_traits<const C>
{
    //@{ @brief Traits type definitions
    typedef std::forward_iterator_tag iterator_category;
    typedef typename C::value_type value_type;
    typedef typename C::size_type size_type;
    typedef typename C::difference_type difference_type;
    typedef typename C::const_pointer pointer;
    typedef typename C::const_reference reference;
    //@}
};

/// @brief raster subregion iterator
///
/// Iterates over a raster-like container in a particular
/// subregion.
template<class T>
class subregion_iter
{
    public:
    //@{ @brief Iterator type definitions
    typedef typename subregion_iterator_traits<T>::iterator_category iterator_category;
    typedef typename subregion_iterator_traits<T>::value_type value_type;
    typedef typename subregion_iterator_traits<T>::size_type size_type;
    typedef typename subregion_iterator_traits<T>::difference_type difference_type;
    typedef typename subregion_iterator_traits<T>::pointer pointer;
    typedef typename subregion_iterator_traits<T>::reference reference;
    //@}

    /// @brief Get the beginning of a subregion
    /// @param p pointer to the beginning of a row in a
    /// subregion
    /// @param m_cols number of columns in the raster
    /// @param s_cols number of columns in the subregion
    subregion_iter (pointer p = 0, size_type m_cols = 0, size_type s_cols = 0)
        : row_begin_ (p),
        row_current_ (p),
        row_end_ (p + s_cols),
        m_cols_ (m_cols),
        s_cols_ (s_cols)
    { }
    /// @brief Element dereference
    reference operator* () const
    { return *row_current_; }
    /// @brief Member read access
    pointer operator->() const
    { return row_current_; }
    /// @brief Prefix ++ operator
    subregion_iter<T> &operator++ ()
    {
        ++row_current_;
        // Are we at the end of a subregion row?
        if (row_current_== row_end_)
        {
            // Go down one raster row
            row_begin_ = row_begin_ + m_cols_;
            row_current_ = row_begin_;
            row_end_ = row_begin_ + s_cols_;
        }
        return *this;
    }
    /// @brief Postfix ++ operator
    subregion_iter<T> operator++ (int)
    {
        subregion_iter<T> tmp (*this);
        ++(*this);
        return tmp;
    }
    /// @brief Comparison
    bool operator== (const subregion_iter<T> &rhs) const
    { return row_current_ == rhs.row_current_; }
    /// @brief Comparison
    bool operator!= (const subregion_iter<T> &rhs) const
    { return row_current_ != rhs.row_current_; }

    /// @brief Row access
    ///
    /// Get a pointer to the first element in the first row
    /// in a raster.  Use this function to iterate over a
    /// raster row by row.
    pointer row_begin () const
    { return row_begin_; }
    /// @brief Row access
    ///
    /// Get a pointer to one past the last element in the
    /// last row in a raster.  Use this function to iterate
    /// over a raster row by row.
    pointer row_end () const
    { return row_end_; }
    /// @brief Go to next row
    ///
    /// Increment the pointer returned by row_begin() so
    /// that it points to the next row.  Use this function
    /// to iterate over a raster row by row.
    subregion_iter<T> next_row ()
    {
        row_begin_ = row_begin_ + m_cols_;
        row_current_ = row_begin_;
        row_end_ = row_begin_ + s_cols_;
        return *this;
    }

    private:
    pointer row_begin_;
    pointer row_current_;
    pointer row_end_;
    size_type m_cols_;
    size_type s_cols_;
};

} // namespace jack_rabbit

#endif // SUBREGION_ITER_HPP
