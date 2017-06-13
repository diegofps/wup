#ifndef __WUP_GRAYDECODER_HPP
#define __WUP_GRAYDECODER_HPP

#include <cstdlib>
#include <cstring>

#include <wup/common/exceptions.hpp>

namespace wup {

class GrayDecoder {
public:

    GrayDecoder() :
            _indexes(NULL),
            _size(0),
            _pattern(NULL),
            _hashPattern(NULL),
            _hash(0)
    {

    }
    
    GrayDecoder(const uint * const indexes, const uint size) : 
            _indexes(indexes),
            _size(size),
            _pattern(new int[_size]()),
            _hashPattern(new int[_size]()),
            _hash(0)
    {

    }
    
    GrayDecoder(const GrayDecoder & other) :
            _indexes(other._indexes),
            _size(other._size),
            _pattern(new int[_size]()),
            _hashPattern(new int[_size]()),
            _hash(other._hash)
    {
        memcpy(_pattern, other._pattern, sizeof(int) * _size);
    }
    
    ~GrayDecoder()
    {
        if (_pattern != NULL)
            delete [] _pattern;

        if (_hashPattern != NULL)
            delete [] _hashPattern;
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
        
        // For each gray pixel
        for (uint i=0;i<_size;++i) {
            uint n = 0;
            const int current = retina[_indexes[i]];
            
            // Calculate the number of other gray pixels that are bigger than the current one
            // If the pixel value is the same, consider their position instead
            for (uint j=0;j<_size;++j) {
                const int visiting = retina[_indexes[j]];
                
                if ((current < visiting) || (current == visiting && i < j))
                    n += 1;

                //if ((abs(current - visiting) < 20 && i < j) || (current < visiting))
                //    n += 1;
                    
                // TODO Make the minimum distance a parameter
                //if ((abs(current-visiting)<10 && i < j) || (current < visiting))
                //    n += 1;
            }
            
            _pattern[i] = n;
        }
        
        updateHash();
    }
    
/*
    void
    setInput(const int * const input)
    {
        memcpy(_pattern, input, sizeof(int) * _size);
        updateHash();
    }
*/
    
    bool 
    operator==(const GrayDecoder & other) const
    {
        if (_size != other._size)
            return false;
        
        for (uint i=0;i<_size;++i)
            if (_pattern[i] != other._pattern[i])
                return false;
        
        return true;
    }

    GrayDecoder &
    operator=(const GrayDecoder & other )
    {
        if (_size != other._size) {
            delete [] _pattern;
            delete [] _hashPattern;
            _pattern = new int[other._size];
            _hashPattern = new int[other._size];
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
        std::copy(_pattern, _pattern + _size, _hashPattern);
        _hash = 0;

        for (uint n=_size-1;n!=0;--n) {
            _hash += _hashPattern[n] * _factorial(n);

            for (int j=n-1;j!=-1;--j) {
                if (_hashPattern[j] > _hashPattern[n]) {
                    --_hashPattern[j];
                }
            }
        }
    }

private:

    uint 
    _factorial(uint n) 
    {
        return n <= 1 ? 1 : n * _factorial(n - 1);
    }
    
private:

    const uint * _indexes;

    uint _size;

    int * _pattern;

    int * _hashPattern;

    size_t _hash;

};

} /* wup */

namespace std {
    
    template <>
    struct hash<wup::GrayDecoder> {
        std::size_t operator()(const wup::GrayDecoder & k) const
        {
            //wup::print(k.hash());
            return k.hash();
        }
    };
    
} /* std */

#endif /* __WUP_GRAYDECODER_HPP */

