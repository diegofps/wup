#ifndef __WUP__MSGS
#define __WUP__MSGS

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <wup/common/exceptions.hpp>

namespace wup
{

const char * const BLUE   = "\033[94m";
const char * const GREEN  = "\033[92m";
const char * const YELLOW = "\033[93m";
const char * const RED    = "\033[91m";
const char * const NORMAL = "\033[0m";

template <typename A, typename B>
std::ostream & operator<<(std::ostream &o, const std::pair<A,B> &pair)
{
	return o << pair.first << "," << pair.second;
}

template <typename P1>
std::stringstream & _cat(std::stringstream &ss, const P1 &p1)
{
    ss << p1;
    return ss;
}

template <typename P1, typename... Args>
std::stringstream & _cat(std::stringstream &ss, const P1 &p1, const Args&... args)
{
    ss << p1;
    return _cat(ss, args...);
}

template <typename... Args>
std::string cat(const Args&... args)
{
	std::stringstream ss;
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

#ifdef QT_VERSION_STR

inline std::ostream &
operator<<(std::ostream & o, const QString & str)
{
    o << str.toUtf8().constData();
    return o;
}

#endif

} /* wup */

#endif /* __WUP__MSGS */

