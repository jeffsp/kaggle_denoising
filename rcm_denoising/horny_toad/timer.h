/// @file timer.h
/// @brief timer stuff
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-14

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

namespace horny_toad
{

/// @brief A microsecond resolution timer in the MATLAB tic/toc style...
class timer
{
    public:

    /// @brief Start the timer
    void tic ()
    {
        struct timezone tz;
        gettimeofday (&start, &tz);
    }
    /// @brief Get elapsed time since tic()
    double toc () const
    {
        struct timeval stop;
        struct timezone tz;
        gettimeofday (&stop, &tz);
        double t1, t2;
        const double MICROSEC_FREQ = 1000000.0;
        t1 = start.tv_sec + start.tv_usec / MICROSEC_FREQ;
        t2 = stop.tv_sec + stop.tv_usec / MICROSEC_FREQ;
        return t2 - t1;
    }
    private:
    struct timeval start;
};

} // namespace horny_toad

#endif // TIMER_H
