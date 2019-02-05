#ifndef __WUP__CLOCK
#define __WUP__CLOCK

#include <sys/time.h> 

namespace wup
{

class Clock {
public:

    Clock () : _last(0)
    {
        start();
    }

    void start()
    {
        gettimeofday(&_begin, NULL);
    }

    // Stops counting and returns the ellapsed time in microseconds
    long stop()
    {
        gettimeofday(&_end, NULL);
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

}; /* wup */

#endif /* __WUP__CLOCK */

