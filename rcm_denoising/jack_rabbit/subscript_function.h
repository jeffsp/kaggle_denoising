/// @file subscript_function.h
/// @brief functions that use raster subscripts
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef SUBSCRIPT_FUNCTION_HPP
#define SUBSCRIPT_FUNCTION_HPP

#include "subregion.h"
#include <functional>

namespace jack_rabbit
{

/// @brief Subscript function base class
template<typename T,template<typename> class F>
class subscript_function : public F<T>
{
    // Limit access to constructors
    protected:
    /// @brief Constructor
    /// @param rows number of subscript rows
    /// @param cols number of subscript columns
    subscript_function (size_t rows, size_t cols)
        : F<T> (rows, cols),
        start_row_ (0), start_col_ (0),
        end_row_ (rows), end_col_ (cols),
        row_ (start_row_), col_ (start_col_) { }
    /// @brief Constructor
    /// @param s raster subregion
    subscript_function (const subregion &s)
        : F<T> (s.rows, s.cols),
        start_row_ (s.r), start_col_ (s.c),
        end_row_ (s.r + s.rows), end_col_ (s.c + s.cols),
        row_ (start_row_), col_ (start_col_) { }
    protected:
    /// @brief Move current subscript one element to the right
    void update_coord () const
    {
        ++col_;
        if (col_ == end_col_)
        {
            col_ = start_col_;
            ++row_;
            if (row_ == end_row_)
                row_ = start_row_;
        }
    }
    private:
    size_t start_row_, start_col_;
    size_t end_row_, end_col_;
    protected:
    /// @brief The current subscript
    mutable size_t row_;
    /// @brief The current subscript
    mutable size_t col_;
};

/// @brief Generating subscript function
template<typename T,template<typename> class F>
class subscript_generator : public subscript_function<T,F>
{
    public:
    /// @brief Constructor
    /// @param rows number of subscript rows
    /// @param cols number of subscript columns
    subscript_generator (size_t rows, size_t cols)
        : subscript_function<T,F> (rows, cols) { }
    /// @brief Constructor
    /// @param s raster subregion
    subscript_generator (const subregion &s)
        : subscript_function<T,F> (s) { }
    /// @brief Function call operator
    T operator() ()
    {
        // Use CRTP for static polymorphism
        T ret = static_cast<F<T> *> (this)->operator() (
            subscript_function<T,F>::row_,
            subscript_function<T,F>::col_);
        subscript_function<T,F>::update_coord ();
        return ret;
    }
};

/// @brief Unary subscript function
template<typename T,template<typename> class F>
class subscript_unary_function : public subscript_function<T,F>,
    public std::unary_function<T,T>
{
    public:
    /// @brief Constructor
    /// @param rows number of subscript rows
    /// @param cols number of subscript columns
    subscript_unary_function (size_t rows, size_t cols)
        : subscript_function<T,F> (rows, cols) { }
    /// @brief Constructor
    /// @param s raster subregion
    subscript_unary_function (const subregion &s)
        : subscript_function<T,F> (s) { }
    /// @brief Unary function operator
    T operator() (const T &u)
    {
        // Use CRTP for static polymorphism
        T ret = static_cast<F<T> *> (this)->operator() (
            subscript_function<T,F>::row_,
            subscript_function<T,F>::col_,
            u);
        subscript_function<T,F>::update_coord ();
        return ret;
    }
};

/// @brief Binary subscript function
template<typename T,template<typename> class F>
class subscript_binary_function : public subscript_function<T,F>,
    public std::binary_function<T,T,T>
{
    public:
    /// @brief Constructor
    /// @param rows number of subscript rows
    /// @param cols number of subscript columns
    subscript_binary_function (size_t rows, size_t cols)
        : subscript_function<T,F> (rows, cols) { }
    /// @brief Constructor
    /// @param s raster subregion
    subscript_binary_function (const subregion &s)
        : subscript_function<T,F> (s) { }
    /// @brief Binary function operator
    T operator() (const T &u, const T &v)
    {
        // Use CRTP for static polymorphism
        T ret = static_cast<F<T> *> (this)->operator() (
            subscript_function<T,F>::row_,
            subscript_function<T,F>::col_,
            u, v);
        subscript_function<T,F>::update_coord ();
        return ret;
    }
};

} // namespace jack_rabbit

#endif // SUBSCRIPT_FUNCTION_HPP
