#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <wup/common/msgs.hpp>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

namespace wup {
namespace arr {

template <typename T>
uint *
range(uint const n,
      T * const indexes)
{
    for (uint i=0; i!=n; ++i)
        indexes[i] = i;
    return indexes;
}

inline uint *
range(uint const n)
{
    return range(n, new uint[n]);
}

inline int *
randomPattern(int const length,
              int const n=2)
{
    auto array = new int[length];
    for (int i=0;i<length;++i)
        array[i] = rand() % n;
    return array;
}

inline void
addNoise(int * const pattern,
         int const length,
         double const noise,
         int const n=2)
{
    for (int i=0;i<length;++i)
        if (rand() / double(RAND_MAX) < noise)
            pattern[i] = rand() % n;
}

template<typename T>
T *
clone(T const * const src,
      int const length)
{
    T * dst = new T[length];
    memcpy(dst, src, sizeof(T) * length);
    return dst;
}

template<typename T>
void
print(T const * const src,
      int const length,
      bool indexes=false)
{
    if (indexes)
    {
        for (int i=0;i<length;++i)
            _printn("<", i, ":", src[i], "> ");
        _print();
    }
    else
    {
        for (int i=0;i<length;++i)
            _printn(src[i], " ");
        _print();
    }
}

template<typename T>
void
print(char const * msg,
      T const * const src,
      int const length,
      bool indexes=false)
{
    std::cout << logPrefix;
    _printn(msg, ": ");
    print(src, length, indexes);
}

template <typename T>
void
binarizeAvg(T * const ptr,
            const uint len)
{
    uint32_t sum = 0;
    for (uint k=0;k<len;++k)
        sum += ptr[k];

    sum /= len;

    for (uint k=0;k<len;++k)
        ptr[k] = ptr[k] >= sum ? 0 : UINT8_MAX;
}

inline void
binarizeAvg(uint8_t * const ptr,
            uint32_t const patternLen,
            uint8_t * dst=nullptr)
{
    if (dst == nullptr)
        dst = ptr;

    uint sum = 0;

    for (uint k=0;k!=patternLen;++k)
        sum += ptr[k];

    sum = sum / patternLen;

    for (uint k=0;k!=patternLen;++k)
        ptr[k] = ptr[k] >= sum ? 0 : UINT8_MAX;
}

template <typename T>
T *
filter(T const * const src,
       T * const dst,
       const uint32_t * const indexes,
       const uint32_t indexesLen)
{
    for (uint k=0;k!=indexesLen;++k)
        dst[k] = src[indexes[k]];
    return dst;
}


template <typename T, typename SIZE>
int indexOfMax(T const * const mem,
               SIZE const & length)
{
    int index = -1;
    T bigger;

    for (SIZE i=0;i!=length;++i)
    {
        if (index == -1 || mem[i] > bigger)
        {
            bigger = mem[i];
            index = i;
        }
    }

    return index;
}

template <typename T>
int
indexOfMax(std::vector<T> const & data)
{
    return indexOfMax(data.data(), data.size());
}

template <typename T, typename SIZE>
int indexOfMax(T const * const mem,
               SIZE const & length,
               int &times)
{
    int index = -1;
    times = 1;
    T bigger;

    for (SIZE i=0;i!=length;++i)
    {
        if (index == -1 || mem[i] > bigger)
        {
            bigger = mem[i];
            index = i;
            times = 1;
        }
        else if (mem[i] == bigger)
        {
            ++times;
        }
    }

    return index;
}

template <typename T, typename SIZE>
void indexOfMax2(T const * const mem,
                 SIZE const & length,
                 int & i1,
                 int & i2)
{
    i1 = -1;
    i2 = -1;

    for (SIZE i=0;i!=length;++i)
    {
        if (i1 == -1 || mem[i] > mem[i1])
        {
            i2 = i1;
            i1 = i;
        }

        else if (i2 == -1 || mem[i] == mem[i2])
        {
            i2 = i;
        }
    }
}

template <typename T>
int indexOfMin(T const * const mem,
               int const length)
{
    int index = -1;
    T bigger;

    for (int i=0;i!=length;++i)
    {
        if (index == -1 || mem[i] < bigger)
        {
            bigger = mem[i];
            index = i;
        }
    }

    return index;
}

template <typename T>
int
indexOfMin(const std::vector<T> & data)
{
    return indexOfMin(data.data(), data.size());
}

template <typename T>
int indexOfMin(T const * const mem,
               int const length,
               int & times)
{
    int index = -1;
    times = 1;
    T bigger;

    for (int i=0;i!=length;++i)
    {
        if (index == -1 || mem[i] < bigger)
        {
            bigger = mem[i];
            index = i;
            times = 1;
        }

        else if (mem[i] == bigger)
        {
            ++times;
        }
    }

    return index;
}

template <typename T, typename SIZE>
void indexOfMin2(T const * const mem,
                 SIZE const & length,
                 int & i1,
                 int & i2)
{
    i1 = -1;
    i2 = -1;

    for (SIZE i=0;i!=length;++i)
    {
        if (i1 == -1 || mem[i] < mem[i1])
        {
            i2 = i1;
            i1 = i;
        }

        else if (i2 == -1 || mem[i] == mem[i2])
        {
            i2 = i;
        }
    }
}

template <typename A>
A
sum(A const * const array,
    int const length)
{
    A tmp = 0;
    for (int i=0;i!=length;++i)
        tmp += array[i];
    return tmp;
}

template <typename A>
A
max(A const * const array,
    int const length)
{
    return array[indexOfMax(array, length)];
}

template <typename A>
A
min(A const * const array,
      int const length)
{
    return array[indexOfMin(array, length)];
}

template <typename A>
double
mean(A const * const array,
     int const length)
{
    A tmp = array[0];
    for (int i=1;i<length;++i) tmp += array[i];
    return tmp / double(length);
}

template <typename A, typename B>
void
meanNStd(B const * const data,
         size_t const len,
         A & mean,
         A & std)
{
    A s1 = 0;
    A s2 = 0;

    for (size_t i=0;i!=len;++i)
    {
        s1 += data[i];
        s2 += data[i] * data[i];
    }

    meanNstd(len, s1, s2, mean, std);
}

template <typename A, typename B>
void
stats(B const * const data,
      size_t const len,
      A & minimum,
      A & maximum,
      A & mean,
      A & std)
{
    if (len == 0)
    {
        minimum = 0;
        maximum = 0;
        mean = 0;
        std = 0;
    }
    else
    {
        minimum = arrayMin(data, len);
        maximum = arrayMax(data, len);
        arrayMeanNStd(data, len, mean, std);
    }
}

template <typename T>
void
reverse(T * const data,
        size_t const length)
{
    T tmp;

    for (int i=0;i!=length/2;++i)
    {
        tmp = data[i];
        data[i] = data[length-i-1];
        data[length-i-1] = tmp;
    }
}

template<typename T>
void
save(std::string const filename,
     T const * const array,
     int const length)
{
    std::ofstream file_out( filename.c_str() );

    for ( int i=0;i<length;++i )
        file_out << array[i] << std::endl;
}

//////////////////////
// Vector Functions //
//////////////////////

template <typename A>
A
sum(std::vector<A> const & data)
{
    return sum(data.data(), data.size());
}

template <typename A>
A
max(std::vector<A> const & data)
{
    return max(data.data(), data.size());
}

template <typename A>
A
min(std::vector<A> const & data)
{
    return min(data.data(), data.size());
}

template <typename A, typename B>
void
stats(std::vector<B> const & data,
      A & minimum,
      A & maximum,
      A & mean,
      A & std)
{
    stats(data.data(), data.size(), minimum, maximum, mean, std);
}

template <typename T>
size_t
load(std::string const filename,
     std::vector<T> & output)
{
    std::ifstream fin;
    T tmp;

    fin.open(filename, std::ios::in);

    if (!fin.good())
        throw WUPException(cat( "Could not open file \'", filename, "\'" ) );

    while (fin >> tmp)
        output.push_back(tmp);

    return output.size();
}

template <typename T> inline
void
save(std::string const filename,
     std::vector<T> const & path)
{
    std::ofstream fout;

    fout.open(filename.c_str(), std::ios::out);

    for (auto point : path)
        fout << point << "\n";
}

template <typename T>
void
reverse(std::vector<T> & data)
{
    reverse(&data[0], data.size());
}

template <typename T>
void
print(std::vector<T> const & data)
{
    arr::print(&data[0], data.size());
}

template <typename T>
void
print(char const * const prefix,
      std::vector<T> const & data)
{
    arr::print(prefix, &data[0], data.size());
}


}
}

#endif // ARRAY_HPP
