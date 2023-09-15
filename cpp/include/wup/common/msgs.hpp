#ifndef __WUP__MSGS
#define __WUP__MSGS

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <wup/common/exceptions.hpp>

#ifdef QT_CORE_LIB
#include <QString>
#endif

namespace wup
{

const char * const NORMAL    = "\033[0m";

const char * const BRIGHTER  = "\033[1m";
const char * const DARKER    = "\033[2m";
const char * const ITALIC    = "\033[3m";
const char * const UNDERLINE = "\033[4m";
const char * const BLINKING  = "\033[5m";
const char * const REVERSE   = "\033[7m";
const char * const INVISIBLE = "\033[8m";
const char * const CROSSING  = "\033[9m";

const char * const GRAY    = "\033[90m";
const char * const RED     = "\033[91m";
const char * const GREEN   = "\033[92m";
const char * const YELLOW  = "\033[93m";
const char * const BLUE    = "\033[94m";
const char * const PURPLE  = "\033[95m";
const char * const CYAN    = "\033[96m";
const char * const WHITE   = "\033[97m";

const char * const BOLD_GRAY   = "\033[1;30m";
const char * const BOLD_RED    = "\033[1;31m";
const char * const BOLD_GREEN  = "\033[1;32m";
const char * const BOLD_YELLOW = "\033[1;33m";
const char * const BOLD_BLUE   = "\033[1;34m";
const char * const BOLD_PURPLE = "\033[1;35m";
const char * const BOLD_CYAN   = "\033[1;36m";
const char * const BOLD_WHITE  = "\033[1;37m";

const char * const BG_GRAY    = "\033[40m";
const char * const BG_RED    = "\033[41m";
const char * const BG_GREEN  = "\033[42m";
const char * const BG_YELLOW = "\033[43m";
const char * const BG_BLUE   = "\033[44m";
const char * const BG_PURPLE = "\033[45m";
const char * const BG_CYAN   = "\033[46m";
const char * const BG_WHITE  = "\033[46m";

template <typename A, typename B>
std::ostream & operator<<(std::ostream &o, const std::pair<A,B> &pair)
{
	return o << pair.first << "," << pair.second;
}

template <typename P1>
std::stringstream & _path_join(std::stringstream &ss, const P1 &p1)
{
    ss << p1;
    return ss;
}

template <typename P1, typename... Args>
std::stringstream & _path_join(std::stringstream &ss, const P1 &p1, const Args&... args)
{
    std::stringstream ss2;
    ss2 << p1;
    std::string p1_2 = ss2.str();

    if (p1_2.compare("") == 0)
        return ss;

    ss << p1_2;

    if (p1_2[p1_2.size()-1] != '/')
        ss << '/';
        
    return _path_join(ss, args...);
}

template <typename... Args>
std::string path_join(const Args&... args)
{
    std::stringstream ss;
    return _path_join(ss, args...).str();
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
std::stringstream & _join(std::stringstream &ss, const S &, const P1 &p1)
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

template <typename S, typename I>
std::string joinList(const S sep, const std::vector<I> & list)
{
    if (list.size() == 0)
        return "";
    
    std::stringstream ss;
    ss << list[0];

    for (size_t i=1;i!=list.size();++i)
        ss << sep << list[i];
    
    return ss.str();
}

template <typename S1, typename S2, typename K, typename V>
std::string joinMap(const S1 sepPairs, const S2 sepKV, const std::map<K,V> & list)
{
    if (list.size() == 0)
        return "";
    
    std::stringstream ss;
    auto it = list.begin();
    ss << it->first << sepKV << it->second;

    for(++it;it!=list.end();++it)
        ss << sepPairs << it->first << sepKV << it->second;
    
    return ss.str();
}

template <typename... Args>
std::string joins(const Args&... args)
{
    return join(" ", args...);
}

inline void
print()
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
    std::cout << p1 << " ";
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
    auto tmp = join(" ", args...);
    auto msg = cat(GREEN, BRIGHTER, "Debug: ", NORMAL, GREEN, tmp, NORMAL, "\n");
    printn(msg);
}

template <typename... Args>
void info(const Args&... args)
{
    auto tmp = join(" ", args...);
    auto msg = cat(BLUE, BRIGHTER, "Info: ", NORMAL, BLUE, tmp, NORMAL, "\n");
    printn(msg);
}

template <typename... Args>
void warn(const Args&... args)
{
    auto tmp = join(" ", args...);
    auto msg = cat(YELLOW, BRIGHTER, "Warning: ", NORMAL, YELLOW, tmp, NORMAL, "\n");
    printn(msg);
}

template <typename... Args>
void error(const Args&... args)
{
    auto tmp = join(" ", args...);
    auto msg = cat(RED, BRIGHTER, "Error: ", NORMAL, RED, tmp, NORMAL, "\n");
    printn(msg);

    throw WUPException(tmp);
}

template <typename... Args>
void bright(const Args&... args)
{
    auto tmp = join(" ", args...);
    auto msg = cat(BRIGHTER, tmp, NORMAL, "\n");
    printn(msg);
}

template <typename P1, typename P2>
void _printEventArgs(const P1 &p1, const P2 &p2)
{
    std::cout << p1 << "=" << p2 << std::endl;
}

template <typename P1, typename P2, typename... Args>
void _printEventArgs(const P1 &p1, const P2 &p2, const Args&... args)
{
    std::cout << p1 << "=" << p2 << ", ";
    _printEventArgs(args...);
}

template <typename P1, typename... Args>
void printEvent(const P1 &p1, const Args&... args)
{
    std::cout << p1 << ":\n  ";
    _printEventArgs(args...);
    print();
}

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

