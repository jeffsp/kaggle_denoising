/// @file fft.h
/// @brief fftw interface
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef FFT_H
#define FFT_H

#include "fftw3.h"
#include <cassert>
#include <complex>
#include <functional>
#include <iosfwd>
#include <numeric>

namespace horny_toad
{

/// @brief Metaprogram support
template<bool INVERSE>
struct SignFlag { };

/// @brief Metaprogram support
template<>
struct SignFlag<false>
{ static inline int value () { return FFTW_FORWARD; } };

/// @brief Metaprogram support
template<>
struct SignFlag<true>
{ static inline int value () { return FFTW_BACKWARD; } };

/// @brief Fast Fourier Transform
template<typename IN,typename OUT,bool INVERSE=false>
class FFT
{
    public:
    /// @brief Create an FFT object
    ///
    /// @param dims vector of dimensions (e.g: dims[2] = {480, 640})
    /// @param in input data
    /// @param out output data
    /// @param flags (e.g.: FFTW_ESTIMATE,FFTW_MEASURE,FFTW_EXHAUSTIVE)
    ///
    /// @note The dimension parameter's rightmost value is
    /// the least significant digit.  Thus, index (z,y,x)
    /// and (z,y,x+1) are physically adjacent in memory.
    ///
    /// @note For complex transforms, the input and output
    /// vector sizes must be equal, and both must be equal
    /// to the size specified by the dimensions.
    ///
    /// @note For real/complex transforms, the dimensions
    /// always specify the size of the real vector.
    ///
    /// @note For real to complex transforms, the size of
    /// the output vector's last dimension must be >= (the
    /// size of the input vector's last dimension)/2+1.
    ///
    /// @note For complex to real transforms, the size of
    /// the input vector's last dimension must be >= (the
    /// size of the output vector's last dimension)/2+1.
    ///
    /// @note During a complex to real transform, the
    /// contents of in are destroyed.
    template<typename DIM_ITER, typename INPUT_ITER, typename OUTPUT_ITER>
    FFT (const DIM_ITER dims_begin, const DIM_ITER dims_end,
        const INPUT_ITER in_begin, const INPUT_ITER in_end,
        OUTPUT_ITER out_begin, OUTPUT_ITER out_end,
        unsigned flags = FFTW_ESTIMATE)
    {
        assert (dims_end > dims_begin);
        set_plan (
            &*dims_begin, &*dims_end,
            &*in_begin, &*in_end,
            &*out_begin, &*out_end,
            flags);
    }
    /// @brief FFT dtor
    ~FFT ()
    {
        fftw_destroy_plan (plan_);
    }
    /// @brief do the transform
    void operator() ()
    {
        fftw_execute (plan_);
    }
    private:
    template<typename T>
    void set_plan (
        const int *dims_begin, const int *dims_end,
        const std::complex<T> *in_begin, const std::complex<T> *in_end,
        std::complex<T> *out_begin, std::complex<T> *out_end,
        unsigned flags)
    {
        int dims_size = dims_end - dims_begin;
        int dims_total = accumulate (dims_begin, dims_end, 1, std::multiplies<int> ());
        int in_size = in_end - in_begin;
        int out_size = out_end - out_begin;
        assert (in_size == dims_total);
        assert (out_size == in_size);
        plan_ = fftw_plan_dft (dims_size,
                const_cast<int *> (dims_begin),
                reinterpret_cast<fftw_complex*>(const_cast<std::complex<T> *> (in_begin)),
                reinterpret_cast<fftw_complex*>(out_begin),
                SignFlag<INVERSE>::value (),
                flags);
    }
    template<typename T>
    void set_plan (
        const int *dims_begin, const int *dims_end,
        const T *in_begin, const T *in_end,
        std::complex<T> *out_begin, std::complex<T> *out_end,
        unsigned flags)
    {
        int dims_size = dims_end - dims_begin;
        int dims_total = accumulate (dims_begin, dims_end, 1, std::multiplies<int> ());
        int in_size = in_end - in_begin;
        int out_size = out_end - out_begin;
        int dims_back = *(dims_end - 1);
        assert (in_size == dims_total);
        assert (out_size == in_size / dims_back * (dims_back / 2 + 1));
        plan_ = fftw_plan_dft_r2c (dims_size,
                const_cast<int *> (dims_begin),
                const_cast<T *> (in_begin),
                reinterpret_cast<fftw_complex*>(out_begin),
                flags);
    }
    template<typename T>
    void set_plan (
        const int *dims_begin, const int *dims_end,
        const std::complex<T> *in_begin, const std::complex<T> *in_end,
        T *out_begin, T *out_end,
        unsigned flags)
    {
        int dims_size = dims_end - dims_begin;
        int dims_total = accumulate (dims_begin, dims_end, 1, std::multiplies<int> ());
        int in_size = in_end - in_begin;
        int out_size = out_end - out_begin;
        int dims_back = *(dims_end - 1);
        assert (in_size == out_size / dims_back * (dims_back / 2 + 1));
        assert (out_size == dims_total);
        plan_ = fftw_plan_dft_c2r (dims_size, dims_begin,
                reinterpret_cast<fftw_complex*>(const_cast<std::complex<T> *> (in_begin)),
                out_begin,
                flags);
    }
    fftw_plan plan_;
};

typedef FFT<double,std::complex<double> > forward_real_fft;
typedef FFT<std::complex<double>,double> inverse_real_fft;
typedef FFT<std::complex<double>,std::complex<double>,false> forward_complex_fft;
typedef FFT<std::complex<double>,std::complex<double>,true> inverse_complex_fft;

} // namespace horny_toad

#endif // FFT_H
