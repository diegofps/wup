#ifndef __WUP_BINARYDECODER_HPP
#define __WUP_BINARYDECODER_HPP

//#include "pattern.hpp"

#include <cstring>
#include <cstdlib>
#include <stdint.h>   // for uint32_t
#include <limits.h>   // for CHAR_BIT

#include <wup/common/exceptions.hpp>
#include <wup/models/decoders/basedecoder.hpp>

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

class BinaryDecoder : public BaseDecoder {
public:

    BinaryDecoder() : BaseDecoder()
    {

    }
    
    BinaryDecoder(const uint * const indexes, const uint inputSize) :
        BaseDecoder(indexes, inputSize, inputSize)
    {

    }
    
    BinaryDecoder(const BinaryDecoder & other) :
        BaseDecoder(other)
    {

    }

    template <typename Retina>
    void read(const Retina & retina)
    {
        if (indexes() == NULL)
            throw WUPException();
        
        for (uint i=0;i<inputSize();++i)
            pattern()[i] = retina[indexes()[i]] != 0;

        updateHash();
    }

    void
    updateHash()
    {
        /*_hash = 2;
        for (int i=0;i<_size;++i)
            _hash = 13 * _hash + (_pattern[i] ? 7 : 11);*/

        size_t h = 0;
        for (uint i=0;i<patternSize();++i)
            h = rotl32(h,3) ^ (pattern()[i]?2:3);

        /*_hash = 1;
        for (int i=0;i<_size;++i)
            _hash = (17*_hash + (_pattern[i]? 7 : 11)) % 104729;*/

        hash(h);
    }

};

} /* wup */

namespace std {

    template <>
    struct hash<wup::BinaryDecoder> {
        std::size_t operator()(const wup::BinaryDecoder & k) const
        {
            return k.hash();
        }
    };

} /* std */

#endif /* __WUP_BINARYDECODER_HPP */

