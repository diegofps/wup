#ifndef STR_HPP
#define STR_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

namespace wup {
namespace str {

inline std::string
slice(std::string const str,
      int a,
      int b)
{
    int const len = int(str.length());
    if (a < 0) a = len + a;
    if (b < 0) b = len + b;
    return str.substr(a, b - a);
}

inline std::string
sliceFrom(std::string const str,
          int a)
{
    const int len = int(str.length());
    if (a < 0) a = len + a;
    return slice(str, a, len);
}

inline std::string
sliceTo(std::string const str,
        int b)
{
    const int len = int(str.length());
    if (b < 0) b = len + b;
    return slice(str, 0, b);
}

inline std::vector<std::string> &
split(std::string const & s,
      char const delim,
      std::vector<std::string> & elems)
{
    std::stringstream ss(s);
    std::string item;

    elems.clear();

    while (std::getline(ss, item, delim))
        elems.push_back(item);

    return elems;
}

inline bool
startsWith(char const * const src,
           char const * const prefix)
{
    for (size_t i=0;prefix[i]!='\0' && src[i]!='\0';++i)
        if (prefix[i] != src[i])
            return false;

    return true;
}

inline bool
startsWith(std::string const & src,
           char const * prefix)
{
    return startsWith(src.c_str(), prefix);
}

inline bool
startsWith(char const * src,
           std::string const & prefix)
{
    return startsWith(src, prefix.c_str());
}

inline bool
startsWith(std::string const & src,
           std::string const & prefix)
{
    return startsWith(src.c_str(), prefix.c_str());
}

inline bool
endsWith(char const * const src, const size_t l1,
         char const * const prefix, const size_t l2)
{
    return l1 < l2 ? false : startsWith(src + l1 - l2, prefix);
}

inline size_t
length(char const * const head)
{
    for (size_t l=0;;++l)
        if (head[l]=='\0')
            return l;
}

inline bool
endsWith(char const * const src,
         char const * const prefix)
{
    return endsWith(src, length(src), prefix, length(prefix));
}

inline bool
endsWith(std::string const & src,
         char const * const prefix)
{
    return endsWith(src.c_str(), src.size(), prefix, length(prefix));
}

inline bool
endsWith(char const * const src,
         std::string const & prefix)
{
    return endsWith(src, length(src), prefix.c_str(), prefix.size());
}

inline bool
endsWith(std::string const & src,
         std::string const & prefix)
{
    return endsWith(src.c_str(), src.size(), prefix.c_str(), prefix.size());
}

inline size_t
removeChar(char * const data,
           size_t const len,
           char const c)
{
    size_t a = 0;
    size_t b = 0;

    while(a!=len)
    {
        if (data[a] == c)
        {
            ++a;
        }
        else
        {
            data[b] = data[a];
            ++a;
            ++b;
        }
    }

    return b;
}

inline void
removeChar(std::string & data,
           char const c)
{
    data.resize(removeChar(&data[0], data.size(), c));
}

inline int
replaceOne(std::string & str,
           std::string const & from,
           std::string const & to)
{
    size_t start_pos = str.find(from);

    if(start_pos == std::string::npos)
        return 0;

    str.replace(start_pos, from.length(), to);

    return 1;
}

inline int
replaceAll(std::string & str,
           std::string const & from,
           std::string const & to)
{
    if(from.empty())
        return 0;

    size_t start_pos = 0;
    int replaces = 0;

    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);

        start_pos += to.length();
        replaces  += 1;
    }

    return replaces;
}

inline void
replaceRegex(std::string & src, std::string from, std::string to)
{
    std::string result = std::regex_replace(src, std::regex(from), to);
}

}
}

#endif // STR_HPP
