/// @file test_raster.cc
/// @brief test raster functionality
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#include "horny_toad/verify.h"
#include "jack_rabbit/jack_rabbit.h"
#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>

using namespace horny_toad;
using namespace jack_rabbit;
using namespace std;

template<typename T,size_t ROWS,size_t COLS>
void test_constructors ()
{
    // default ctor
    raster<T> a;
    VERIFY (a.empty ());
    // size ctor
    raster<T> b (ROWS, COLS);
    VERIFY (b.back () == T ());
    VERIFY (b.front () == T ());
    // size ctor
    raster<T> c (ROWS, COLS, T (1));
    VERIFY (c.back () == T (1));
    VERIFY (c.front () == T (1));
    // copy ctor
    raster<T> d (c);
    VERIFY (d.back () == T (1));
    VERIFY (d.front () == T (1));
    // copy ctor
    vector<T> v (ROWS * COLS + 1);
    raster<T> e (ROWS, COLS, v);
    VERIFY (e.size () == ROWS * COLS);
    // copy ctor
    raster<T> f (ROWS, COLS, 9);
    raster<double> g (f);
    VERIFY (g.size () == ROWS * COLS);
    VERIFY (g.front () == 9);
    VERIFY (g.back () == 9);
    raster<T> h (ROWS*COLS);
    VERIFY (h.size () == ROWS*COLS);
    VERIFY (h.rows () == ROWS*COLS);
    VERIFY (h.cols () == 1);
}

template<typename T,size_t ROWS,size_t COLS>
void test_const_ops ()
{
    raster<T> a;
    VERIFY (a.empty ());
    raster<T> b (ROWS, COLS);
    // size, dimensions, capacity, reserve
    VERIFY (b.size () == ROWS * COLS);
    VERIFY (b.rows () == ROWS);
    VERIFY (b.cols () == COLS);
    VERIFY (b.max_size () >= b.size ());
    VERIFY (b.capacity () >= b.size ());
    raster<T> c (ROWS, COLS);
    c.reserve (ROWS*2, COLS*2);
    VERIFY (c.size () == b.size ());
    c.reserve (ROWS*3 * COLS*3);
    VERIFY (c.size () == b.size ());
    b.assign (1);
    c.assign (1);
    // relational ops
    VERIFY (b == c);
    b.back () = 0;
    VERIFY (b != c);
    raster<T> d (ROWS, COLS);
    raster<T> e (COLS, ROWS);
    VERIFY (d != e);
    // indexing
    raster<T> f (ROWS, COLS);
    VERIFY (f.row (f.begin ()) == 0);
    VERIFY (f.row (f.begin () + 1) == 0);
    VERIFY (f.row (f.end () - 1) == ROWS - 1);
    VERIFY (f.row (f.end ()) == ROWS);
    VERIFY (f.col (f.begin ()) == 0);
    VERIFY (f.col (f.begin () + 1) == 1);
    VERIFY (f.col (f.end () - 1) == COLS - 1);
    VERIFY (f.col (f.end ()) == 0); // not COLS
    // subregion creation
    subregion s = f.sub (-1, -1, ROWS - 1, COLS - 1);
    VERIFY (s.r == 0);
    VERIFY (s.c == 0);
    VERIFY (s.rows == ROWS - 2);
    VERIFY (s.cols == COLS - 2);
    s = f.sub (1, 1, ROWS, COLS);
    VERIFY (s.r == 1);
    VERIFY (s.c == 1);
    VERIFY (s.rows == ROWS - 1);
    VERIFY (s.cols == COLS - 1);
    s = f.sub (ROWS - 1, COLS - 1, ROWS, COLS);
    VERIFY (s.r == ROWS - 1);
    VERIFY (s.c == COLS - 1);
    VERIFY (s.rows == 1);
    VERIFY (s.cols == 1);
    s = f.sub (ROWS, COLS, ROWS, COLS);
    VERIFY (s.rows == 0);
    VERIFY (s.cols == 0);
    s = f.sub (ROWS, COLS - 1, 1, 1);
    VERIFY (s.rows == 0);
    VERIFY (s.cols == 1);
    s = f.sub (ROWS - 1, COLS, 1, 1);
    VERIFY (s.rows == 1);
    VERIFY (s.cols == 0);
}

template<typename T,size_t ROWS,size_t COLS>
void test_assignment ()
{
    raster<T> a;
    raster<T> b (ROWS, COLS, 1);
    raster<T> c (COLS, ROWS, 2);
    // swap
    swap (a, b);
    VERIFY (a.rows () == ROWS);
    VERIFY (a.cols () == COLS);
    VERIFY (a.front () == 1);
    VERIFY (a.back () == 1);
    VERIFY (b.empty ());
    // operator=
    a = c;
    VERIFY (a.rows () == c.rows ());
    VERIFY (a.cols () == c.cols ());
    VERIFY (a.front () == 2);
    VERIFY (a.back () == 2);
    // assign
    a.assign (3);
    VERIFY (a.front () == 3);
    VERIFY (a.back () == 3);
    // copy assignment
    raster<T> d (ROWS, COLS, 3);
    raster<double> e;
    e = d;
    VERIFY (e.rows () == ROWS);
    VERIFY (e.cols () == COLS);
    VERIFY (e.front () == 3);
    VERIFY (e.back () == 3);
}

template<typename T,size_t ROWS,size_t COLS>
void test_access ()
{
    // operator[], operator(i,j)
    raster<T> a (ROWS, COLS, 1);
    VERIFY (&a[0] == &a (0, 0));
    VERIFY (&a[a.size () - 1] == &a (ROWS - 1, COLS - 1));
    // at
    bool threw = false;
    try { a (0, 0) = a.at (ROWS - 1, COLS - 1); }
    catch (...) { threw = true; }
    VERIFY (!threw);
    try { a (0, 0) = a.at (ROWS, COLS); }
    catch (...) { threw = true; }
    VERIFY (threw);
    // begin, end, front, back
    list<T> b (ROWS * COLS);
    copy (a.begin (), a.end (), b.begin ());
    VERIFY (b.back () == 1);
    raster<T> c (ROWS, COLS);
    transform (c.begin (), c.end () - 1,
        c.begin () + 1,
        bind2nd (plus<T> (), 1));
    VERIFY (c.front () == 0);
    VERIFY (c.back () == ROWS * COLS - 1);
    // rbegin, rend
    raster<T> d (ROWS, COLS);
    copy (c.rbegin (), c.rend (), d.begin ());
    VERIFY (d.front () == ROWS * COLS - 1);
    VERIFY (d.back () == 0);
    // loc
    VERIFY (d.loc (0, 0) == d.begin ());
    VERIFY (d.loc (ROWS - 1, COLS) == d.end ());
}

template<typename T,size_t ROWS,size_t COLS>
void test_insert_remove ()
{
    raster<T> a (ROWS, COLS);
    raster<T> b (a);
    // clear
    a.clear ();
    VERIFY (a.empty ());
    // resize
    a.resize (ROWS, COLS);
    VERIFY (a.rows () == ROWS);
    VERIFY (a.cols () == COLS);
    a.resize (ROWS + 1, COLS + 1, 2);
    VERIFY (a.rows () == ROWS + 1);
    VERIFY (a.cols () == COLS + 1);
    for (size_t i = 0; i < a.rows (); ++i)
        VERIFY (a (i, COLS) == 2);
    for (size_t j = 0; j < a.cols (); ++j)
        VERIFY (a (ROWS, j) == 2);
    a.resize (ROWS, COLS);
    VERIFY (a == b);
    // insert_rows erase_rows
    typename raster<T>::iterator iter = a.insert_rows (a.begin (), 2, 10);
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 10);
    VERIFY (a (1, COLS - 1) == 10);
    VERIFY (a (2, 0) == 0);
    VERIFY (a (2, COLS - 1) == 0);
    iter = a.erase_row (a.begin ());
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 10);
    VERIFY (a (1, COLS - 1) == 0);
    iter = a.erase_row (a.begin ());
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 0);
    VERIFY (a (0, COLS - 1) == 0);
    iter = a.insert_rows (a.end (), 2, 11);
    VERIFY (&*iter == &a (ROWS, 0));
    VERIFY (a (ROWS - 1, 0) == 0);
    VERIFY (a (ROWS - 1, COLS - 1) == 0);
    VERIFY (a (ROWS, 0) == 11);
    VERIFY (a (ROWS + 1, COLS - 1) == 11);
    iter = a.erase_row (a.end () - 1);
    VERIFY (a (ROWS, 0) == 11);
    VERIFY (a (ROWS, COLS - 1) == 11);
    iter = a.erase_row (a.end () - 1);
    VERIFY (a == b);
    // insert_cols erase_cols
    iter = a.insert_cols (a.begin (), 2, 10);
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 10);
    VERIFY (a (ROWS - 1, 1) == 10);
    VERIFY (a (0, 2) == 0);
    VERIFY (a (ROWS - 1, 2) == 0);
    iter = a.erase_col (a.begin ());
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 10);
    VERIFY (a (ROWS - 1, 1) == 0);
    iter = a.erase_col (a.begin ());
    VERIFY (iter == a.begin ());
    VERIFY (a (0, 0) == 0);
    VERIFY (a (ROWS - 1, 0) == 0);
    iter = a.insert_cols (a.end (), 2, 11);
    VERIFY (&*iter == &a (0, COLS));
    VERIFY (a (0, COLS - 1) == 0);
    VERIFY (a (ROWS - 1, COLS - 1) == 0);
    VERIFY (a (0, COLS) == 11);
    VERIFY (a (ROWS - 1, COLS + 1) == 11);
    iter = a.erase_col (a.end () - 1);
    VERIFY (&*iter == &a (0, COLS + 1));
    VERIFY (a (0, COLS) == 11);
    VERIFY (a (ROWS - 1, COLS - 1) == 0);
    iter = a.erase_col (a.end () - 1);
    VERIFY (a == b);
}

template<typename T,size_t ROWS,size_t COLS>
void test_subregion_iter ()
{
    raster<T> a (ROWS, COLS, 1);
    typename raster<T>::subregion_iterator i;
    // non-edge element subregion
    subregion s = a.sub (1, 1, ROWS - 2, COLS - 2);
    for (i = a.begin (s); i != a.end (s); ++i)
        *i = 0;
    VERIFY (a (0, 0) == 1);
    VERIFY (a (0, COLS - 1) == 1);
    VERIFY (a (ROWS - 1, 0) == 1);
    VERIFY (a (ROWS - 1, COLS - 1) == 1);
    VERIFY (a (1, 1) == 0);
    VERIFY (a (1, COLS - 2) == 0);
    VERIFY (a (ROWS - 2, 1) == 0);
    VERIFY (a (ROWS - 2, COLS - 2) == 0);
    // subregion_iterator access
    typename raster<T>::subregion_iterator j = a.begin (s);
    VERIFY (a (1, 1) == 0);
    VERIFY (a (1, 2) == 0);
    *j = 1;
    VERIFY (a (1, 1) == 1);
    *++j = 2;
    VERIFY (a (1, 2) == 2);
    // const_subregion_iterator access
    const raster<T> b (a);
    j = a.begin (s);
    typename raster<T>::const_subregion_iterator k = b.begin (s);
    VERIFY (a (1, 1) == 1);
    *j = *k + 1;
    VERIFY (a (1, 1) == 2);
}

template<typename T>
class Mean
{
    T num;
    T sum;
    public:
    Mean () : num (0), sum (0) { }
    void operator() (const T &v) { ++num; sum += v; }
    T operator() () { return sum / num; }
};

template<typename T>
class Count
{
    size_t total;
    public:
    Count () : total (0) { }
    void operator() (const T &) { ++total; }
    size_t operator() () { return total; }
};

template<typename T,size_t ROWS,size_t COLS>
void test_algorithms ()
{
    raster<T> a (ROWS, COLS, 10);
    subregion all = { 0, 0, ROWS, COLS };
    Mean<T> m = for_each (a.begin (all), a.end (all), Mean<T> ());
    VERIFY (m () == 10);
    Count<T> count = for_each (a.begin (all), a.end (all), Count<T> ());
    VERIFY (count () == ROWS * COLS);
    const raster<T> b (ROWS, COLS, 8);
    raster<T> c (ROWS, COLS);
    subregion s = { 0, 0, ROWS, COLS };
    copy (b.begin (s), b.end (s), c.begin (s));
    VERIFY (c.front () == 8);
    VERIFY (c.back () == 8);
    raster<T> d (ROWS, COLS, 0);
    VERIFY (d.front () == 0);
    VERIFY (d.back () == 0);
    copy (b.begin (), b.end (), d.begin ());
    VERIFY (d.front () == 8);
    VERIFY (d.back () == 8);
}

template<typename T>
struct dist_op0
{
    dist_op0 (size_t , size_t ) { }
    T operator() (size_t r, size_t c)
    { return sqrt (static_cast<T> (c * c + r * r)); }
};

template<typename T>
struct dist_op1
{
    dist_op1 (size_t , size_t ) { }
    T operator() (size_t r, size_t c, const T& u)
    { return u * sqrt (static_cast<T> (c * c + r * r)); }
};

template<typename T>
struct dist_op2
{
    dist_op2 (size_t , size_t ) { }
    T operator() (size_t r, size_t c, const T& u, const T& v)
    { return (u + v) * sqrt (static_cast<T> (c * c + r * r)); }
};

template<typename T,size_t ROWS,size_t COLS>
void test_functions ()
{
    // generator
    raster<T> a (ROWS, COLS);
    generate (a.begin (), a.end (),
        subscript_generator<T,dist_op0> (ROWS, COLS));
    VERIFY (a (0, 0) == 0.0);
    VERIFY (a (ROWS - 1, COLS - 1) > ROWS);
    VERIFY (a (ROWS - 1, COLS - 1) > COLS);
    // unary_function
    raster<T> b (ROWS, COLS, 1.0);
    transform (b.begin (), b.end (), b.begin (),
        subscript_unary_function<T,dist_op1> (ROWS, COLS));
    VERIFY (b (0, 0) == 0.0);
    VERIFY (b (ROWS - 1, COLS - 1) > ROWS);
    VERIFY (b (ROWS - 1, COLS - 1) > COLS);
    raster<T> c0 (ROWS, COLS, 1.0);
    raster<T> c1 (ROWS, COLS, 1.0);
    raster<T> c2 (ROWS, COLS, 1.0);
    // binary_function in a subregion
    //
    // non-edge element subregion
    subregion s = c0.sub (1, 1, ROWS - 2, COLS - 2);
    transform (c0.begin (s), c0.end (s),
        c1.begin (s), c2.begin (s),
        subscript_binary_function<T,dist_op2> (s));
    VERIFY (c2 (0, 0) == 1.0);
    VERIFY (c2 (ROWS - 1, 0) == 1.0);
    VERIFY (c2 (0, COLS - 1) == 1.0);
    VERIFY (c2 (ROWS - 1, COLS - 1) == 1.0);
    VERIFY (c2 (1, 1) != 1.0);
    VERIFY (c2 (ROWS - 2, 1) != 1.0);
    VERIFY (c2 (1, COLS - 2) != 1.0);
    VERIFY (c2 (ROWS - 2, COLS - 2) != 1.0);
}

template<typename T,size_t ROWS,size_t COLS>
void test_copy ()
{
    raster<T> a (ROWS, COLS, 1);
    raster<T> b (ROWS, COLS, 0);
    copy (a.begin (), a.end (), b.begin ());
    VERIFY (b.front () == 1);
    VERIFY (b.back () == 1);
    b.assign (0);
    subregion s = { 1, 1, ROWS - 2, COLS - 2 };
    copy (a.begin (s), a.end (s), b.begin (s));
    VERIFY (b.front () == 0);
    VERIFY (b (1, 0) == 0);
    VERIFY (b (1, 1) == 1);
    VERIFY (b (1, COLS - 1) == 0);
    VERIFY (b (1, COLS - 2) == 1);
    VERIFY (b (ROWS - 2, 0) == 0);
    VERIFY (b (ROWS - 2, 1) == 1);
    VERIFY (b (ROWS - 2, COLS - 1) == 0);
    VERIFY (b (ROWS - 2, COLS - 2) == 1);
    VERIFY (b.back () == 0);
}

int main ()
{
    try
    {
        test_constructors<char,19,23> ();
        test_constructors<int,10,10> ();
        test_constructors<float,19,17> ();
        test_const_ops<complex<double>,7,11> ();
        test_assignment<wchar_t,23,32> ();
        test_assignment<unsigned,23,32> ();
        test_access<int,8,9> ();
        test_access<long double,5,6> ();
        test_insert_remove<long,3,5> ();
        test_subregion_iter<int,15,16> ();
        test_subregion_iter<char,128,5> ();
        test_algorithms<int,64,32> ();
        test_algorithms<float,64,32> ();
        test_functions<float,16,17> ();
        test_copy<float,12,21> ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
