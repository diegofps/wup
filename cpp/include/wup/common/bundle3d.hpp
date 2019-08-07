#ifndef BUNLDE3D_HPP
#define BUNLDE3D_HPP

#include <cstring>
#include <vector>
#include <cstdint>

namespace wup {

template <typename T>
class Bundle3D {
public:

    uint32_t _dim1;

    uint32_t _dim2;

    uint32_t _dim3;

    T * _data;

    bool _ownerOfData;

public:

    Bundle3D() : Bundle3D(1, 1, 1)
    {

    }

    Bundle3D(const uint32_t dim1, const uint32_t dim2, const uint32_t dim3) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(dim3),
        _data(new T[dim1 * dim2 * dim3]()),
        _ownerOfData(true)
    {

    }

    Bundle3D(T * data, const uint32_t dim1, const uint32_t dim2, const uint32_t dim3) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(dim3),
        _data(data),
        _ownerOfData(false)
    {

    }

    ~Bundle3D()
    {
        if (_ownerOfData)
            delete [] _data;
    }

    Bundle3D<T> &
    operator=(const Bundle3D<T> & other)
    {
        if (_ownerOfData)
            delete [] _data;

        _dim1 = other._dim1;
        _dim2 = other._dim2;
        _dim3 = other._dim3;
        _data = other._data;
        _ownerOfData = false;

        return *this;
    }

    T &
    operator()(const uint32_t i1, const uint32_t i2, const uint32_t i3)
    {
        return _data[i1 * (_dim2 * _dim3) + i2 * (_dim3) + i3];
    }

    const T &
    operator()(const uint32_t i1, const uint32_t i2, const uint32_t i3) const
    {
        return _data[i1 * (_dim2 * _dim3) + i2 * (_dim3) + i3];
    }

    T *
    data()
    {
        return _data;
    }

    const T *
    data() const
    {
        return _data;
    }

    uint64_t
    size() const
    {
        return _dim1 * _dim2 * _dim3;
    }

    void
    operator=(const T & value)
    {
        for (uint64_t i=0;i!=size();++i)
            _data[i] = value;
    }

    void
    clear()
    {
        memset(_data, 0, sizeof(T) * size());
    }

    void
    reshape(const uint32_t dim1, const uint32_t dim2, const uint32_t dim3, bool preserveOldData=false)
    {
        const uint64_t oldSize = size();

        if (dim1 != _dim1)
            _dim1 = dim1;

        if (dim2 != _dim2)
            _dim2 = dim2;

        if (dim3 != _dim3)
            _dim3 = dim3;

        const uint64_t newSize = uint64_t(_dim1) * uint64_t(_dim2) * uint64_t(_dim3);

        if (newSize > oldSize)
        {
            T * newData = new T[newSize];

            if (preserveOldData)
                std::copy(_data, _data + oldSize, newData);

            if (_ownerOfData)
                delete [] _data;

            _data = newData;
        }
    }

    uint32_t
    dim1() const
    {
        return _dim1;
    }

    uint32_t
    dim2() const
    {
        return _dim2;
    }

    uint32_t
    dim3() const
    {
        return _dim3;
    }

}; // Bundle3D

template <typename T>
T
wmin(const Bundle3D<T> & data)
{
    return arrayMin(&data(0,0,0), data.size());
}

template <typename T>
T
wmax(const Bundle3D<T> & data)
{
    return arrayMax(&data(0,0,0), data.size());
}

} // wup

#endif
