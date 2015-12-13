#ifndef __WUP_RAMINPUT_HPP
#define __WUP_RAMINPUT_HPP

#include "pattern.hpp"
#include <stdint.h>   // for uint32_t
#include <limits.h>   // for CHAR_BIT

inline uint32_t rotl32 (uint32_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n<<c) | (n>>( (-c)&mask ));
}

inline uint32_t rotr32 (uint32_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n>>c) | (n<<( (-c)&mask ));
}

namespace wup {

class RamInput {
public:
    RamInput() :
            _indexes(NULL),
            _size(0),
            _pattern(NULL),
            _hash(0)
        { }
    
    RamInput(const int * const indexes, const int size) : 
            _indexes(indexes),
            _size(size),
            _pattern(new bool[_size]()),
            _hash(0)
        { }
    
    RamInput(const RamInput & other) :
            _indexes(other._indexes),
            _size(other._size),
            _pattern(new bool[_size]()),
            _hash(other._hash)
        {
            memcpy(_pattern, other._pattern, sizeof(bool) * _size);
        }
    
    ~RamInput()
    {
        if (_pattern != NULL)
            delete [] _pattern;
    }
    
    int
    size() const
        { return _size; }
    
    const bool * 
    pattern() const
        { return _pattern; }
    
    void
    read(const wup::Pattern & pattern)
    {
        if (_indexes == NULL)
            throw WUPException();
        
        for (int i=0;i<_size;++i)
            _pattern[i] = pattern[_indexes[i]] == 1;
        _updateHash();
    }
    
    void
    setInput(const bool * const input)
    {
        memcpy(_pattern, input, sizeof(bool) * _size);
        _updateHash();
    }
    
    bool 
    operator==(const RamInput & other) const
    {
        if (_size != other._size)
            return false;
        
        for (int i=0;i<_size;++i)
            if (_pattern[i] != other._pattern[i])
                return false;
        
        return true;
    }

    RamInput &
    operator=(const RamInput & other )
    {
        if (_size != other._size) {
            delete [] _pattern;
            _pattern = new bool[other._size];
        }
        
        _size = other._size;
        memcpy(_pattern, other._pattern, sizeof(bool) * _size);
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
        /*_hash = 2;
        for (int i=0;i<_size;++i)
            _hash = 13 * _hash + (_pattern[i] ? 7 : 11);*/
        
        _hash = 0;
        for (int i=0;i<_size;++i)
            _hash = rotl32(_hash,3) ^ (_pattern[i]?2:3);
        
        /*_hash = 1;
        for (int i=0;i<_size;++i)
            _hash = (17*_hash + (_pattern[i]? 7 : 11)) % 104729;*/
    }
    
private:
    const int * _indexes;
    int _size;
    bool * _pattern;
    size_t _hash;
};

} /* wup */

namespace std {
    
    template <>
    struct hash<wup::RamInput> {
        std::size_t operator()(const wup::RamInput & k) const
        {
            //wup::print(k.hash());
            return k.hash();
        }
    };
    
} /* std */

#endif /* __WUP_RAMINPUT_HPP */
