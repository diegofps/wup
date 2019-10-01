#ifndef __WUP__MSGS
#define __WUP__MSGS

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <wup/common/exceptions.hpp>

#ifdef QT_CORE_LIB
#include <QString>
#endif

namespace wup
{

extern std::string logPrefix;

const char * const RED     = "\033[91m";
const char * const GREEN   = "\033[92m";
const char * const YELLOW  = "\033[93m";
const char * const BLUE    = "\033[94m";
const char * const MAGENTA = "\033[95m";
const char * const NORMAL  = "\033[0m";

const char * const BG_RED    = "\033[41m";
const char * const BG_BLUE   = "\033[44m";
const char * const BG_YELLOW = "\033[45m";
const char * const BG_GREEN  = "\033[46m";

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

template <typename S, typename P1>
std::stringstream & _join(std::stringstream &ss, const S & sep, const P1 &p1)
{
    ss << p1;
    return ss;
}

template <typename S, typename P1, typename... Args>
std::stringstream & _join(std::stringstream &ss, const S & sep, const P1 &p1, const Args&... args)
{
    ss << p1 << sep;
    return _join(ss, sep, args...);
}

template <typename S, typename... Args>
std::string join(const S sep, const Args&... args)
{
    std::stringstream ss;
    return _join(ss, sep, args...).str();
}

template <typename... Args>
std::string joins(const Args&... args)
{
    return join(" ", args...);
}

template <typename... Args>
void
setLogger(const Args&... args)
{
    logPrefix = cat(args...);
}

inline void
_print()
{
    std::cout << std::endl;
}

template <typename P1>
void _print(const P1 &p1)
{
    std::cout << p1 << std::endl;
}

template <typename P1, typename... Args>
void _print(const P1 &p1, const Args&... args)
{
    std::cout << p1 << " ";
    _print(args...);
}

template <typename P1>
void _printn(const P1 &p1)
{
    std::cout << p1;
}

template <typename P1, typename... Args>
void _printn(const P1 &p1, const Args&... args)
{
    std::cout << p1;
    _printn(args...);
}

template <typename... Args>
void _debug(const Args&... args)
{
    _print(GREEN, "Debug: ", args..., NORMAL);
}

template <typename... Args>
void _info(const Args&... args)
{
    _print(BLUE, "Info: ", args..., NORMAL);
}

template <typename... Args>
void _warn(const Args&... args)
{
    _print(YELLOW, "Warning: ", args..., NORMAL);
}

template <typename... Args>
void _error(const Args&... args)
{
    auto tmp = cat(RED, "Error: ", args..., NORMAL);
    _print(tmp);
    throw WUPException(tmp);
}

#define WRAP_LOGPREFIX(name, subname) \
template <typename... Args> \
void \
name(const Args&... args) \
{ \
    std::cout << logPrefix; \
    subname(args...); \
}

WRAP_LOGPREFIX(print, _print)
WRAP_LOGPREFIX(printn, _printn)
WRAP_LOGPREFIX(debug, _debug)
WRAP_LOGPREFIX(info, _info)
WRAP_LOGPREFIX(warn, _warn)
WRAP_LOGPREFIX(error, _error)

#define WRAP_LOGPREFIX_T(name, subname)             \
template <typename... Args>                         \
void                                                \
name(const Args&... args)                           \
{                                                   \
    subname(cats(logPrefix, " ", args..., "\n"));   \
}

WRAP_LOGPREFIX_T(printt, _printn)

inline void
pressEnter()
{
    print("Press ENTER to continue...");
    getchar();
}

inline void
notImplemented()
{
    error("This function is not implemented yet");
}

#ifdef QT_CORE_LIB

inline std::ostream &
operator<<(std::ostream & o, const QString & str)
{
    o << str.toUtf8().constData();
    return o;
}

#endif

} /* wup */

#endif /* __WUP__MSGS */

