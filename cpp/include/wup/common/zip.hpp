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
/// uBuffer - Pointer to the sequence of bytes to be compresses.
/// uSize - Size of the uncompressed buffer to be compressed (in bytes).
/// cBuffer - Pointer to the compressed buffer (will be allocated by the function with new[]). You must delete after use.
/// cSize - Size of the compressed buffer (in bytes).
template <typename A, typename B>
void
zip(const A * const uBuffer, const uint64_t uSize, B *& cBuffer, uint64_t & cSize,
    int compressionLevel=Z_DEFAULT_COMPRESSION)
{
    uLongf boundSize = sizeof(uint64_t) + compressBound(uSize);

    if (boundSize % sizeof(int32_t))
        boundSize = (boundSize / sizeof(int32_t) + 1) * sizeof(int32_t);

    Bytef *dest = new Bytef[boundSize];
    uLongf *destLen = (uLongf*)dest;
    const Bytef *source = (const Bytef*)uBuffer;
    uLong sourceLen = uSize;

    *destLen = boundSize - sizeof(uint64_t);

    int result = compress2(dest+sizeof(uint64_t), destLen, source, sourceLen, compressionLevel);

    *destLen += sizeof(uint64_t);

    cBuffer = (B*) dest;
    cSize = *destLen;
    *destLen = uSize;

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
/// cBuffer - Pointer to the compressed sequence of bytes.
/// cSize - Size of the compressed buffer (in bytes).
/// uBuffer - Pointer to the uncompressed buffer (will be allocated by the function with new[]).
/// uSize - Size of the uncompressed buffer (in bytes).
template <typename A, typename B>
void
unzip(const A * const cBuffer, const uint64_t cSize, B *& uBuffer, uint64_t & uSize)
{
    uLongf & tmp = *((uLongf*) cBuffer);

    Bytef * src = (Bytef *) cBuffer;
    Bytef * dst = new Bytef[tmp];

    int result = uncompress(dst, &tmp, src+sizeof(uLongf), (uLong) cSize);

    uSize = tmp;
    uBuffer = (B*) dst;

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
