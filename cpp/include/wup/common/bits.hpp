#ifndef BITS_HPP
#define BITS_HPP

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

}

#endif // BITS_HPP
