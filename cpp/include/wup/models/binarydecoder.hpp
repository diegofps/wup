#ifndef __WUP_BINARYDECODER_HPP
#define __WUP_BINARYDECODER_HPP

//#include "pattern.hpp"

#include <cstring>
#include <cstdlib>
#include <stdint.h>   // for uint32_t
#include <limits.h>   // for CHAR_BIT

#include <wup/common/exceptions.hpp>

inline uint32_t rotl32 (uint32_t n, uint c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n<<c) | (n>>( (-c)&mask ));
}

inline uint32_t rotr32 (uint32_t n, uint c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n>>c) | (n<<( (-c)&mask ));
}

namespace wup {

class BinaryDecoder {
public:
    BinaryDecoder() :
            _indexes(NULL),
            _size(0),
            _pattern(NULL),
            _hash(0)
    {

    }
    
    BinaryDecoder(const uint * const indexes, const uint size) :
            _indexes(indexes),
            _size(size),
            _pattern(new int[_size]()),
            _hash(0)
    {

    }
    
    BinaryDecoder(const BinaryDecoder & other) :
            _indexes(other._indexes),
            _size(other._size),
            _pattern(new int[_size]()),
            _hash(other._hash)
    {
        memcpy(_pattern, other._pattern, sizeof(int) * _size);
    }
    
    ~BinaryDecoder()
    {
        if (_pattern != NULL)
            delete [] _pattern;
    }
    
    uint
    size() const
    {
		return _size; 
	}

    const int *
    pattern() const
    {
        return _pattern;
    }

    int *
    pattern()
    {
        return _pattern;
    }

    template <typename Retina>
    void read(const Retina & retina)
    {
        if (_indexes == NULL)
            throw WUPException();
        
        for (uint i=0;i<_size;++i)
            _pattern[i] = retina[_indexes[i]] != 0;

        updateHash();
    }
    
    void
    setInput(const int * const input)
    {
        memcpy(_pattern, input, sizeof(int) * _size);
        updateHash();
    }
    
    bool 
    operator==(const BinaryDecoder & other) const
    {
        if (_size != other._size)
            return false;
        
        for (uint i=0;i<_size;++i)
            if (_pattern[i] != other._pattern[i])
                return false;
        
        return true;
    }

    BinaryDecoder &
    operator=(const BinaryDecoder & other )
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
    {
        return _hash;
    }

    void
    updateHash()
    {
        /*_hash = 2;
        for (int i=0;i<_size;++i)
            _hash = 13 * _hash + (_pattern[i] ? 7 : 11);*/

        _hash = 0;
        for (uint i=0;i<_size;++i)
            _hash = rotl32(_hash,3) ^ (_pattern[i]?2:3);

        /*_hash = 1;
        for (int i=0;i<_size;++i)
            _hash = (17*_hash + (_pattern[i]? 7 : 11)) % 104729;*/
    }

private:

    const uint * _indexes;

    uint _size;

    int * _pattern;

    size_t _hash;

};

} /* wup */

namespace std {
    
    template <>
    struct hash<wup::BinaryDecoder> {
        std::size_t operator()(const wup::BinaryDecoder & k) const
        {
            //wup::print(k.hash());
            return k.hash();
        }
    };
    
} /* std */

#endif /* __WUP_BINARYDECODER_HPP */

