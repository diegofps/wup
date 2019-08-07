#ifndef __WUP_RANDOM_HPP
#define __WUP_RANDOM_HPP

#include <cstdlib>
#include <cmath>
//#include <ctime>
#include <chrono>

namespace wup {

class random {
private:

    double _gaussianSpare;

    bool _hasGaussianSpare;

public:

    random() : _hasGaussianSpare(false)
    {
        //srand(time(NULL));
        srand(std::chrono::system_clock::now().time_since_epoch().count());
    }

    bool fairCoin() const
    {
        return rand() % 2;
    }

    int uniformIntNoise(const int& lower, const int& upper) const
    {
        return lower + rand() % (upper-lower);
    }

    int uniformIntNoise(const int& upper) const
    {
        return rand() % upper;
    }

    int uniformIntNoise() const
    {
        return rand();
    }

    double uniformNoise(const double& lower, const double& upper) const
    {
        return lower + uniformNoise() * (upper - lower);
    }

    double uniformNoise(const double& upper) const
    {
        return uniformNoise(0.0, upper);
    }

    double uniformNoise() const
    {
        return rand() / static_cast<double>(RAND_MAX);
    }

    double gaussianNoise(const double& mean, const double &stdDev)
    {
        return mean + stdDev * gaussianNoise();
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

};

}

#endif // __WUP_RANDOM_HPP
