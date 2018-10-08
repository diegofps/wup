#ifndef WUP_CRONUS_HPP__
#define WUP_CRONUS_HPP__

#include <chrono>
#include <wup/common/msgs.hpp>

namespace wup
{

class Cronus
{
public:

    Cronus(bool enabled_=true) :
        enabled(enabled_)
    {
        start();
    }

    void start()
    {
        begin = std::chrono::high_resolution_clock::now();
        last = begin;
    }

    int loop(const char * msg = nullptr)
    {
        current = std::chrono::high_resolution_clock::now();
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(current - last).count();
        if (enabled && msg != nullptr) print(msg, ": ", ms);
        last = current;
        return ms;
    }

    int total(const char * msg = nullptr)
    {
        current = std::chrono::high_resolution_clock::now();
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(current - begin).count();
        if (enabled && msg != nullptr) print(msg, ": ", ms);
        return ms;
    }

    long now()
    {
        current = std::chrono::high_resolution_clock::now();
        return timeToLong(current);
    }

    long timeToLong(std::chrono::time_point<std::chrono::high_resolution_clock>  & t)
    {
        auto duration = t.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

private:

    std::chrono::time_point<std::chrono::high_resolution_clock> begin;

    std::chrono::time_point<std::chrono::high_resolution_clock> last;

    std::chrono::time_point<std::chrono::high_resolution_clock> current;

    bool enabled;

};

}

#endif /* WUP_CRONUS_HPP__ */

