#ifndef ZIP_HPP
#define ZIP_HPP

#include <zlib.h>
#include <cstdint>
#include "exceptions.hpp"

namespace wup
{

template <typename A, typename B>
void
zip(const A * const buffer, const uint64_t LENGTH, B *& compressedBuffer, uint64_t & compressedSize)
{
    uLongf boundSize = sizeof(uint64_t) + compressBound(LENGTH);

    Bytef *dest = new Bytef[boundSize];
    uLongf *destLen = (uLongf*)dest;
    const Bytef *source = buffer;
    uLong sourceLen = LENGTH;

    *destLen = boundSize;

    int result = compress(dest+sizeof(uint64_t), destLen, source, sourceLen);

    *destLen += sizeof(uint64_t);

    compressedBuffer = dest;
    compressedSize = *destLen;
    *destLen = LENGTH;

    if (result == Z_OK)
        return;

    if (result == Z_MEM_ERROR)
        throw wup::WUPException("MEM_ERROR");

    else if (result == Z_BUF_ERROR)
        throw wup::WUPException("BUF_ERROR");

    else
        throw wup::WUPException("unknown error");
}

template <typename A, typename B>
void
unzip(const A * const compressedBuffer, const uint64_t compressedSize,
      B *& uncompressedBuffer, uint64_t & uncompressedSize)
{
    uLongf & tmp = *((uLongf*) compressedBuffer);

    Bytef * src = (Bytef*) compressedBuffer;
    Bytef * dst = new Bytef[tmp];

    int result = uncompress(dst, &tmp, src+sizeof(uLongf), (uLong) compressedSize);

    uncompressedSize = tmp;
    uncompressedBuffer = dst;

    if (result == Z_OK)
        return;

    if (result == Z_MEM_ERROR)
        throw wup::WUPException("MEM_ERROR");

    else if (result == Z_DATA_ERROR)
        throw wup::WUPException("DATA_ERROR");

    else if (result == Z_BUF_ERROR)
        throw wup::WUPException("BUF_ERROR");

    else
        throw wup::WUPException("unknown error");
}

}

#endif // ZIP_HPP
