#ifndef BUNLDE3D_HPP
#define BUNLDE3D_HPP

#include <cstring>
#include <vector>
#include <cstdlib>

using namespace std;

namespace wup {

template <typename T>
class Bundle3D {
public:

    int _rows;

    int _cols;

    int _depth;

    vector<T> _data;

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

} // wup

#endif
