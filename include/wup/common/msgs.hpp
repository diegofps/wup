#ifndef __WUP__MSGS
#define __WUP__MSGS

#include <string>
#include <sstream>
#include <iostream>

#include <wup/common/exceptions.hpp>

namespace wup
{

using namespace std;

const char * const BLUE   = "\033[94m";
const char * const GREEN  = "\033[92m";
const char * const YELLOW = "\033[93m";
const char * const RED    = "\033[91m";
const char * const NORMAL = "\033[0m";

template <typename P1>
stringstream & _cat(stringstream &ss, const P1 &p1)
{
    ss << p1;
    return ss;
}

template <typename P1, typename... Args>
stringstream & _cat(stringstream &ss, const P1 &p1, const Args&... args)
{
    ss << p1;
    return _cat(ss, args...);
}

template <typename... Args>
string cat(const Args&... args)
{
    stringstream ss;
    return _cat(ss, args...).str();
}

inline void print()
{
    std::cout << std::endl;
}

template <typename P1>
void print(const P1 &p1)
{
    std::cout << p1 << std::endl;
}

template <typename P1, typename... Args>
void print(const P1 &p1, const Args&... args)
{
    std::cout << p1;
    print(args...);
}

template <typename P1>
void printn(const P1 &p1)
{
    std::cout << p1;
}

template <typename P1, typename... Args>
void printn(const P1 &p1, const Args&... args)
{
    std::cout << p1;
    printn(args...);
}

template <typename... Args>
void debug(const Args&... args)
{
    print(GREEN, "Debug: ", args..., NORMAL);
}

template <typename... Args>
void info(const Args&... args)
{
    print(BLUE, "Info: ", args..., NORMAL);
}

template <typename... Args>
void warn(const Args&... args)
{
    print(YELLOW, "Warning: ", args..., NORMAL);
}

template <typename... Args>
void error(const Args&... args)
{
    print(RED, "Error: ", args..., NORMAL);
    throw FatalException();
}

}; /* wup */

#endif /* __WUP__MSGS */

