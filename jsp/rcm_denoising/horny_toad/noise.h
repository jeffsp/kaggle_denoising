/// @file noise.h
/// @brief noise utilities
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-15

#ifndef NOISE_H
#define NOISE_H

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include "laplace_distribution.hpp"

namespace horny_toad
{

/// @brief generate gaussian noise
///
/// @tparam T type of noise samples
/// @param samples number of noise samples to return
/// @param seed RNG seed
///
/// @return vector of noise samples
template<typename T>
std::vector<T> gaussian_noise (size_t samples, unsigned seed)
{
    std::vector<T> s (samples);
    boost::mt19937 rng (seed);
    boost::normal_distribution<T> dist (0.0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<T>> gen (rng, dist);
    for (size_t i = 0; i < s.size (); ++i)
        s[i] = gen ();
    return s;
}

/// @brief generate laplace noise
///
/// @tparam T type of noise samples
/// @param samples number of noise samples to return
/// @param seed RNG seed
///
/// @return vector of noise samples
template<typename T>
std::vector<T> laplace_noise (size_t samples, unsigned seed)
{
    std::vector<T> s (samples);
    boost::mt19937 rng (seed);
    // set the mean to 0.0 and the set the scale parameter such that the
    // variance of the distribution equals 1.0
    boost::laplace_distribution<T> dist (0.0, sqrt (1.0 / 2.0));
    boost::variate_generator<boost::mt19937&, boost::laplace_distribution<T>> gen (rng, dist);
    for (size_t i = 0; i < s.size (); ++i)
        s[i] = gen ();
    return s;
}

} // namespace horny_toad

#endif // NOISE_H
