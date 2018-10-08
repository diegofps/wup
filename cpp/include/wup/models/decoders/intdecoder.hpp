#ifndef __WUP_INTDECODER_HPP
#define __WUP_INTDECODER_HPP

namespace wup 
{

class IntDecoder 
{
public:

    const uint * _indexes;
    
    int _inputSize;
    
    long _address;
    
public:
    
    IntDecoder() :
        _indexes(nullptr),
        _inputSize(0)
    {

    }
    
    IntDecoder(const uint * const indexes, const uint inputSize) : 
        _indexes(indexes),
        _inputSize(inputSize)
    {
        
    }
    
    IntDecoder(const uint * const first, const uint * const last) : 
        _indexes(first),
        _inputSize(last-first)
    {

    }
    
    bool 
    operator==(const IntDecoder & other) const
    {
        return _address == other._address;
    }

    bool 
    operator<(const IntDecoder & other) const
    {
        return _address < other._address;
    }

    template <typename Retina>
    void 
    read(const Retina & retina)
    {
        _address = 0;
        for (const uint * index = _indexes; index != _indexes + _inputSize; ++index)
            _address = (_address << 1) | retina[*index];
    }

    bool
    isZero() const
    {
        return _address == 0;
    }

};

} /* wup */

namespace std 
{

    struct std::hash<long> h;

    template <>
    struct hash<wup::IntDecoder> 
    {
        std::size_t operator()(const wup::IntDecoder & k) const
        {
            return h.operator()(k._address);
        }
    };

} /* std */

#endif /* __WUP_INTDECODER_HPP */

