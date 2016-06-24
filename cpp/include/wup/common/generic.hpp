#ifndef __WUP__GENERIC
#define __WUP__GENERIC

#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <sstream>

#include <wup/common/exceptions.hpp>
#include <wup/common/msgs.hpp>

namespace wup {

typedef struct {
	double w;
	int id;
} BOX;

#ifdef __ANDROID_API__
inline double log2(const double value)
{ return log(value) / log(2.); }
#endif

inline double
sdistance(const double * const v1, const double * const v2, const int cols)
{
	double ssum = 0.0;
	for (int i=0;i<cols;++i) {
		const double v = v1[i] - v2[i];
		ssum += v * v;
	}
	return ssum;
}

inline double
distance(const double * const v1, const double * const v2, const int cols)
{ return sqrt( sdistance(v1, v2, cols) ); }

int *
randomPattern(const int length, const int n=2)
{
    auto array = new int[length];
    for (int i=0;i<length;++i)
        array[i] = rand() % n;
    return array;
}

void
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
	if (indexes) {
		for (int i=0;i<length;++i)
			printn("<", i, ":", src[i], "> ");
		print();
	} else {
		for (int i=0;i<length;++i)
			printn(src[i], " ");
		print();
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

    // TODO Needs tunning
	if (j > bottom)
		halfqsort(array, bottom, j, k);

	if (i < top && i <= k)
		halfqsort(array,  i, top, k);
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
meanNstd(const int n, const T s1, const T s2,
        T &mean, T &_std)
{
    mean = s1 / n;
    stddev(n, s1, s2, _std);
}

template<typename T>
void
meanNstd(const T *signal, const int length,
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

template <typename A, typename B>
A min(const A &a, const B &b)
{
    return a<b?a:b;
}

template <typename A, typename B>
A max(const A &a, const B &b)
{
    return a>b?a:b;
}

template <typename A>
A sum(const A * const array, const int length)
{
	A a = (A) 0;
	for (int i=0;i<length;++i)
		a += array[i];
	return a;
}

inline int *
randperm(const int n, int * const indexes)
{
    int r, t;
    //boost::random::mt19937 gen(time(NULL));
    //boost::random::uniform_real_distribution<> dist;

    for (int i=0;i<n;++i) {
        indexes[i] = i;
    }

//    std::random_shuffle(indexes, &indexes[n-1]);

    for (int i=0;i<n;++i) {
//    	r = (int)(rand() / (float) RAND_MAX * (n - i));
//    	if (r == (n - i)) --r;

        r = rand() % (n - i);
//        r = floor(dist(gen) * (n - i));
        t = indexes[r];
        indexes[r] = indexes[n-i-1];
        indexes[n-i-1] = t;
    }

    return indexes;
}

inline int *
randperm(const int n)
{
    return randperm(n, new int[n]);
}

template <typename T>
int indexOfMax(const T * const mem, const int length)
{
    int index = -1;
    T bigger;

    for (int i=0;i<length;++i) {
        if (index == -1 || mem[i] > bigger) {
            bigger = mem[i];
            index = i;
        }
    }

    return index;
}

template <typename T>
int indexOfMax(const T * const mem, const int length, int &times)
{
    int index = -1;
	times = 1;
    T bigger;

    for (int i=0;i<length;++i) {
        if (index == -1 || mem[i] > bigger) {
            bigger = mem[i];
            index = i;
            times = 1;
        } else if (mem[i] == bigger) {
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

std::vector<std::string> & split(std::vector<std::string> &elems,
        const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    
    elems.clear();
    while (std::getline(ss, item, delim))
        elems.push_back(item);
    
    return elems;
}

inline double 
parse_double(const std::string &str)
{
	std::stringstream ss(str);
	double n;
	ss >> n;
	
	if ((ss.rdstate() & std::ifstream::failbit)
			|| (ss.rdstate() & std::ifstream::badbit)) {
		std::cout << str << std::endl;
		throw ParsersException(std::string("Error while parsing double"));
	} else 
		return n;
}

inline int 
parse_int(const std::string &str)
{
	std::stringstream ss(str);
	int n;
	ss >> n;
	
	if ((ss.rdstate() & std::ifstream::failbit)
			|| (ss.rdstate() & std::ifstream::badbit))
		throw ParsersException(std::string("Error while parsing int"));
	else
		return n;
}

inline int
parse_int(const char * const str)
{ return parse_int(std::string(str)); }

inline double
parse_double(const char * const str)
{ return parse_double(std::string(str)); }

template <typename T> inline void
export_vector(std::string filename,
		const std::vector<T> & path)
{
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::out);
	for (auto point : path)
		fout << point << "\n";
}

} /* wup */

#endif /* __WUP__GENERIC */

