#ifndef __WUP_FACTORIALINPUT_HPP
#define __WUP_FACTORIALINPUT_HPP

#include "pattern.hpp"

namespace wup {

class FactorialInput {
public:

    FactorialInput() :
            _indexes(NULL),
            _size(0),
            _pattern(NULL),
            _hash(0)
        { }
    
    FactorialInput(const int * const indexes, const int size) : 
            _indexes(indexes),
            _size(size),
            _pattern(new int[_size]()),
            _hash(0)
        { }
    
    FactorialInput(const FactorialInput & other) :
            _indexes(other._indexes),
            _size(other._size),
            _pattern(new int[_size]()),
            _hash(other._hash)
        {
            memcpy(_pattern, other._pattern, sizeof(int) * _size);
        }
    
    ~FactorialInput()
    {
        if (_pattern != NULL)
            delete [] _pattern;
    }
    
    int
    size() const
        { return _size; }
    
    const int * 
    pattern() const
        { return _pattern; }
    
    void
    read(const wup::Pattern & pattern)
    {
        if (_indexes == NULL)
            throw WUPException();
        
        for (int i=0;i<_size;++i) {
            int n = 0;
            const int current = pattern[_indexes[i]];
            
            for (int j=0;j<_size;++j) {
                const int visiting = pattern[_indexes[j]];
                if ((current == visiting && i < j) || (current < visiting))
                    n += 1;
            }
            
            _pattern[i] = n;
        }
        
        _updateHash();
    }
    
    void
    setInput(const int * const input)
    {
        memcpy(_pattern, input, sizeof(int) * _size);
    }
    
    bool 
    operator==(const FactorialInput & other) const
    {
        if (_size != other._size)
            return false;
        
        for (int i=0;i<_size;++i)
            if (_pattern[i] != other._pattern[i])
                return false;
        
        return true;
    }

    FactorialInput &
    operator=(const FactorialInput & other )
    {
        if (_size != other._size) {
            delete [] _pattern;
            _pattern = new int[other._size];
        }
        
        _size = other._size;
        memcpy(_pattern, other._pattern, sizeof(int) * _size);
        _indexes = other._indexes;
        _hash = other._hash;
        return *this;
    }

    size_t
    hash() const
        { return _hash; }
    
private:
    
    void
    _updateHash()
    {
        _hash = 0;
        
        for (int n=_size-1;n!=0;--n) {
            _hash += _pattern[n] * _factorial(n);
            
            for (int j=n-1;j!=-1;--j) {
                if (_pattern[j] > _pattern[n]) {
                    --_pattern[j];
                }
            }
        }
    }
    
    size_t 
    _factorial(int n) 
        { return n <= 1 ? 1 : n * _factorial(n - 1); }
    
private:
    const int * _indexes;
    int _size;
    int * _pattern;
    size_t _hash;
};

} /* wup */

namespace std {
    
    template <>
    struct hash<wup::FactorialInput> {
        std::size_t operator()(const wup::FactorialInput & k) const
        {
            //wup::print(k.hash());
            return k.hash();
        }
    };
    
} /* std */

#endif /* __WUP_FACTORIALINPUT_HPP */

