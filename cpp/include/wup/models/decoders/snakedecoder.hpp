#ifndef __WUP_SNAKEDECODER_HPP
#define __WUP_SNAKEDECODER_HPP

#include <cstring>
#include <cstdlib>

#include <wup/common/bits.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/exceptions.hpp>
#include <wup/models/decoders/basedecoder.hpp>

namespace wup {

class SnakeDecoder : public BaseDecoder {
public:

    SnakeDecoder() :
        BaseDecoder()
    {

    }

    SnakeDecoder(const uint * const indexes, const uint inputSize) :
        BaseDecoder(indexes, inputSize, inputSize-1)
    {

    }

    SnakeDecoder(const SnakeDecoder & other) :
        BaseDecoder(other)
    {

    }

    template <typename Retina>
    void read(const Retina & retina)
    {
        if (indexes() == NULL)
            throw WUPException();

        for (uint i=0;i<patternSize();++i) {
            auto & current = retina[indexes()[i]];
            auto & next = retina[indexes()[i+1]];

            if (current < next)
                pattern()[i] = 0;

            else if (current == next)
                pattern()[i] = 1;

            else
                pattern()[i] = 2;
        }

        updateHash();
    }

    void
    updateHash()
    {
        size_t h = 0;
        for (uint i=0;i<patternSize();++i)
            h = rotateLeft(h,3) ^ (pattern()[i]?2:3);

        hash(h);
    }

};

} /* wup */

namespace std {

    template <>
    struct hash<wup::SnakeDecoder> {
        std::size_t operator()(const wup::SnakeDecoder & k) const
        {
            return k.hash();
        }
    };

} /* std */

#endif /* __WUP_SNAKEDECODER_HPP */
