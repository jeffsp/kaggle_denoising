/// @file tracer.h
/// @brief log message to clog, but only on a DEBUG build
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-03-18

#ifndef TRACER_H
#define TRACER_H

#include <string>
#include <iostream>

namespace horny_toad
{

/// @brief Support for TRACER macro
class tracer
{
#ifndef NDEBUG
    private:
    std::ostream &os;
    public:
    tracer(std::ostream &s, const std::string &file, const std::string &func, const int line)
        : os (s)
    {
        os << file << ", " << func << "(), line " << line << ": ";
    }
    template <class T>
    tracer &operator<<(const T &v)
    {
        os << v;
        return *this;
    }
    ~tracer()
    {
        os << std::endl;
    }
#else
    public:
    tracer(std::ostream &, const std::string &, const std::string &, const int ) { }
    template <class T>
    tracer &operator<<(const T &) { return *this; }
    ~tracer() { }
#endif
};

/// @brief VERIFY that a statement is true, ignoring NDEBUG
#define TRACE(s) tracer(s,__FILE__,__FUNCTION__,__LINE__)

} // namespace horny_toad

#endif // TRACER_H
