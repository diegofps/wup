#ifndef BUNLDE3D_HPP
#define BUNLDE3D_HPP

#include <cstring>
#include <vector>
#include <cstdlib>

namespace wup {

template <typename T>
class Bundle3D {
public:

    int _rows;

    int _cols;

    int _depth;

    std::vector<T> _data;

    Bundle3D() : Bundle3D(1, 1, 1)
    {

    }

    Bundle3D(const int rows, const int cols, const int depth) :
        _rows(rows),
        _cols(cols),
        _depth(cols),
        _data(cols * rows * depth, T())
    {

    }

    T &
    operator()(const int i, const int j, const int d)
    {
        return _data[i * (_cols * _depth) + j * (_depth) + d];
    }

    const T &
    operator()(const int i, const int j, const int d) const
    {
        return _data[i * (_cols * _depth) + j * (_depth) + d];
    }

    T *
    data()
    {
        return _data.data();
    }

    const T *
    data() const
    {
        return _data.data();
    }

    uint64_t
    size() const
    {
        return _rows * _cols * _depth;
    }

    void
    operator=(const T & value)
    {
        for (uint i=0;i!=_data.size();++i)
            _data[i] = value;
    }

    void
    clear()
    {
        memset(_data.data(), 0, sizeof(T) * _data.size());
    }

    void
    reshape(const int rows, const int cols, const int depth)
    {
        if (rows != _rows)
            _rows = rows;

        if (cols != _cols)
            _cols = cols;

        if (depth != _depth)
            _depth = depth;

        const int newSize = _rows * _cols * _depth;

        if (newSize > _data.size())
            _data.resize(newSize);
    }

    int
    numRows() const
    {
        return _rows;
    }

    int
    numCols() const
    {
        return _cols;
    }

    int
    numDepth() const
    {
        return _depth;
    }

}; // Bundle3D

template <typename T>
T
min(const Bundle3D<T> & data)
{
    return arrayMin(&data(0,0,0), data.size());
}

template <typename T>
T
max(const Bundle3D<T> & data)
{
    return arrayMax(&data(0,0,0), data.size());
}

} // wup

#endif
