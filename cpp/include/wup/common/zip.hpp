#ifndef ZIP_HPP
#define ZIP_HPP

#include <zlib.h>
#include <cstdint>
#include <vector>
#include <wup/common/exceptions.hpp>
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
zip(const A * const uncompressedBuffer, const uint64_t uncompressedSize,
    B *& compressedBuffer, uint64_t & compressedSize,
    int compressionLevel=Z_DEFAULT_COMPRESSION)
{
    uLongf boundSize = sizeof(uint64_t) + compressBound(uncompressedSize);

    if (boundSize % sizeof(int32_t))
        boundSize = (boundSize / sizeof(int32_t) + 1) * sizeof(int32_t);

    Bytef *dest = new Bytef[boundSize];
    uLongf *destLen = (uLongf*)dest;
    const Bytef *source = (const Bytef*)uncompressedBuffer;
    uLong sourceLen = uncompressedSize;

    *destLen = boundSize - sizeof(uint64_t);

    int result = compress2(dest+sizeof(uint64_t), destLen, source, sourceLen, compressionLevel);

    *destLen += sizeof(uint64_t);

    compressedBuffer = (B*) dest;
    compressedSize = *destLen;
    *destLen = uncompressedSize;

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

    Bytef * src = (Bytef *) compressedBuffer;
    Bytef * dst = new Bytef[tmp];

    int result = uncompress(dst, &tmp, src+sizeof(uLongf), (uLong) compressedSize);

    uncompressedSize = tmp;
    uncompressedBuffer = (B*) dst;

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
