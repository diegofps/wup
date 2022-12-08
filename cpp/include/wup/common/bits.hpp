#ifndef BITS_HPP
#define BITS_HPP

#include <cstdlib>
#include <string>

#include <stdint.h>
#include <limits.h>

namespace wup {

inline uint32_t
rotateLeft(uint32_t n,
           uint32_t c)
{
    const uint32_t mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n<<c) | (n>>( (-c)&mask ));
}

inline uint32_t
rotateRight(uint32_t n,
            uint32_t c)
{
    const uint32_t mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}

inline uint32_t
swapUInt32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

inline std::string
binarize(uint8_t const * const data, 
         std::size_t bytes, 
         std::size_t step=0)
{
    if (step == 0)
        step = bytes * CHAR_BIT;

    std::size_t bits = bytes * CHAR_BIT;
    std::string result(bits + (bits%step==0?bits/step-1:bits/step), '_');

    std::size_t k = 0;
    std::size_t o = 0;

    for(std::size_t byte=0; byte!=bytes; ++byte)
        for(std::size_t bit=0; bit<CHAR_BIT; ++bit, ++k)
        {
            if (k % step == 0 && k != 0)
                result[o++] = ' ';

            result[o++] = '0' + (((data[byte]) >> bit) & 1);
        }

    return result;
}

inline std::string
hexify(std::uint8_t const * const data, 
       std::size_t const bytes, 
       std::size_t step=0)
{
    if (step == 0)
        step = bytes * CHAR_BIT / 4;

    std::size_t bits = bytes * CHAR_BIT / 4;
    std::string result(bits + (bits%step==0?bits/step-1:bits/step), '_');
    char const * const chars = "0123456789ABCDEF";

    std::size_t k = 0;
    std::size_t o = 0;

    for(std::size_t byte=0; byte!=bytes; ++byte)
        for(std::size_t bit=0; bit<CHAR_BIT; bit+=4, ++k)
        {
            if (k % step == 0 && k != 0)
                result[o++] = ' ';

            result[o++] = chars[(((data[byte]) >> bit) & 0xF)];
        }

    return result;
}

template<typename T>
std::string
binarize(T const & t, 
       std::size_t const step=0)
{
    return binarize((uint8_t*)(&t), sizeof(t), step);
};

template<typename T>
std::string
hexify(T const & t, 
       std::size_t const step=0)
{
    return hexify((uint8_t*)(&t), sizeof(t), step);
};

}

#endif // BITS_HPP
