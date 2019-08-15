#ifndef __WUP__CLOCK
#define __WUP__CLOCK

#include <wup/common/msgs.hpp>
#include <sys/time.h>
#include <cstdint>
#include <string>
#include <ctime>

namespace wup
{

class Clock {
public:

    Clock () :
        _delta_sec(0),
        _delta_nsec(0)
    {
        start();
    }

    Clock &
    start()
    {
        clock_gettime(clock_id, &_begin);
        return *this;
    }

    // Stops the clock, get the time took by calling ellapsed_*()

    Clock &
    stop()
    {
        clock_gettime(clock_id, &_end);
        _delta_nsec = _end.tv_nsec - _begin.tv_nsec;
        _delta_sec = _end.tv_sec - _begin.tv_sec;
        return *this;
    }


    // Stop the clock and start it again

    Clock &
    lap()
    {
        return stop().start();
    }


    // Display a message and the time passed since the last lap / start

    Clock &
    lap_second(const std::string msg)
    {
        printn(YELLOW, "|CLOCK| ", MAGENTA, msg, ": ", YELLOW, this->lap().ellapsed_seconds(), " s\n", NORMAL);
        return *this;
    }

    Clock &
    lap_milli(const std::string msg)
    {
        printn(YELLOW, "|CLOCK| ", MAGENTA, msg, ": ", YELLOW, this->lap().ellapsed_milli(), " ms\n", NORMAL);
        return *this;
    }

    Clock &
    lap_micro(const std::string msg)
    {
        printn(YELLOW, "|CLOCK| ", MAGENTA, msg, ": ", YELLOW, this->lap().ellapsed_micro(), " us\n", NORMAL);
        return *this;
    }

    Clock &
    lap_nano(const std::string msg)
    {
        printn(YELLOW, "|CLOCK| ", MAGENTA, msg, ": ", YELLOW, this->lap().ellapsed_nano(), " ns\n", NORMAL);
        return *this;
    }


    // Returns the ellapsed time in seconds / milli/ micro / nano
    // until the last time stop() was called

    long double
    ellapsed_seconds() const
    {
        return _delta_sec + _delta_nsec / static_cast<long double>(1000000000.0);
    }

    long double
    ellapsed_milli() const
    {
        return _delta_sec * static_cast<long double>(1000.0) + _delta_nsec / static_cast<long double>(1000000.0);
    }

    long double
    ellapsed_micro() const
    {
        return _delta_sec * static_cast<long double>(1000000.0) + _delta_nsec / static_cast<long double>(1000.0);
    }

    long double
    ellapsed_nano() const
    {
        return _delta_sec * static_cast<long double>(1000000000.0) + _delta_nsec;
    }

private:

    const int clock_id = CLOCK_MONOTONIC;

    struct timespec _begin, _end;

    long double _delta_sec;

    long double _delta_nsec;

};

class Clock2 {
public:

    Clock2() : _last(0)
    {
        start();
    }

    void start()
    {
        gettimeofday(&_begin, nullptr);
    }

    // Stops counting and returns the ellapsed time in microseconds
    long stop()
    {
        gettimeofday(&_end, nullptr);
        const long begin = (_begin.tv_usec + 1000000l * _begin.tv_sec);
        const long end   = (_end.tv_usec   + 1000000l * _end.tv_sec  );
        return _last = end - begin;
    }

    // Returns the ellapsed time in microseconds
    long ellapsed() const
    {
        return _last;
    }
    
    // Returns the ellapsed time in seconds
    double ellapsed_seconds() const
    {
        return _last / 1000000.0;
    }

private:

    struct timeval _begin, _end;

    long _last;

};

inline uint64_t
time_micro()
{
    struct timeval v;
    gettimeofday(&v, nullptr);
    return uint64_t(v.tv_usec) + uint64_t(1000000l) * uint64_t(v.tv_sec);
}

inline uint64_t
time_milli()
{
    return time_micro() / uint64_t(1000);
}

inline uint64_t
time_seconds()
{
    return time_micro() / uint64_t(1000000);
}

}; /* wup */

#endif /* __WUP__CLOCK */

