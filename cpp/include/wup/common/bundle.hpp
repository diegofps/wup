#ifndef BUNDLE_HPP
#define BUNDLE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <wup/common/msgs.hpp>
#include <wup/common/generic.hpp>

namespace wup {

template <typename T>
class Bundle {
private:

    uint _columns;
    uint _capacity;
    uint _size;
    T * _data;
    bool _ownerOfData;

public:

    Bundle() :
        Bundle(1, 1)
    {

    }

    Bundle(const uint columns) :
        Bundle(1, columns)
    {

    }

    Bundle(const uint rows, const uint columns, const T & initialValue) :
        Bundle(rows, columns)
    {
        this->operator=(initialValue);
    }

    Bundle(const uint rows, const uint columns) :
        _columns(columns),
        _capacity(columns * rows),
        _size(_capacity),
        _data(new T[_capacity]),
        _ownerOfData(true)
    {

    }

    Bundle(T * const data, const uint columns, const uint size) :
        _columns(columns),
        _capacity(size),
        _size(size),
        _data(data),
        _ownerOfData(false)
    {

    }

    Bundle(const std::string filename, const char delimiter='\t', int ignoreRows=0) :
        _columns(0),
        _capacity(8),
        _size(0),
        _data(new T[_capacity]),
        _ownerOfData(true)
    {
        // Abre o arquivo para leitura
        std::ifstream file_in(filename);
        if (!file_in.good())
        throw WUPException(cat("Could not open file: ", filename));

        // Carrega linha a linha
        int rows = 0;
        std::string line;
        std::vector<std::string> cells;

        while(!file_in.eof())
        {
            getline(file_in, line);
            ++rows;

            // Ignora cabecalhos e linhas em branco
            if (line.empty()) continue;
            if (rows <= ignoreRows) continue;

            // Quebra a linha em celulas
            split(line, delimiter, cells);

            // Guarda o numero de colunas
            if (_columns == 0)
                _columns = cells.size();

            // Guarda os dados
            for (uint i=0;i<cells.size();++i)
                push_back( parse_double(cells[i]) );
        }

        file_in.close();
    }

    ~Bundle()
    {
        if (_ownerOfData)
            delete [] _data;
    }

    Bundle<T> & operator=(const Bundle<T> & other)
    {
        if (_ownerOfData)
            delete [] _data;

        _columns = other._columns;
        _capacity = other._capacity;
        _size = other._size;
        _data = other._data;
        _ownerOfData = false;

        return *this;
    }

    template <typename I1, typename I2>
    T &
    operator()(const I1 & i, const I2 & j)
    {
#ifndef WUP_UNSAFE
        const I1 index = i * _columns + j;
        if (j > _columns || index > _size) error("Out of bounds");
        return _data[index];
#else
        return _data[i * _columns + j];
#endif
    }

    template <typename I1, typename I2>
    const T &
    operator()(const I1 & i, const I2 & j) const
    {
#ifndef WUP_UNSAFE
        const I1 index = i * _columns + j;
        if (j > _columns || index > _size) error("Out of bounds");
        return _data[index];
#else
        return _data[i * _columns + j];
#endif
    }

    T & operator()(const uint j)
    {
        return (*this)(0l, j);
    }

    const T & operator()(const uint j) const
    {
        return (*this)(0l, j);
    }

    T &
    at(const uint32_t index)
    {
#ifndef WUP_UNSAFE
        if (index > _size)
            throw WUPException("Out of bounds");
#endif

        return _data[index];
    }

    const T &
    at(const uint32_t index) const
    {
#ifndef WUP_UNSAFE
        if (index > _size)
            throw WUPException("Out of bounds");
#endif

        return _data[index];
    }


    const Bundle<T> & operator=(const T & value)
    {
        T * cur = _data;
        const T * const end = _data + _size;

        while (cur != end)
        {
            *cur = value;
            ++cur;
        }

        return *this;
    }

    void reshape(const uint cols)
    {
        reshape(1, cols);
    }

    void reshape(const uint rows, const uint cols)
    {
        const uint newSize = rows * cols;

        _columns = cols;
        _size = newSize;

        if (newSize <= _capacity)
            return;

        _capacity = newSize;
        auto newData = new T [newSize];
        delete [] _data;

        _data = newData;
    }

    uint numCols() const
    {
        return _columns;
    }

    uint numRows() const
    {
        return ceil(_size / double(_columns));
    }

    void clear()
    {
        _size = 0;
    }

    void push_back(const T * const array, const uint length)
    {
        require(length);

        for (uint i=0;i<length;++i)
            _data[_size++] = array[i];
    }

    void push_back(const T & t)
    {
        require(1);

        _data[_size++] = t;
    }

    T * begin() const
    {
        return _data;
    }

    T * end() const
    {
        return _data + _size;
    }

    T * begin()
    {
        return _data;
    }

    T * end()
    {
        return _data + _size;
    }

    void exportTo(const std::string filename) const
    {
        std::ofstream fout;
        fout.open(filename.c_str(), std::ios::out);
        fout << *this;
    }

    T * data()
    {
        return _data;
    }

    const T * data() const
    {
        return _data;
    }

    /*
        Returns the number of elements in this Bundle
    */
    uint size() const
    {
        return _size;
    }

private:

    void
    require(const int quantity)
    {
        if (_size + quantity <= _capacity)
            return;

        if (!_ownerOfData)
            throw WUPException("This Bundle can't be resized. It does not own the data");

        int newCap = max(_capacity + quantity, _capacity * 2);
        T * newData = new T[newCap];

        copy(_data, _data + _size, newData);

        delete [] _data;
        _capacity = newCap;
        _data = newData;
    }

};

template <typename T>
class BundleView
{
public:

    T * data;

    uint rows;

    uint cols;

    uint stride;

    BundleView(Bundle<T> & _bundle, const uint _rows, const uint _cols) :
        data(&_bundle(0, 0)),
        cols(_cols),
        rows(_rows),
        stride(_bundle.numCols())
    {

    }

    BundleView(Bundle<T> & bundle, const uint i1, const uint j1, const uint i2, const uint j2) :
        data(&bundle(i1, j1)),
        cols(j2-j1),
        rows(i2-i1),
        stride(bundle.numCols())
    {
#ifndef WUP_UNSAFE
        if (i1 < 0 || j1 < 0 || i2 > bundle.numRows() || j2 > bundle.numCols() || i2 <= i1 || j2 <= j1)
            throw WUPException("Out of bounds");
#endif
    }

    BundleView(const T * const _data, const uint _rows, const uint _cols, const uint _stride) :
        data(_data),
        rows(_rows),
        cols(_cols),
        stride(_stride)
    {

    }

    T &
    operator()(const uint i, const uint j)
    {
#ifndef WUP_UNSAFE
        if (j >= cols || i >= rows || i < 0 || j < 0)
            throw WUPException("Out of bounds");
#endif
        const int index = i * stride + j;
        return data[index];
    }

    const T &
    operator()(const uint i, const uint j) const
    {
#ifndef WUP_UNSAFE
        if (j >= cols || i >= rows || i < 0 || j < 0)
            throw WUPException("Out of bounds");
#endif

        const uint index = i * stride + j;
        return data[index];
    }

    const T &
    operator[](const uint index) const
    {
#ifndef WUP_UNSAFE
        if (index >= cols * rows || index < 0)
            throw WUPException("Out of bounds");
#endif
        const uint i = index / cols;
        const uint j = index % cols;
        return (*this)(i, j);
    }

    T &
    operator[](const uint index)
    {
#ifndef WUP_UNSAFE
        if (index >= cols * rows || index < 0)
            throw WUPException("Out of bounds");
#endif
        const uint i = index / cols;
        const uint j = index % cols;
        return (*this)(i, j);
    }

};

template <typename T>
std::ostream & operator<<(std::ostream & o, const wup::Bundle<T> & bundle)
{
    for (uint i=0; i<bundle.numRows(); ++i)
    {
        o << bundle(i, 0);

        for (uint j=1; j<bundle.numCols(); ++j)
            o << "," << bundle(i, j);

        o << "\n";
    }

    return o;
}

template <typename T>
std::ostream & operator<<(std::ostream & o, const wup::BundleView<T> & view)
{
    for (uint i=0; i<view.rows; ++i)
    {
        o << view(i, 0);

        for (uint j=1; j<view.cols; ++j)
            o << "," << view(i, j);

        o << "\n";
    }
    return o;
}

template <typename T>
T
wmin(const Bundle<T> & data)
{
    return arrayMin(&data(0,0), data.size());
}

template <typename T>
T
wmax(const Bundle<T> & data)
{
    return arrayMax(&data(0,0), data.size());
}

}
#endif // BUNDLE_HPP
