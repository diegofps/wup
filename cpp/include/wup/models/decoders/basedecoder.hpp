#ifndef __WUP_DECODERS_BASEDECODER_HPP
#define __WUP_DECODERS_BASEDECODER_HPP

#include <cstring>
#include <cstdlib>

#include <wup/common/exceptions.hpp>

namespace wup {

class BaseDecoder {
public:

    BaseDecoder() :
            _indexes(NULL),
            _inputSize(0),
            _patternSize(0),
            _pattern(NULL),
            _hash(0)
    {

    }

    BaseDecoder(const uint * const indexes, const uint inputSize, const uint patternSize) :
            _indexes(indexes),
            _inputSize(inputSize),
            _patternSize(patternSize),
            _pattern(new int[_patternSize]()),
            _hash(0)
    {

    }

    BaseDecoder(const BaseDecoder & other) :
            _indexes(other._indexes),
            _inputSize(other._inputSize),
            _patternSize(other._patternSize),
            _pattern(new int[_patternSize]()),
            _hash(other._hash)
    {
        memcpy(_pattern, other._pattern, sizeof(int) * _patternSize);
    }

    virtual
    ~BaseDecoder()
    {
        if (_pattern != NULL)
            delete [] _pattern;
    }

    uint
    inputSize() const
    {
        return _inputSize;
    }

    uint
    patternSize() const
    {
        return _patternSize;
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

    const uint *
    indexes()
    {
        return _indexes;
    }

    bool
    operator==(const BaseDecoder & other) const
    {
        if (_inputSize != other._inputSize)
            return false;

        if (_patternSize != other._patternSize)
            return false;

        for (uint i=0;i<_patternSize;++i)
            if (_pattern[i] != other._pattern[i])
                return false;

        return true;
    }

    BaseDecoder &
    operator=(const BaseDecoder & other)
    {
        if (_patternSize != other._patternSize) {
            if (_pattern != NULL)
                delete [] _pattern;
            _pattern = new int[other._patternSize];
        }

        _inputSize = other._inputSize;
        _patternSize = other._patternSize;
        _indexes = other._indexes;
        _hash = other._hash;

        memcpy(_pattern, other._pattern, sizeof(int) * _patternSize);

        return *this;
    }

    size_t
    hash() const
    {
        return _hash;
    }

    void
    hash(size_t h)
    {
        _hash = h;
    }

private:

    const uint * _indexes;

    uint _inputSize;

    uint _patternSize;

    int * _pattern;

    size_t _hash;

};

} /* wup */

namespace std {

    template <>
    struct hash<wup::BaseDecoder> {
        std::size_t operator()(const wup::BaseDecoder & k) const
        {
            return k.hash();
        }
    };

} /* std */

#endif /* __WUP_DECODERS_BASEDECODER_HPP */

