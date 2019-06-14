#ifndef __WUP_RANGES
#define __WUP_RANGES

#include <wup/common/msgs.hpp>
#include <vector>

namespace wup
{

template <typename T>
class XRange {

public:

    class iterator {
    public:
        
        iterator(const T end, const T current, const T step) : 
            _end(end), _current(current), _step(step) { }
        
        const T & operator*() const  { return _current; }
        
        void operator++() { _current += _step; }
        
        bool operator!=(const iterator &other) const
        {
            if (_step < 0) return _current > other._end;
            else return _current < other._end;
        }
        
    private:
        
        const T _end;
        
        T _current;
        
        const T _step;
        
    };

    XRange(const T start, const T end, const T step) : 
        _end(end), _start(start), _step(step) { }
    
    iterator begin() const { return iterator(_end, _start, _step); }
    
    iterator end() const { return iterator(_end, _end, _step); }
    
    const T _end;
    
    const T _start;
    
    const T _step;
    
};

template <typename T>
XRange<T> xrange(const T &end)
{
    return XRange<T>(0, end, 1);
}

template <typename T>
XRange<T> xrange(const T &start, const T &end, const T &step=1)
{
    if (step == 0) error("step parameter in xrange can not be zero");
    return XRange<T>(start, end, step);
}

template <typename T>
std::vector<T> range(const T &end)
{
    std::vector<T> v;
    for (auto &i : xrange(end)) v.push_back(i);
    return v;
}

template <typename T>
std::vector<T> range(const T &start, const T &end, const T &step=1)
{
    std::vector<T> v;
    for (auto &i : xrange(start, end, step)) v.push_back(i);
    return v;
}

}; /* wup */

#endif /* __WUP_RANGES */

