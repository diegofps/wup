#ifndef ZIP_HPP
#define ZIP_HPP

#include <zlib.h>
#include <cstdint>
#include "exceptions.hpp"
#include <wup/common/msgs.hpp>

namespace wup
{

/// Compresses a sequence of bytes.
///
/// buffer - Pointer to the sequence of bytes to be compresses.
/// bufferSize - Size of the buffer to be compressed (in bytes).
/// compressedBuffer - Pointer to the compressed buffer (will be allocated by the function with new[]).
/// compressedSize - Size of the compressed buffer (in bytes).
template <typename A, typename B>
void
zip(const A * const buffer, const uint64_t bufferLength, B *& compressedBuffer, uint64_t & compressedSize)
{
    uLongf boundSize = sizeof(uint64_t) + compressBound(bufferLength);

    if (compressedBuffer == nullptr)
    {
        print("creating new area for zip");
        compressedBuffer = new Bytef[boundSize];
    }
    else if (compressedSize < boundSize)
    {
        print("expanding area for zip");
        delete [] compressedBuffer;
        compressedBuffer = new Bytef[boundSize];
    }
    else
    {
        print("reusing for zip");
    }

    Bytef *dest = compressedBuffer;
    uLongf *destLen = (uLongf*)dest;
    const Bytef *source = buffer;
    uLong sourceLen = bufferLength;

    *destLen = boundSize;

    int result = compress(dest+sizeof(uint64_t), destLen, source, sourceLen);

    *destLen += sizeof(uint64_t);

    //compressedBuffer = dest;
    compressedSize = *destLen;
    *destLen = bufferLength;

    if (result == Z_OK)
        return;

    if (result == Z_MEM_ERROR)
        throw wup::WUPException("MEM_ERROR");

    else if (result == Z_BUF_ERROR)
        throw wup::WUPException("BUF_ERROR");

    else
        throw wup::WUPException("unknown error");
}

/// Uncompresses a sequence of bytes.
///
/// compressedBuffer - Pointer to the compressed sequence of bytes.
/// compressedSize - Size of the compressed buffer (in bytes).
/// uncompressedBuffer - Pointer to the uncompressed buffer (will be allocated by the function with new[]).
/// uncompressedSize - Size of the uncompressed buffer (in bytes).
template <typename A, typename B>
void
unzip(const A * const compressedBuffer, const uint64_t compressedSize,
      B *& uncompressedBuffer, uint64_t & uncompressedSize)
{
    uLongf & tmp = *((uLongf*) compressedBuffer);

    if (uncompressedBuffer == nullptr)
    {
        print("creating new area for unzip");
        uncompressedBuffer = new Bytef[tmp];
    }
    else if (uncompressedSize < tmp)
    {
        print("expanding area for unzip");
        delete [] uncompressedBuffer;
        uncompressedBuffer = new Bytef[tmp];
    }
    else
    {
        print("reusing for zip");
    }

    Bytef * src = (Bytef*) compressedBuffer;
    Bytef * dst = uncompressedBuffer;

    int result = uncompress(dst, &tmp, src+sizeof(uLongf), (uLong) compressedSize);

    uncompressedSize = tmp;
    //uncompressedBuffer = dst;

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