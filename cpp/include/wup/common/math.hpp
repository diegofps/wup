#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

namespace wup {
namespace math {

inline void
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
sdistance(const double * const v1,
          const double * const v2,
          const T cols)
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

inline double
distance(const double * const v1,
         const double * const v2,
         const int cols)
{
    return sqrt( sdistance(v1, v2, cols) );
}

inline double
logistic(const double x)
{
    return 1.0 / (1.0 + exp(-x));
}

template <typename T>
void
stddev(const int n,
       const T s1,
       const T s2,
       T &_std)
{
    _std = sqrt( (s2-s1*s1/n) / (n-1) );
}

template <typename T>
void
meanNstd(int const & n,
         T const s1,
         T const s2,
         T & mean,
         T & _std)
{
    mean = s1 / n;
    stddev(n, s1, s2, _std);
}

template<typename T>
void
meanNstd(T const * signal,
         int const length,
         double & mean,
         double & _std)
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
A abs(A const & a)
{
    return a<0?-a:a;
}

template <typename P>
const P min(P const & p1)
{
    return p1;
}

template <typename P, typename... Args>
const P min(P const & p1,
            Args const & ... args)
{
    P const & p2 = min(args...);
    return p1 < p2 ? p1 : p2;
}

template <typename P>
P const
max(P const & p1)
{
    return p1;
}

template <typename P, typename... Args>
P const
max(P const & p1,
    Args const & ... args)
{
    P const & p2 = max(args...);
    return p1 > p2 ? p1 : p2;
}

}
}

#endif // MATH_HPP
