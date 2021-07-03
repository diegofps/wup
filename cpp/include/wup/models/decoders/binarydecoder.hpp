#ifndef __WUP_BINARYDECODER_HPP
#define __WUP_BINARYDECODER_HPP

#include <cstring>
#include <cstdlib>

#include <wup/common/bits.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/exceptions.hpp>
#include <wup/models/decoders/basedecoder.hpp>

namespace wup {

class BinaryDecoder : public BaseDecoder {
public:

    BinaryDecoder() :
        BaseDecoder()
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
        
        isZero(true);
        
        for (uint i=0;i<inputSize();++i)
        {
            if (retina[indexes()[i]] != 0)
            {
                pattern()[i] = 1;
                isZero(false);
            }
            else
            {
                pattern()[i] = 0;
            }
        }

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
            h = bits::rotateLeft(h,3) ^ (pattern()[i]?2:3);

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

