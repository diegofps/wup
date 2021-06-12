#ifndef BUNLDE3D_HPP
#define BUNLDE3D_HPP

#include <cstring>
#include <vector>
#include <cstdint>

#include <wup/common/sbio.hpp>

namespace wup {

template <typename T>
class Bundle3D {
private:

    uint32_t _dim1;
    uint32_t _dim2;
    uint32_t _dim3;
    uint32_t _planeSize;
    T * _data;
    bool _ownerOfData;

public:

    Bundle3D() :
        Bundle3D(1, 1, 1)
    {

    }

    Bundle3D(const uint32_t dim1) :
        _dim1(dim1),
        _dim2(1),
        _dim3(1),
        _planeSize(_dim2*_dim3),
        _data(new T[_dim1 * _dim2 * _dim3]()),
        _ownerOfData(true)
    {

    }

    Bundle3D(const uint32_t dim1, const uint32_t dim2) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(1),
        _planeSize(_dim2*_dim3),
        _data(new T[_dim1 * _dim2 * _dim3]()),
        _ownerOfData(true)
    {

    }

    Bundle3D(const uint32_t dim1, const uint32_t dim2, const uint32_t dim3) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(dim3),
        _planeSize(_dim2*_dim3),
        _data(new T[_dim1 * _dim2 * _dim3]()),
        _ownerOfData(true)
    {

    }

    Bundle3D(T * data, const uint32_t dim1) :
        _dim1(dim1),
        _dim2(1),
        _dim3(1),
        _planeSize(_dim2*_dim3),
        _data(data),
        _ownerOfData(false)
    {

    }

    Bundle3D(T * data, const uint32_t dim1, const uint32_t dim2) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(1),
        _planeSize(_dim2*_dim3),
        _data(data),
        _ownerOfData(false)
    {

    }

    Bundle3D(T * data, const uint32_t dim1, const uint32_t dim2, const uint32_t dim3) :
        _dim1(dim1),
        _dim2(dim2),
        _dim3(dim3),
        _planeSize(_dim2*_dim3),
        _data(data),
        _ownerOfData(false)
    {

    }

    ~Bundle3D()
    {
        if (_ownerOfData)
            delete [] _data;
    }

    Bundle3D(IntReader & reader) :
        _dim1(reader.getUInt32()),
        _dim2(reader.getUInt32()),
        _dim3(reader.getUInt32()),
        _planeSize(_dim2*_dim3),
        _data(new T[_dim1 * _dim2 * _dim3]()),
        _ownerOfData(true)
    {
        if (reader.getBool())
            reader.getData(_data, _dim1 * _dim2 * _dim3 * sizeof(T));

        reader.getMilestone();
    }

    void
    exportTo(IntWriter & writer, bool exportData=true)
    {
        if (!_ownerOfData)
            throw WUPException("Bundle can't be automatically exported when it does not own the data.");

        writer.putUInt32(_dim1);
        writer.putUInt32(_dim2);
        writer.putUInt32(_dim3);

        writer.putBool(exportData);

        if (exportData)
            writer.putData(_data, _dim1 * _dim2 * _dim3 * sizeof(T));

        writer.putMilestone();
    }

    Bundle3D<T> &
    operator=(const Bundle3D<T> & other)
    {
        if (_ownerOfData)
            delete [] _data;

        _dim1 = other._dim1;
        _dim2 = other._dim2;
        _dim3 = other._dim3;
        _planeSize = other._planeSize;
        _data = other._data;
        _ownerOfData = false;

        return *this;
    }

    uint32_t
    numRows() const
    {
        return _dim1;
    }

    uint32_t
    numCols() const
    {
        return _dim2;
    }

    uint32_t
    numChannels() const
    {
        return _dim3;
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

    // Functions to access the coordinates (0,0,i3)
    template <typename I1>
    T &
    operator()(const I1 & i1)
    {
        return (*this)(i1, 0, 0);
    }

    template <typename I1>
    const T &
    operator()(const I1 & i1) const
    {
        return (*this)(i1, 0, 0);
    }

    // Functions to access the coordinates (0,i2,i3)
    template <typename I1, typename I2>
    T &
    operator()(const I1 & i1, const I2 & i2)
    {
        return (*this)(i1, i2, 0);
    }

    template <typename I1, typename I2>
    const T &
    operator()(const I1 & i1, const I2 & i2) const
    {
        return (*this)(i1, i2, 0);
    }

    // Functions to address the coordinates (i1,i2,i3)
    template <typename I1, typename I2, typename I3>
    T &
    operator()(const I1 & i1, const I2 & i2, const I3 & i3)
    {
        return _data[i1 * (_planeSize) + i2 * (_dim3) + i3];
    }

    template <typename I1, typename I2, typename I3>
    T &
    operator()(const I1 & i1, const I2 & i2, const I3 & i3) const
    {
        return _data[i1 * (_planeSize) + i2 * (_dim3) + i3];
    }

    // Directly access the data
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

    // Number of elements in this Bundle
    uint64_t
    size() const
    {
        return _dim1 * _planeSize;
    }

    void
    operator=(const T & value)
    {
        for (uint64_t i=0;i!=size();++i)
            _data[i] = value;
    }

    // Assign 0 to all bits in this Bundle
    void
    clear()
    {
        memset(_data, 0, sizeof(T) * size());
    }

    void
    reshape(const uint32_t & dim1)
    {
        reshape(dim1, 1, 1);
    }

    void
    reshape(const uint32_t & dim1, const uint32_t & dim2)
    {
        reshape(dim1, dim2, 1);
    }

    void
    reshape(const uint32_t & dim1, const uint32_t & dim2, const uint32_t & dim3)
    {
        const uint64_t oldSize = size();

        if (dim2 != _dim2 || dim3 != _dim3)
            _planeSize = dim2 * dim3;

        if (dim1 != _dim1)
            _dim1 = dim1;

        if (dim2 != _dim2)
            _dim2 = dim2;

        if (dim3 != _dim3)
            _dim3 = dim3;

        const uint64_t newSize =
                static_cast<uint64_t>(_dim1) *
                static_cast<uint64_t>(_dim2) *
                static_cast<uint64_t>(_dim3);

        if (newSize > oldSize)
        {
            T * newData = new T[newSize];

//            if (preserveOldData)
//                std::copy(_data, _data + oldSize, newData);

            if (_ownerOfData)
                delete [] _data;

            _data = newData;
        }
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
