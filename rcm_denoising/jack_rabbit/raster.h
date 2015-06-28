/// @file raster.h
/// @brief raster container adapter
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef RASTER_HPP
#define RASTER_HPP

#include "subregion_iter.h"
#include <stdexcept>
#include <vector>

/// @brief Jack rabbit image processing utilities
namespace jack_rabbit
{
/// \example raster_example0.cc
/// This is a simple example of how to use the raster class.

/// \example raster_example1.cc
/// This is a detailed example of using subregion iterators
/// and raster function objects in order to create a raster
/// that contains an image:
/// \image html raster_example1.png "raster example1 output"

/// \example raster_example2.cc
/// This example uses a custom allocator in order to write
/// a colorful display to a simulated display buffer using
/// the raster interface:
/// \image html raster_example2.png "raster example2 output"

/// \example test_raster.cc
/// Raster test suite

/// @brief raster container adapter
///
/// This is a simple container that provides 2D
/// subscripting, STL compatibility, and subregion
/// iteration.
template<typename T, class Cont = std::vector<T> >
class raster
{
    public:
    //@{
    typedef raster<T,Cont> self_type;
    typedef Cont container_type;
    typedef typename Cont::value_type value_type;
    typedef typename Cont::pointer pointer;
    typedef typename Cont::const_pointer const_pointer;
    typedef typename Cont::reference reference;
    typedef typename Cont::const_reference const_reference;
    typedef typename Cont::size_type size_type;
    typedef typename Cont::difference_type difference_type;
    typedef typename Cont::allocator_type allocator_type;
    typedef typename Cont::iterator iterator;
    typedef typename Cont::const_iterator const_iterator;
    typedef typename Cont::reverse_iterator reverse_iterator;
    typedef typename Cont::const_reverse_iterator const_reverse_iterator;
    typedef subregion_iter<self_type> subregion_iterator;
    typedef const subregion_iter<const self_type> const_subregion_iterator;
    //@}

    /// @brief Default constructor
    /// @param a optional custom allocator
    explicit raster (const allocator_type &a = allocator_type ())
        : rows_ (0), cols_ (0), cont_ (a)
    { }
    /// @brief Size constructor
    /// @param rows number of rows in raster
    /// @param cols number of columns in raster
    /// @param v initialization value
    /// @param a optional custom allocator
    raster (size_type rows, size_type cols, const T &v = T (),
        const allocator_type &a = allocator_type ())
        : rows_ (rows), cols_ (cols), cont_ (rows * cols, v, a)
    { }
    /// @brief Size constructor
    /// @param sz number of rows in raster (cols will equal 1)
    /// @param a optional custom allocator
    explicit raster (size_type sz,
        const allocator_type &a = allocator_type ())
        : rows_ (sz), cols_ (1), cont_ (sz, T (), a)
    { }
    /// @brief Copy constructor
    /// @param rows number of rows in c
    /// @param cols number of columns in c
    /// @param c container to copy
    ///
    /// The container will be resized if its size is not equal to rows
    /// * cols.
    raster (size_type rows, size_type cols, const Cont &c)
        : rows_ (rows), cols_ (cols), cont_ (c)
    { cont_.resize (rows_ * cols_); }
    /// @brief Copy constructor
    /// @param m raster to copy
    raster (const raster<T,Cont> &m)
        : rows_ (m.rows_), cols_ (m.cols_), cont_ (m.cont_)
    { }
    /// @brief Copy constructor
    /// @param m raster to copy
    template<typename M,class C>
    raster (const raster<M,C> &m)
        : rows_ (m.rows ()), cols_ (m.cols ()), cont_ (m.size ())
    {
        typename raster<T,Cont>::iterator dest = this->begin ();
        typename raster<M,C>::const_iterator src = m.begin ();
        const typename raster<M,C>::const_iterator src_end = m.end ();
        while (src != src_end)
            *dest++ = *src++;
    }
    /// @brief Destructor
    ~raster ()
    { }

    /// @brief Get dimensions
    /// @return the number of rows
    size_type rows () const
    { return rows_; }
    /// @brief Get dimensions
    /// @return the number of cols
    size_type cols () const
    { return cols_; }
    /// @brief Get number of elements in the raster
    /// @return the total number of elements in the raster
    size_type size () const
    { return cont_.size (); }
    /// @brief Get maximum possible elements in the raster
    /// @return the total possible number of elements in the raster
    size_type max_size () const
    { return cont_.max_size (); }
    /// @brief Get currently allocated number of elements in the raster
    /// @return the number of elements allocated by the
    /// raster
    size_type capacity () const
    { return cont_.capacity (); }
    /// @brief Reserve space for more elements
    /// @param n number of elements to reserve
    void reserve (size_type n)
    { cont_.reserve (n); }
    /// @brief Reserve space for more elements
    /// @param rows number of rows to reserve
    /// @param cols number of cols to reserve
    void reserve (size_type rows, size_type cols)
    { cont_.reserve (rows * cols); }
    /// @brief Indicates if the raster has a zero dimension
    /// @return true if the raster is empty, otherwise false
    bool empty() const
    { return cont_.empty (); }

    /// @brief Swap this raster with another
    void swap (raster<T,Cont> &m)
    {
        std::swap (rows_, m.rows_);
        std::swap (cols_, m.cols_);
        std::swap (cont_, m.cont_);
    }
    /// @brief Copy assignment
    raster<T,Cont> &operator= (const raster<T,Cont> &rhs)
    {
        if (this != &rhs)
        {
            raster<T,Cont> tmp (rhs);
            swap (tmp);
        }
        return *this;
    }
    /// @brief Assign all element values
    /// @param v value to assign
    void assign (const T &v)
    { cont_.assign (cont_.size (), v); }

    /// @brief Element access
    reference front ()
    { return cont_.front (); }
    /// @brief Element access
    const_reference front () const
    { return cont_.front (); }
    /// @brief Element access
    reference back ()
    { return cont_.back (); }
    /// @brief Element access
    const_reference back () const
    { return cont_.back (); }

    /// @brief Random access
    /// @param i element index
    reference operator[] (size_type i)
    { return *(cont_.begin () + i); }
    /// @brief Random access
    /// @param i element index
    const_reference operator[] (size_type i) const
    { return *(cont_.begin () + i); }
    /// @brief Random access
    /// @param r element row
    /// @param c element col
    reference operator() (size_type r, size_type c)
    { return *(cont_.begin () + index (r, c)); }
    /// @brief Random access
    /// @param r element row
    /// @param c element col
    const_reference operator() (size_type r, size_type c) const
    { return *(cont_.begin () + index (r, c)); }
    /// @brief Checked random access
    /// @param r element row
    /// @param c element col
    ///
    /// Throws if the subscript is invalid.
    reference at (size_type r, size_type c)
    { return cont_.at (index (r, c)); }
    /// @brief Checked random access
    /// @param r element row
    /// @param c element col
    ///
    /// Throws if the subscript is invalid.
    const_reference at (size_type r, size_type c) const
    { return cont_.at (index (r, c)); }

    /// @brief Iterator access
    iterator begin ()
    { return cont_.begin (); }
    /// @brief Iterator access
    const_iterator begin () const
    { return cont_.begin (); }
    /// @brief Iterator access
    iterator end ()
    { return cont_.end (); }
    /// @brief Iterator access
    const_iterator end () const
    { return cont_.end (); }
    /// @brief Iterator access
    reverse_iterator rbegin ()
    { return cont_.rbegin (); }
    /// @brief Iterator access
    const_reverse_iterator rbegin () const
    { return cont_.rbegin (); }
    /// @brief Iterator access
    reverse_iterator rend ()
    { return cont_.rend (); }
    /// @brief Iterator access
    const_reverse_iterator rend () const
    { return cont_.rend (); }

    /// @brief Get an iterator given a subscript
    /// @param r element row
    /// @param c element col
    iterator loc (size_type r, size_type c)
    { return begin () + index (r, c); }
    /// @brief Get an element index given its subscripts
    /// @param r element row
    /// @param c element col
    size_type index (size_type r, size_type c) const
    { return r * cols_ + c; }
    /// @brief Get an iterator's row subscript
    /// @param loc iterator
    size_type row (const_iterator loc) const
    { return (loc - begin ()) / cols_; }
    /// @brief Get an iterator's column subscript
    /// @param loc iterator
    size_type col (const_iterator loc) const
    { return (loc - begin ()) % cols_; }
    /// @brief Get a raster subregion
    /// @param r start row of subregion
    /// @param c start column of subregion
    /// @param rows number of rows in the subregion
    /// @param cols number of cols in the subregion
    ///
    /// The subregion will be adjusted, if necessary, to ensure that
    /// it does not specify an area outside of the raster.
    subregion sub (int r,
        int c,
        size_type rows,
        size_type cols) const
    {
        subregion s;
        s.r = (std::max<int>) (r, 0);
        s.c = (std::max<int>) (c, 0);
        rows -= (s.r - r);
        cols -= (s.c - c);
        if (s.r > this->rows ())
            s.rows = 0;
        else if (s.r + rows > this->rows ())
            s.rows = this->rows () - s.r;
        else
            s.rows = rows;
        if (s.c > this->cols ())
            s.cols = 0;
        else if (s.c + cols > this->cols ())
            s.cols = this->cols () - s.c;
        else
            s.cols = cols;
        return s;
    }

    /// @brief Special iterator access
    subregion_iterator begin (const subregion &s)
    { return subregion_iterator (&*begin () + index (s.r, s.c), cols (), s.cols); }
    /// @brief Special iterator access
    const_subregion_iterator begin (const subregion &s) const
    { return const_subregion_iterator (&*begin () + index (s.r, s.c), cols (), s.cols); }
    /// @brief Special iterator access
    subregion_iterator end (const subregion &s)
    { return subregion_iterator (&*begin () + index (s.r + s.rows, s.c), cols (), s.cols); }
    /// @brief Special iterator access
    const_subregion_iterator end (const subregion &s) const
    { return const_subregion_iterator (&*begin () + index (s.r + s.rows, s.c), cols (), s.cols); }

    /// @brief Change dimensions
    /// @param rows new number of rows
    /// @param cols new number of columns
    void resize (size_type rows, size_type cols)
    { resize (rows, cols, T ()); }
    /// @brief Change dimensions
    /// @param rows new number of rows
    /// @param cols new number of columns
    /// @param v fill new elements, if any, with this value
    void resize (size_type rows, size_type cols, const T &v)
    {
        // Don't alloc and swap unnecessarily
        if (rows == rows_ && cols == cols_)
            return;
        raster<T,Cont> tmp (rows, cols, v);
        size_type min_rows = (std::min) (rows, rows_);
        size_type min_cols = (std::min) (cols, cols_);
        for (size_type r = 0; r < min_rows; ++r)
            std::copy (begin () + index (r, 0),
                begin () + index (r, min_cols),
                tmp.begin () + tmp.index (r, 0));
        swap (tmp);
    }
    /// @brief Insert one or more rows
    /// @param loc insert rows before the element in this row
    /// @param n number of rows to insert
    /// @param v fill new elements with this value
    /// @return an iterator for the first inserted element
    ///
    /// If loc refers to an element past the last raster element, rows
    /// are inserted after the last row.
    iterator insert_rows (iterator loc, size_type n, const T &v)
    {
        size_type r = row (loc);
        if (loc >= end ())
            r = rows_;
        raster<T,Cont> tmp (rows_ + n, cols_, v);
        // copy elems before inserted rows
        std::copy (begin () + index (0, 0),
            begin () + index (r, 0),
            tmp.begin () + tmp.index (0, 0));
        // copy elems after inserted rows
        std::copy (begin () + index (r, 0),
            begin () + index (rows_, 0),
            tmp.begin () + tmp.index (r + n, 0));
        // commit the change
        swap (tmp);
        return begin () + index (r, 0);
    }
    /// @brief Insert one or more columns
    /// @param loc insert columns before the element in this column
    /// @param n number of columns to insert
    /// @param v fill new elements with this value
    ///
    /// If loc refers to an element past the last raster element,
    /// columns are inserted after the last column.
    iterator insert_cols (iterator loc, size_type n, const T &v)
    {
        size_type c = col (loc);
        if (loc >= end ())
            c = cols_;
        raster<T,Cont> tmp (rows_, cols_ + n, v);
        for (size_type r = 0; r < rows_; ++r)
        {
            // copy elems before inserted columns
            std::copy (begin () + index (r, 0),
                begin () + index (r, c),
                tmp.begin () + tmp.index (r, 0));
            // copy elems after inserted columns
            std::copy (begin () + index (r, c),
                begin () + index (r, cols_),
                tmp.begin () + tmp.index (r, c + n));
        }
        // commit the change
        swap (tmp);
        return begin () + index (0, c);
    }
    /// @brief Erase a row
    /// @param loc erase the row that contains this element
    /// @return an iterator for the element after the last erased
    /// element
    ///
    /// If loc refers to an element past the last raster element, no
    /// rows are erased, and the return value is end().
    iterator erase_row (iterator loc)
    {
        if (loc >= end ())
            return end ();
        size_type r = row (loc);
        raster<T,Cont> tmp (rows_ - 1, cols_);
        // copy elems before erased rows
        std::copy (begin () + index (0, 0),
            begin () + index (r, 0),
            tmp.begin () + tmp.index (0, 0));
        // copy elems after erased rows
        std::copy (begin () + index (r + 1, 0),
            begin () + index (rows_, 0),
            tmp.begin () + tmp.index (r, 0));
        // commit the change
        swap (tmp);
        return begin () + index (r, 0);
    }
    /// @brief Erase a column
    /// @param loc erase the column that contains this element
    /// @return an iterator for the element after the last erased
    /// element
    ///
    /// If loc refers to an element past the last raster element, no
    /// columns are erased, and the return value is end().
    iterator erase_col (iterator loc)
    {
        if (loc >= end ())
            return end ();
        size_type c = col (loc);
        raster<T,Cont> tmp (rows_, cols_ - 1);
        for (size_type r = 0; r < rows_; ++r)
        {
            // copy elems before erased rows
            std::copy (begin () + index (r, 0),
                begin () + index (r, c),
                tmp.begin () + tmp.index (r, 0));
            // copy elems after erased rows
            std::copy (begin () + index (r, c + 1),
                begin () + index (r, cols_),
                tmp.begin () + tmp.index (r, c));
        }
        // commit the change
        swap (tmp);
        return begin () + index (0, c);
    }
    /// @brief Remove all elements
    void clear ()
    {
        raster<T,Cont> tmp;
        swap (tmp);
    }

    /// @brief Compare two rasters
    template<typename M,typename C>
    friend bool operator== (const raster<M,C> &a, const raster<M,C> &b);

    private:
    size_type rows_;
    size_type cols_;
    Cont cont_;
};

/// @brief Compare two rasters
template<typename T,typename Cont>
inline bool operator== (const raster<T,Cont> &a, const raster<T,Cont> &b)
{
    return a.rows_ == b.rows_ &&
        a.cols_ == b.cols_ &&
        a.cont_ == b.cont_;
}

/// @brief Compare two rasters
template<typename T,typename Cont>
inline bool operator!= (const raster<T,Cont> &a, const raster<T,Cont> &b)
{
    return !(a == b);
}

} // namespace jack_rabbit

#endif // RASTER_HPP
