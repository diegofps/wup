#ifndef __WUP_BASENDECODER_HPP
#define __WUP_BASENDECODER_HPP

#include <cstring>
#include <cstdlib>

#include <wup/common/bits.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/exceptions.hpp>
#include <wup/models/decoders/basedecoder.hpp>

namespace wup {

class BaseNDecoder : public BaseDecoder {
public:

    BaseNDecoder() : BaseDecoder()
    {

    }

    BaseNDecoder(const uint * const indexes, const uint inputSize) :
        BaseDecoder(indexes, inputSize, inputSize)
    {

    }

    BaseNDecoder(const BaseNDecoder & other) :
        BaseDecoder(other)
    {

    }

    template <typename Retina>
    void read(const Retina & retina)
    {
        if (indexes() == NULL)
            throw WUPException();

        for (uint i=0;i<inputSize();++i)
            pattern()[i] = retina[indexes()[i]];

        updateHash();
    }

    void
    updateHash()
    {
        size_t h = 0;
        for (uint i=0;i<patternSize();++i)
            h = rotateLeft(h,3) ^ (pattern()[i]);
        hash(h);
    }

};

} /* wup */

namespace std {

    template <>
    struct hash<wup::BaseNDecoder> {
        std::size_t operator()(const wup::BaseNDecoder & k) const
        {
            return k.hash();
        }
    };

} /* std */


#endif // __WUP_BASENDECODER_HPP
