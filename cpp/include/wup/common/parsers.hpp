#ifndef PARSERS_HPP
#define PARSERS_HPP

#include <wup/common/exceptions.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace wup {

template <typename T>
T
parseBasicType(std::string const & str)
{
    std::stringstream ss(str);
    T n;
    ss >> n;

    if ((ss.rdstate() & std::ifstream::failbit) || (ss.rdstate() & std::ifstream::badbit))
    {
        std::cout << str << std::endl;
        throw ParsersException(std::string("Error while parsing double"));
    }
    else
    {
        return n;
    }
}

template <typename T>
T
parseBasicType(char const * const str)
{
    return parseBasicType<T>(std::string(str));
}

inline bool
parseBool(const std::string & str)
{
    if (str.size() == 0)
        throw ParsersException(std::string("Could not read boolean value from string"));

    char const & c = str[0];

    return c == 'y' ||
           c == 'Y' ||
           c == 's' ||
           c == 'S' ||
           c == 't' ||
           c == 'T' ||
           c == '1';
}

inline double parseDouble(std::string const & str) { return parseBasicType<double>(str); }
inline double parseDouble(char const * const str) { return parseBasicType<double>(str); }

inline long parseLong(std::string const & str) { return parseBasicType<long>(str); }
inline long parseLong(char const * const str) { return parseBasicType<long>(str); }

inline ulong parseULong(std::string const & str) { return parseBasicType<ulong>(str); }
inline ulong parseULong(char const * const str) { return parseBasicType<ulong>(str); }

inline int parseInt(std::string const & str) { return parseBasicType<int>(str); }
inline int parseInt(char const * const str) { return parseBasicType<int>(str); }

inline uint parseUInt(std::string const & str) { return parseBasicType<uint>(str); }
inline uint parseUInt(char const * const str) { return parseBasicType<uint>(str); }

inline float parseFloat(std::string const & str) { return parseBasicType<float>(str); }
inline float parseFloat(char const * const str) { return parseBasicType<float>(str); }

inline short parseShort(std::string const & str) { return parseBasicType<short>(str); }
inline short parseShort(char const * const str) { return parseBasicType<short>(str); }

inline ushort parseUShort(std::string const & str) { return parseBasicType<ushort>(str); }
inline ushort parseUShort(char const * const str) { return parseBasicType<ushort>(str); }

}

#endif // PARSERS_HPP
