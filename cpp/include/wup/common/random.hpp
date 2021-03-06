#ifndef __WUP_RANDOM_HPP
#define __WUP_RANDOM_HPP

#include "wup/common/generic.hpp"
#include "wup/common/ranges.hpp"
#include "wup/common/array.hpp"
#include "wup/common/str.hpp"

#include <cstdlib>
#include <cmath>
//#include <ctime>
#include <chrono>
#include <random>

namespace wup {

template <typename PRECISION>
class NaiveGenerator
{
public:

    bool _hasGaussianSpare;
    PRECISION _gaussianSpare;
    uint seed;

public:

    NaiveGenerator() :
        _hasGaussianSpare(false)
    {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }

    int
    uniformInt()
    {
        return rand_r(&seed);
    }

    PRECISION
    uniformDouble()
    {
        return rand_r(&seed) / static_cast<PRECISION>(RAND_MAX);
    }

    PRECISION
    normalDouble()
    {
        // Based on Marsaglia Polar Method
        // https://en.wikipedia.org/wiki/Marsaglia_polar_method
        // http://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/

        if(_hasGaussianSpare)
        {
            _hasGaussianSpare = false;
            return _gaussianSpare;
        }

        _hasGaussianSpare = true;
        PRECISION u, v, s;
        do
        {
            u = uniformDouble() * 2.0 - 1.0;
            v = uniformDouble() * 2.0 - 1.0;
            s = u * u + v * v;
        }
        while( (s >= 1.0) || (s == 0.0) );

        s = sqrt(-2.0 * log(s) / s);
        _gaussianSpare = v * s;

        return u * s;
    }

};


template <typename PRECISION>
class StrongGenerator
{
public:

    std::mt19937 generator;
    std::normal_distribution<PRECISION> normal1;
    std::uniform_real_distribution<PRECISION> uniform1;
    std::uniform_int_distribution<int> uniform2;

public:

    StrongGenerator() :
        normal1(0.0,1.0),
        uniform1(0.0,1.0),
        uniform2(0,RAND_MAX)
    {

    }

    int
    uniformInt()
    {
        return uniform2(generator);
    }

    PRECISION
    uniformDouble()
    {
        return uniform1(generator);
    }

    PRECISION
    normalDouble()
    {
        return normal1(generator);
    }

};


template <typename BASE, typename PRECISION>
class base_random {
private:

    BASE base;

public:


    ////////////////////////////////////////////////////////////////////////////////////////
    // Coin generators
    ////////////////////////////////////////////////////////////////////////////////////////

    bool fairCoin()
    {
        return base.uniformInt() % 2;
    }

    bool unfairCoin(const double & v)
    {
        return base.uniformInt() < v * RAND_MAX;
    }


    ////////////////////////////////////////////////////////////////////////////////////////
    // Int generators
    ////////////////////////////////////////////////////////////////////////////////////////

    int uniformInt(const int & lower, const int & upper)
    {
        return lower + base.uniformInt() % (upper - lower);
    }

    int uniformInt(const int & upper)
    {
        return base.uniformInt() % upper;
    }

    int uniformInt()
    {
        return base.uniformInt();
    }


    ////////////////////////////////////////////////////////////////////////////////////////
    // Double generators
    ////////////////////////////////////////////////////////////////////////////////////////

    PRECISION uniformDouble(const PRECISION & lower, const PRECISION & upper)
    {
        return lower + uniformDouble() * (upper - lower);
    }

    PRECISION uniformDouble(const double& upper)
    {
        return uniformDouble(0.0, upper);
    }

    PRECISION uniformDouble()
    {
        return base.uniformDouble();
    }

    PRECISION normalDouble(const double& mean, const double &stdDev)
    {
        return mean + stdDev * normalDouble();
    }

    PRECISION normalDouble()
    {
        return base.normalDouble();
    }


    ////////////////////////////////////////////////////////////////////////////////////////
    // Arrays and matrix generators
    ////////////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    T *
    shuffle(T * const data, const uint32_t n)
    {
        for (uint32_t i=0;i!=n-1;++i)
        {
            const uint32_t pivot = base.uniformInt() % (n - i);
            const uint32_t final  = n-i-1;

            const T tmp = data[pivot];
            data[pivot] = data[final];
            data[final] = tmp;
        }

        return data;
    }

    template <typename T>
    std::vector<T> &
    shuffle(std::vector<T> & data)
    {
        shuffle(data.data(), data.size());
        return data;
    }

    template <typename T>
    T *
    randperm(const uint n, T * const indexes)
    {
        arr::range(n, indexes);
        shuffle(indexes, n);
        return indexes;
    }

    uint *
    randperm(const uint n)
    {
        return randperm(n, new uint[n]);
    }

    uint *
    randperm2D(const uint rows, const uint cols, const uint rowStride)
    {
        uint * const indexes = range2D(rows, cols, rowStride);
        shuffle(indexes, rows * cols);
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
    uint *
    randperm3DPlane(const uint rows, const uint cols, const uint depth,
               const uint rowStride, const uint planeStride)
    {
        uint * const indexes = range3DPlane(rows, cols, depth, rowStride, planeStride);
        shuffle(indexes, rows * cols * depth);
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
    uint *
    randperm3DCell(const uint rows, const uint cols, const uint depth,
               const uint rowStride)
    {
        uint * const indexes = range3DCell(rows, cols, depth, rowStride);
        shuffle(indexes, rows * cols * depth);
        return indexes;
    }

    int *
    randomPattern(int const length,
                  int const n=2)
    {
        auto array = new int[length];
        for (int i=0;i<length;++i)
            array[i] = uniformInt(n);
        return array;
    }

    void
    randomPattern(std::vector<int> & data,
                  int const n=2)
    {
        for (size_t i=0;i<data.size();++i)
            data[i] = uniformInt(n);
    }

    void
    addNoise(int * const pattern,
             int const length,
             double const noise,
             int const n=2)
    {
        for (int i=0;i<length;++i)
            if (unfairCoin(noise))
                pattern[i] = uniformInt(n);
    }

    template <typename T>
    void
    addNoiseFlip(T * const pattern,
                 int const length,
                 double const noise,
                 T const max=1)
    {
        for (int i=0;i<length;++i)
            if (unfairCoin(noise))
                pattern[i] = max - pattern[i];
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // Strings
    ////////////////////////////////////////////////////////////////////////////////////////

    std::string
    uuid()
    {
        std::string tmp = boost::uuids::to_string(boost::uuids::random_generator()());
        str::removeChar(tmp, '-');
        return tmp;
    }

};

typedef base_random<NaiveGenerator<double>, double> random;
//typedef base_random<StrongGenerator<double>, double> random;

}

#endif // __WUP_RANDOM_HPP
