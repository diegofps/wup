#ifndef __WUP_DECODERS_GRAYDECODER_HPP
#define __WUP_DECODERS_GRAYDECODER_HPP

#include <cstdlib>
#include <cstring>

#include <wup/common/exceptions.hpp>
#include <wup/models/decoders/basedecoder.hpp>

namespace wup {

class GrayDecoder : public BaseDecoder {
public:

    GrayDecoder() :
        BaseDecoder(),
        _hashPattern(NULL),
        _factorials(NULL)
    {

    }
    
    GrayDecoder(const uint * const indexes, const uint inputSize) :
            BaseDecoder(indexes, inputSize, inputSize),
            _hashPattern(new int[inputSize]()),
            _factorials(createFactorials(inputSize))
    {

    }

    GrayDecoder(const GrayDecoder & other) :
            BaseDecoder(other),
            _hashPattern(new int[other.inputSize()]()),
            _factorials(createFactorials(other.inputSize()))
    {

    }
    
    ~GrayDecoder()
    {
        if (_hashPattern != NULL)
            delete [] _hashPattern;
    }

    template <typename Retina>
    void read(const Retina & retina)
    {
        if (indexes() == NULL)
            throw WUPException();
        
        // For each gray pixel
        for (uint i=0;i<inputSize();++i) {
            uint n = 0;
            const int current = retina[indexes()[i]];
            
            // Calculate the number of other gray pixels that are bigger than the current one
            // If the pixel value is the same, consider their position instead
            for (uint j=0;j<inputSize();++j) {
                const int visiting = retina[indexes()[j]];
                
                if ((current < visiting) || (current == visiting && i < j))
                    n += 1;

                //if ((abs(current - visiting) < 20 && i < j) || (current < visiting))
                //    n += 1;
                    
                // TODO Make the minimum distance a parameter
                //if ((abs(current-visiting)<10 && i < j) || (current < visiting))
                //    n += 1;
            }
            
            pattern()[i] = n;
        }
        
        updateHash();
    }

    GrayDecoder &
    operator=(const GrayDecoder & other)
    {
        if (inputSize() != other.inputSize()) {
            if (_hashPattern != NULL)
                delete [] _hashPattern;
            if (_factorials != NULL)
                delete [] _factorials;

            _hashPattern = new int[other.inputSize()];
            _factorials = createFactorials(other.inputSize());
        }

        this->BaseDecoder::operator=(other);
        return *this;
    }

    void
    updateHash()
    {
        std::copy(pattern(), pattern() + inputSize(), _hashPattern);
        size_t h = 0;

        for (uint n=inputSize()-1; n!=0; --n) {
            h += _hashPattern[n] * _factorials[n];

            for (int j=n-1; j!=-1; --j) {
                if (_hashPattern[j] > _hashPattern[n]) {
                    --_hashPattern[j];
                }
            }
        }

        hash(h);
    }

private:

    size_t * createFactorials(const uint inputSize)
    {
        size_t * f = new size_t[inputSize];
        f[0] = 1;

        for (size_t i=1; i<inputSize; ++i)
            f[i] = i * f[i-1];

        return f;
    }

private:

    int * _hashPattern;

    size_t * _factorials;

};

} /* wup */

namespace std {

    template <>
    struct hash<wup::GrayDecoder> {
        std::size_t operator()(const wup::GrayDecoder & k) const
        {
            return k.hash();
        }
    };

} /* std */

#endif /* __WUP_DECODERS_GRAYDECODER_HPP */

