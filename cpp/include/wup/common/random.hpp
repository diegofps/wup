#ifndef __WUP_RANDOM_HPP
#define __WUP_RANDOM_HPP

#include "wup/common/generic.hpp"

#include <cstdlib>
#include <cmath>
//#include <ctime>
#include <chrono>
#include <random>

namespace wup {

class random {
private:

    double _gaussianSpare;

    bool _hasGaussianSpare;

//    std::default_random_engine generator;
    std::mt19937 generator;
    std::normal_distribution<long double> distribution;
    std::uniform_real_distribution<long double> dist2;

    uint seed;

public:

    random() :
        _hasGaussianSpare(false),
        distribution(0.0,1.0),
        dist2(0.0,1.0)
    {
        //srand(time(NULL));
//        srand(std::chrono::system_clock::now().time_since_epoch().count());
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }

    bool fairCoin()
    {
        return rand_r(&seed) % 2;
    }

    bool unfairCoin(const double & v)
    {
        return rand_r(&seed) / static_cast<double>(RAND_MAX) < v;
    }

    int uniformIntNoise(const int& lower, const int& upper)
    {
        return lower + rand_r(&seed) % (upper-lower);
    }

    int uniformIntNoise(const int& upper)
    {
        return rand_r(&seed) % upper;
    }

    int uniformIntNoise()
    {
        return rand_r(&seed);
    }

    double uniformNoise(const double& lower, const double& upper)
    {
        return lower + uniformNoise() * (upper - lower);
    }

    double uniformNoise(const double& upper)
    {
        return uniformNoise(0.0, upper);
    }

    long double uniformNoise()
    {
        return rand_r(&seed) / static_cast<double>(RAND_MAX);
//        return dist2(generator);
    }

    double gaussianNoise(const double& mean, const double &stdDev)
    {
        return mean + stdDev * gaussianNoise();
    }

    long double gaussianNoise2()
    {
        return distribution(generator);
    }

    double gaussianNoise()
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
            u = uniformNoise() * 2.0 - 1.0;
            v = uniformNoise() * 2.0 - 1.0;
            s = u * u + v * v;
        }
        while( (s >= 1.0) || (s == 0.0) );

        s = sqrt(-2.0 * log(s) / s);
        _gaussianSpare = v * s;

        return u * s;
    }

    template <typename T>
    T *
    shuffle(T * const data, const uint32_t n)
    {
        for (uint32_t i=0;i!=n-1;++i)
        {
            const uint32_t pivot = rand_r(&seed) % (n - i);
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

}

#endif // __WUP_RANDOM_HPP
