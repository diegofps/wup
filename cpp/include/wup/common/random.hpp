#ifndef __WUP_RANDOM_HPP
#define __WUP_RANDOM_HPP

#include "wup/common/generic.hpp"

#include <cstdlib>
#include <cmath>
//#include <ctime>
#include <chrono>
#include <random>

namespace wup {

class NaiveGenerator
{
public:

    bool _hasGaussianSpare;
    double _gaussianSpare;
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

    double
    uniformDouble()
    {
        return rand_r(&seed) / static_cast<double>(RAND_MAX);
    }

    double
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
        double u, v, s;
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


class StrongGenerator
{
public:

    std::mt19937 generator;
    std::normal_distribution<double> normal1;
    std::uniform_real_distribution<double> uniform1;
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

    double
    uniformDouble()
    {
        return uniform1(generator);
    }

    double
    normalDouble()
    {
        return normal1(generator);
    }

};


template <typename BASE>
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

    double uniformDouble(const double & lower, const double & upper)
    {
        return lower + uniformDouble() * (upper - lower);
    }

    double uniformDouble(const double& upper)
    {
        return uniformDouble(0.0, upper);
    }

    double uniformDouble()
    {
        return base.uniformDouble();
    }

    double normalDouble(const double& mean, const double &stdDev)
    {
        return mean + stdDev * normalDouble();
    }

    double normalDouble()
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

    inline uint *
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
    inline uint *
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
    inline uint *
    randperm3DCell(const uint rows, const uint cols, const uint depth,
               const uint rowStride)
    {
        uint * const indexes = range3DCell(rows, cols, depth, rowStride);
        shuffle(indexes, rows * cols * depth);
        return indexes;
    }

};

typedef base_random<NaiveGenerator> random;
//typedef base_random<StrongGenerator> random;

}

#endif // __WUP_RANDOM_HPP
