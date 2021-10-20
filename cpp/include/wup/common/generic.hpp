#ifndef __WUP__GENERIC
#define __WUP__GENERIC

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <sstream>
#include <dirent.h>
#include <stdint.h>
#include <limits.h>

//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>

#include <wup/common/exceptions.hpp>
#include <wup/common/msgs.hpp>

namespace wup {

typedef struct _BOX
{
    uint id;
    double w;

    _BOX() { }
    _BOX(const uint _id, const double _w) : id(_id), w(_w) { }
    bool operator<(const wup::_BOX & other) { return w < other.w; }

} BOX;

#ifdef __ANDROID_API__
inline double log2(const double value)
{ return log(value) / log(2.); }
#endif

template <typename T, typename B>
void
prevIndex(T & index, const B & size, bool loop=true)
{
    if (size == 0)
    {
        index = 0;
    }

    else if (index == 0)
    {
        if (loop)
            index = size - 1;
    }

    else
    {
        --index;
    }
}

template <typename T, typename B>
void
nextIndex(T & index, const B & size, bool loop=true)
{
    if (size == 0)
    {
        index = 0;
    }

    else if (++index == size)
    {
        index = loop ? 0 : size - 1;
    }
}

inline void
topkqsort(BOX * const array, const int bottom, const int top, const int k)
{
    BOX y;
    int i = bottom;
    int j = top;

    double x = array[(bottom + top) / 2].w;

    while (i <= j) {
        while (array[i].w > x && i < top)
            ++i;

        while (array[j].w < x && j > bottom)
            --j;

        if (i <= j) {
            y = array[i];
            array[i] = array[j];
            array[j] = y;
            ++i;
            --j;
        }
    }

    if (j > bottom && j >= k)
        topkqsort(array, bottom, j, k);

    if (i < top && i <= k)
        topkqsort(array,  i, top, k);
}

inline void
halfqsort(BOX * const array, const int bottom, const int top, const int k)
{
    BOX y;
    int i = bottom;
    int j = top;

    while (i <= j) {
        const double x = array[(bottom + top) / 2].w;

        while (array[i].w > x && i < top)
            ++i;

        while (array[j].w < x && j > bottom)
            --j;

        if (i <= j) {
            y = array[i];
            array[i] = array[j];
            array[j] = y;
            ++i;
            --j;
        }
    }

    // TODO Needs tunning
    if (j > bottom)
        halfqsort(array, bottom, j, k);

    if (i < top && i <= k)
        halfqsort(array,  i, top, k);
}

inline void
halfqsort(BOX * const array, const int length, const int k)
{
    halfqsort(array, 0, length - 1, k);
}

//template <typename A, typename B>
//A min(const A &a, const B &b)
//{
//    return a<b?a:b;
//}

//template <typename A, typename B>
//A max(const A &a, const B &b)
//{
//    return a>b?a:b;
//}


inline uint *
range2D(const uint rows, const uint cols, const uint rowStride, uint * const indexes)
{
    uint w = 0;

    for (uint i=0;i!=rows;++i)
    {
        const uint k = i * rowStride;

        for (uint j=0;j!=cols;++j)
        {
            indexes[w] = k + j;
            ++w;
        }
    }

    return indexes;
}

inline uint *
range2D(const uint rows, const uint cols, const uint rowStride)
{
    uint * const indexes = new uint[rows * cols];
    return range2D(rows, cols, rowStride, indexes);
}


inline uint *
range3DPlane(const uint rows, const uint cols, const uint depth,
        const uint rowStride, const uint planeStride, uint * const indexes)
{
    uint w = 0;

    for (uint d=0;d!=depth;++d)
    {
        for (uint i=0;i!=rows;++i)
        {
            const uint k = d * planeStride + i * rowStride;

            for (uint j=0;j!=cols;++j)
            {
                indexes[w] = k + j;
                ++w;
            }
        }
    }

    return indexes;
}

inline uint *
range3DPlane(const uint rows, const uint cols, const uint depth,
        const uint rowStride, const uint planeStride)
{
    uint * const indexes = new uint[rows * cols * depth];
    return range3DPlane(rows, cols, depth, rowStride, planeStride, indexes);
}

inline uint *
range3DCell(const uint rows, const uint cols, const uint depth,
        const uint rowStride, uint * const indexes)
{
    uint w = 0;
    uint k;

    for (uint i=0;i!=rows;++i)
    {
        k = i * rowStride;

        for (uint j=0;j!=cols;++j)
        {
            for (uint d=0;d!=depth;++d)
            {
                indexes[w] = k;
                ++k;
                ++w;
            }
        }
    }

    return indexes;
}

inline uint *
range3DCell(const uint rows, const uint cols, const uint depth,
        const uint rowStride)
{
    uint * const indexes = new uint[rows * cols * depth];
    return range3DCell(rows, cols, depth, rowStride, indexes);
}

} /* wup */

#endif /* __WUP__GENERIC */

