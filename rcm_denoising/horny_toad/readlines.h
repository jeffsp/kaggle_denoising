/// @file readlines.h
/// @brief readlines
/// @author Jeff Perry <jeffsp@gmail.com>
/// @version 1.0
/// @date 2013-01-15

#ifndef READLINES_H
#define READLINES_H

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace horny_toad
{

/// @brief read newline delimited strings from an input stream
///
/// @param s the input stream
///
/// @return vector of strings
std::vector<std::string> readlines (std::istream &s)
{
    std::vector<std::string> lines;
    std::string l;
    while (std::getline (s, l))
        lines.push_back (l);
    return lines;
}

/// @brief read lines from a file
///
/// @param f the file
///
/// @return vector of strings containing the lines
std::vector<std::string> readlines (const char *fn)
{
    std::ifstream s (fn);
    if (!s)
        throw std::runtime_error ("could not open file");
    return readlines (s);
}

/// @brief read whitespace delimited fields from an input stream
///
/// @tparam T type of fields to extract
/// @param s the input stream
///
/// @return vector of Ts containing the fields
template<typename T>
std::vector<T> readwords (std::istream &s)
{
    std::vector<T> words;
    T l;
    for (;;) // ever
    {
        s >> l;
        if (s)
            words.push_back (l);
        else
            break;
    }
    return words;
}

/// @brief read words from a file
///
/// @param f the file
///
/// @return vector of strings containing the words
template<typename T>
std::vector<T> readwords (const char *fn)
{
    std::ifstream s (fn);
    if (!s)
        throw std::runtime_error ("could not open file");
    return readwords<T> (s);
}

} // namespace horny_toad

#endif // READLINES_H
