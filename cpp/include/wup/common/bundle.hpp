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
public:

    Bundle() : _columns(1)
    { }

    Bundle(const uint columns) : _columns(columns)
    { }

    Bundle(const uint rows, const uint columns) :
        _columns(columns), _data(columns * rows, T())
    { }

    Bundle(const uint rows, const uint columns, const T & initialValue) :
        _columns(columns),
        _data(columns * rows, initialValue)
    { }

    Bundle(const std::string filename, const char delimiter='\t',
        int ignoreRows=0) : _columns(0)
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
            split(cells, line, delimiter);

            // Guarda o numero de colunas
            if (_columns == 0)
                _columns = cells.size();

            // Guarda os dados
            for (uint i=0;i<cells.size();++i)
                _data.push_back( parse_double(cells[i]) );
        }

        file_in.close();
    }

    T & operator()(const uint i, const uint j)
    {
#ifndef WUP_UNSAFE
        if (j >= _columns)
            throw WUPException("Out of bounds");
#endif
        const int index = i * _columns + j;

#ifndef WUP_UNSAFE
        if (index >= _data.size())
            throw WUPException("Out of bounds");
#endif
        return _data[index];
    }

    const T & operator()(const uint i, const uint j) const
    {
#ifndef WUP_UNSAFE
        if (j >= _columns)
            throw WUPException("Out of bounds");
#endif

        const uint index = i * _columns + j;

#ifndef WUP_UNSAFE
        if (index >= _data.size())
            throw WUPException("Out of bounds");
#endif

        return _data[index];
    }

    T & operator()(const uint j)
    { return (*this)(0l, j); }

    const T & operator()(const uint j) const
    { return (*this)(0l, j); }

    const Bundle<T> & operator=(const T & value)
    {
        for (uint i=0; i<_data.size(); ++i)
            _data[i] = value;
        return *this;
    }

    uint numCols() const
    { return _columns; }

    uint numRows() const
    { return ceil(_data.size() / double(_columns)); }

    uint numItms() const
    { return _data.size(); }

    void clear()
    { _data.clear(); }

    void push_back(const T * const array, const uint length)
    {
        for (uint i=0;i<length;++i)
            _data.push_back(array[i]);
    }

    void push_back(const T & t)
    { _data.push_back(t); }

    typename std::vector<T>::const_iterator begin() const
    { return _data.begin(); }

    typename std::vector<T>::const_iterator end() const
    { return _data.end(); }

    typename std::vector<T>::iterator begin()
    { return _data.begin(); }

    typename std::vector<T>::iterator end()
    { return _data.end(); }

    void exportTo(const std::string filename) const
    {
        std::ofstream fout;
        fout.open(filename.c_str(), std::ios::out);
        fout << *this;
    }

    std::vector<T> & data()
    { return _data; }

    const std::vector<T> & data() const
    { return _data; }

    uint size() const
    { return _data.size(); }

private:

    uint _columns;

    std::vector<T> _data;

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

}
#endif // BUNDLE_HPP
