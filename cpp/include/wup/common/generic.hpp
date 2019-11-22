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

#include <wup/common/exceptions.hpp>
#include <wup/common/msgs.hpp>

using namespace std;

namespace wup {

typedef struct _BOX {

    uint id;

    double w;

    _BOX()
    {

    }

    _BOX(const uint _id, const double _w) : id(_id), w(_w)
    {

    }

    bool operator<(const wup::_BOX & other)
    {
        return w < other.w;
    }

} BOX;

#ifdef __ANDROID_API__
inline double log2(const double value)
{ return log(value) / log(2.); }
#endif

template <typename T, typename B>
void
prevIndex(T & index, const B & size, bool loop=true)
{
    if (index == 0)
    {
        if (loop)
            index = size - 1;
    }
    else
        --index;
}

void
intersect1D(const int & a1, const int & aw,
            const int & b1, const int & bw,
            int & c, int & cw)
{
    const int a2 = a1 + aw;
    const int b2 = b1 + bw;

    if (a2 <= b1)
    {
        c  = 0;
        cw = 0;
    }

    else if (a2 <= b2)
    {
        if (a1 <= b1)
        {
            c  = b1;
            cw = a2 - b1;
        }
        else
        {
            c  = a1;
            cw = a2 - a1;
        }
    }

    else if (a1 <= b1)
    {
        c  = b1;
        cw = bw;
    }

    else if (a1 <= b2)
    {
        c  = a1;
        cw = b2 - a1;
    }

    else
    {
        c  = 0;
        cw = 0;
    }
}

template <typename T, typename B>
void
nextIndex(T & index, const B & size, bool loop=true)
{
    if (++index == size)
        index = loop ? 0 : size-1;
}

inline uint32_t
rotl32 (uint32_t n, uint c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n<<c) | (n>>( (-c)&mask ));
}

inline uint32_t
rotr32 (uint32_t n, uint c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}

template <typename T>
T
rotl (T n, uint c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n<<c) | (n>>( (-c)&mask ));
}

template <typename T>
T
rotr (T n, uint c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}

inline void
binarizeAvg(uint8_t * const ptr, const uint patternLen, uint8_t * dst=nullptr)
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

inline void
binarizeAvg(uint8_t * const ptr, const uint numValues, const uint patternLen)
{
    uint current = 0;

    for (uint i=0; i!=numValues; ++i)
    {
        binarizeAvg(ptr + current, patternLen);
        current += patternLen;
    }
}

inline long 
factorial(long n)
{
    if (n <= 1) return 1;
    
    long r = 1;
    for (r=1; n!=1; r*=n, --n);
    return r;
}

template <typename T>
inline double
sdistance(const double * const v1, const double * const v2, const T cols)
{
    const uint size = uint(cols);
    double ssum = 0.0;

    for (uint i=0;i!=size;++i)
    {
        const double v = v1[i] - v2[i];
        ssum += v * v;
    }

    return ssum;
}

inline void
createPath(const std::string & folderpath)
{
    system(cat("mkdir -p ", folderpath).c_str());
}

inline double
distance(const double * const v1, const double * const v2, const int cols)
{ return sqrt( sdistance(v1, v2, cols) ); }

inline int *
randomPattern(const int length, const int n=2)
{
    auto array = new int[length];
    for (int i=0;i<length;++i)
        array[i] = rand() % n;
    return array;
}

inline void
addNoise(int * const pattern, const int length, 
    const double noise, const int n=2)
{
    for (int i=0;i<length;++i)
        if (rand() / double(RAND_MAX) < noise)
            pattern[i] = rand() % n;
}

template<typename T>
T *
cloneArray(const T * const src, const int length)
{
    T * dst = new T[length];
    memcpy(dst, src, sizeof(T) * length);
    return dst;
}

template<typename T>
void
printArray(const T * const src, const int length, bool indexes=false)
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
printArray(const char * msg, const T * const src, const int length, bool indexes=false)
{
    std::cout << logPrefix;
    _printn(msg, ": ");
    printArray(src, length, indexes);
}

template <typename T>
void
binarizeAvg(T * const ptr, const uint len)
{
    uint32_t sum = 0;
    for (uint k=0;k<len;++k)
        sum += ptr[k];

    sum /= len;

    for (uint k=0;k<len;++k)
        ptr[k] = ptr[k] >= sum ? 0 : UINT8_MAX;
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

inline double
logistic(const double x)
{
    return 1.0 / (1.0 + exp(-x));
}

template <typename T>
void
stddev(const int n, const T s1, const T s2, T &_std)
{
    _std = sqrt( (s2-s1*s1/n) / (n-1) );
}

template <typename T>
void
meanNstd(const int & n, const T s1, const T s2,
        T & mean, T & _std)
{
    mean = s1 / n;
    stddev(n, s1, s2, _std);
}

template<typename T>
void
meanNstd(const T * signal, const int length,
        double &mean, double &_std)
{
    double s1 = 0.0;
    double s2 = 0.0;

    for (int i=0;i<length;++i) {
        s1 += signal[i];
        s2 += signal[i] * signal[i];
    }

    meanNstd(length, s1, s2, mean, _std);
}


template <typename A>
A abs(const A &a)
{
    return a<0?-a:a;
}

template <typename P>
const P wmin(const P &p1)
{
    return p1;
}

template <typename P, typename... Args>
const P wmin(const P &p1, const Args&... args)
{
    const P p2 = wmin(args...);

    if (p1 < p2)
        return p1;
    else
        return p2;
}

template <typename P>
const P wmax(const P &p1)
{
    return p1;
}

template <typename P, typename... Args>
const P wmax(const P &p1, const Args&... args)
{
    const P p2 = wmax(args...);

    if (p1 > p2)
        return p1;
    else
        return p2;
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

template <typename A>
A sum(const A * const array, const int length)
{
    A a = (A) 0;
    for (int i=0;i<length;++i)
        a += array[i];
    return a;
}


template <typename T>
T *
shuffle(T * const data, const uint32_t n)
{
    for (uint32_t i=0;i!=n-1;++i)
    {
        const uint32_t pivot = rand() % (n - i);
        const uint32_t final  = n-i-1;

        const T tmp = data[pivot];
        data[pivot] = data[final];
        data[final] = tmp;
    }

    return data;
}


template <typename T>
uint *
range(const uint n, T * const indexes)
{
    for (uint i=0; i!=n; ++i)
        indexes[i] = i;
    return indexes;
}

inline uint *
range(const uint n)
{
    return range(n, new uint[n]);
}


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


template <typename T>
inline T *
randperm(const uint n, T * const indexes)
{
    range(n, indexes);
    shuffle(indexes, n);
    return indexes;
}

inline uint *
randperm(const uint n)
{
    return randperm(n, new uint[n]);
}

template <typename T>
T *
filter(const T * const src, T * const dst, const uint * const indexes, const uint indexesLen)
{
    for (uint k=0;k!=indexesLen;++k)
        dst[k] = src[indexes[k]];
    return dst;
}

inline uint *
randperm2D(const uint rows, const uint cols, const uint rowStride)
{
    uint * const indexes = range2D(rows, cols, rowStride);
    wup::shuffle(indexes, rows * cols);
    return indexes;
}

/*
    Generates a random permutation of indexes for a 3D region of interest with size
    [depth x rows x cols], which is inside a matrix of size [depth x Rows x Cols].

    This function is plain oriented, which means that each value of depth contains
    the an entire Plane in the original matrix [Rows, Cols]. If you want to apply this
    with opencv, consider the function randperm3DPlane.

    rows - Number of rows in the region of interest
    cols - number of columns in the region of interest
    depth - depth of the cell, or the number of planes in the matrix.
    rowStride - Number of elements in the Row of the full matrix.
    planeStride - Number of elements in the Plane of the full matrix. Like Rows * Cols.
*/
inline uint *
randperm3DPlane(const uint rows, const uint cols, const uint depth,
           const uint rowStride, const uint planeStride)
{
    uint * const indexes = range3DPlane(rows, cols, depth, rowStride, planeStride);
    wup::shuffle(indexes, rows * cols * depth);
    return indexes;
}

/*
    Generates a random permutation of indexes for a 3D region of interest with size
    [rows x cols x depth], which is inside a matrix of size [Rows x Cols X depth].

    This function assumes that each cell in [rows, cols] contains depth elements.
    This is the format used by opencv, for instance.

    rows - Number of rows in the region of interest
    cols - number of columns in the region of interest
    depth - depth of the cell, or the number of elements in it. For RGB it is 3.
    rowStride - Number of elements in the Row of the full matrix. Like Rows * depth.
*/
inline uint *
randperm3DCell(const uint rows, const uint cols, const uint depth,
           const uint rowStride)
{
    uint * const indexes = range3DCell(rows, cols, depth, rowStride);
    wup::shuffle(indexes, rows * cols * depth);
    return indexes;
}


inline string
slice(const string str, int a, int b)
{
    const int len = int(str.length());
    if (a < 0) a = len + a;
    if (b < 0) b = len + b;
    return str.substr(a, b - a);
}

inline string
slice_from(const string str, int a)
{
    const int len = int(str.length());
    if (a < 0) a = len + a;
    return slice(str, a, len);
}

inline string
slice_to(const string str, int b)
{
    const int len = int(str.length());
    if (b < 0) b = len + b;
    return slice(str, 0, b);
}


template <typename T, typename SIZE>
int indexOfMax(const T * const mem, const SIZE & length)
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

template <typename T, typename SIZE>
int indexOfMax(const T * const mem, const SIZE & length, int &times)
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
void indexOfMax2(const T * const mem, const SIZE & length, int &i1, int &i2)
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
int indexOfMin(const T * const mem, const int length)
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
int indexOfMin(const T * const mem, const int length, int & times)
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

template <typename A>
A arrayMax(const A * const array, const int length)
{
    return array[indexOfMax(array, length)];
}

template <typename A>
A arrayMin(const A * const array, const int length)
{
    return array[indexOfMin(array, length)];
}

template <typename A>
double arrayMean(const A * const array, const int length)
{
    A tmp = array[0];
    for (int i=1;i<length;++i) tmp += array[i];
    return tmp / double(length);
}

inline std::vector<int>
load_array(const std::string filename)
{
    FILE *f = fopen(filename.c_str(), "r");
    if (!f) throw FatalException(
        cat( "Could not open file \'", filename, "\'" ) );

    std::vector<int> result;
    int tmp;

    while(fscanf(f, "%d", &tmp) == 1)
        result.push_back(tmp);

    return result;
}

template<typename T>
void save_array(const std::string filename, const T * const array, const int length)
{
    std::ofstream file_out( filename.c_str() );
    for ( int i=0;i<length;++i )
        file_out << array[i] << std::endl;
}

template<typename T>
T load_value(const std::string filename)
{
    std::ifstream file_in( filename.c_str() );
    if (!file_in.good()) throw FatalException(
        cat( "Could not open file \'", filename, "\'" ) );

    T tmp;
    file_in >> tmp;
    return tmp;
}

inline std::vector<std::string> &
split(const std::string & s, char delim, std::vector<std::string> & elems)
{
    std::stringstream ss(s);
    std::string item;
    
    elems.clear();
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    
    return elems;
}

inline bool
parse_bool(const std::string &str)
{
    if (str.size() == 0)
        throw ParsersException(std::string("Error while parsing bool"));

    const char & firstChar = str[0];
    return firstChar == 'y' || firstChar == 'Y' || firstChar == 's' || firstChar == 'S' || firstChar == 't' || firstChar == 'T' || firstChar == '1';
}

template <typename T>
T parse_basic_type(const std::string & str)
{
    std::stringstream ss(str);
    T n;
    ss >> n;

    if ((ss.rdstate() & std::ifstream::failbit) || (ss.rdstate() & std::ifstream::badbit))
    {
        std::cout << str << std::endl;
        throw ParsersException(std::string("Error while parsing double"));
    }
    else
    {
        return n;
    }
}

template <typename T>
T parse_basic_type(const char * const str)
{
    return parse_basic_type<T>(std::string(str));
}

inline double parse_double(const std::string &str) { return parse_basic_type<double>(str); }
inline double parse_double(const char * const str) { return parse_basic_type<double>(str); }

inline long parse_long(const std::string &str) { return parse_basic_type<long>(str); }
inline long parse_long(const char * const str) { return parse_basic_type<long>(str); }

inline ulong parse_ulong(const std::string &str) { return parse_basic_type<ulong>(str); }
inline ulong parse_ulong(const char * const str) { return parse_basic_type<ulong>(str); }

inline int parse_int(const std::string &str) { return parse_basic_type<int>(str); }
inline int parse_int(const char * const str) { return parse_basic_type<int>(str); }

inline uint parse_uint(const std::string &str) { return parse_basic_type<uint>(str); }
inline uint parse_uint(const char * const str) { return parse_basic_type<uint>(str); }

inline float parse_float(const std::string &str) { return parse_basic_type<float>(str); }
inline float parse_float(const char * const str) { return parse_basic_type<float>(str); }

inline short parse_short(const std::string &str) { return parse_basic_type<short>(str); }
inline short parse_short(const char * const str) { return parse_basic_type<short>(str); }

inline ushort parse_ushort(const std::string &str) { return parse_basic_type<ushort>(str); }
inline ushort parse_ushort(const char * const str) { return parse_basic_type<ushort>(str); }

template <typename T> inline void
export_vector(std::string filename,
        const std::vector<T> & path)
{
    std::ofstream fout;
    fout.open(filename.c_str(), std::ios::out);
    for (auto point : path)
        fout << point << "\n";
}

inline std::vector<std::string> &
dirFiles(const char * path, std::vector<std::string> &list)
{
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir (path)) == NULL)
        throw WUPException(cat("Directory '", path, "' does not exists"));
    
    while ((ent = readdir (dir)) != NULL) {
        std::string tmp(ent->d_name);
        
        if (tmp == "." || tmp == "..")
            continue;
        
        list.push_back(cat(path, tmp));
    }
    
    closedir(dir);
    return list;
}

inline std::vector<std::string> &
dirFiles(const std::string & path, std::vector<std::string> &list)
{
    return dirFiles(path.c_str(), list);
}

} /* wup */

inline uint32_t
swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

#endif /* __WUP__GENERIC */

