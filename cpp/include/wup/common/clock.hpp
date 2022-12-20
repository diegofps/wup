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

    double
    lap_seconds(double factor=1.0)
    {
        stop();
        const double ellapsed = this->ellapsed_seconds() / factor;
        start();
        return ellapsed;
    }

    double
    lap_milli(double factor=1.0)
    {
        stop();
        const double ellapsed = this->ellapsed_milli() / factor;
        start();
        return ellapsed;
    }

    double
    lap_micro(double factor=1.0)
    {
        stop();
        const double ellapsed = this->ellapsed_micro() / factor;
        start();
        return ellapsed;
    }

    double
    lap_nano(double factor=1.0)
    {
        stop();
        const double ellapsed = this->ellapsed_micro() / factor;
        start();
        return ellapsed;
    }


    // Display a message and the time passed since the last lap / start

    Clock &
    lap_seconds(const std::string msg, double factor=1.0)
    {
        stop();
        const auto ellapsed = this->ellapsed_seconds() / factor;
        printn(YELLOW, "|CLOCK| ", PURPLE, msg, ": ", YELLOW, ellapsed, " s\n", NORMAL);
        return start();
    }

    Clock &
    lap_milli(const std::string msg, double factor=1.0)
    {
        stop();
        const auto ellapsed = this->ellapsed_milli() / factor;
        printn(YELLOW, "|CLOCK| ", PURPLE, msg, ": ", YELLOW, ellapsed, " ms\n", NORMAL);
        return start();
    }

    Clock &
    lap_micro(const std::string msg, double factor=1.0)
    {
        stop();
        const auto ellapsed = this->ellapsed_micro() / factor;
        printn(YELLOW, "|CLOCK| ", PURPLE, msg, ": ", YELLOW, ellapsed, " us\n", NORMAL);
        return start();
    }

    Clock &
    lap_nano(const std::string msg, double factor=1.0)
    {
        stop();
        const auto ellapsed = this->ellapsed_micro() / factor;
        printn(YELLOW, "|CLOCK| ", PURPLE, msg, ": ", YELLOW, ellapsed, " ns\n", NORMAL);
        return start();
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
time_now_in_micro()
{
    struct timeval v;
    gettimeofday(&v, nullptr);
    return uint64_t(v.tv_usec) + uint64_t(1000000l) * uint64_t(v.tv_sec);
}

inline uint64_t
time_now_in_milli()
{
    return time_now_in_micro() / uint64_t(1000);
}

inline uint64_t
time_now_in_seconds()
{
    return time_now_in_micro() / uint64_t(1000000);
}

}; /* wup */

#endif /* __WUP__CLOCK */

