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

    Bundle(const int columns) : _columns(columns)
    { }

    Bundle(const int rows, const int columns) : _columns(columns), _data(columns * rows, T())
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
        while(!file_in.eof()) {
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
        if (j >= _columns)
            throw WUPException("Out of bounds");

        const int index = i * _columns + j;

        if (index >= _data.size())
            throw WUPException("Out of bounds");

        return _data[index];
    }

    const T & operator()(const uint i, const uint j) const
    {
        if (j >= _columns)
            throw WUPException("Out of bounds");

        const uint index = i * _columns + j;

        if (index >= _data.size())
            throw WUPException("Out of bounds");

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
std::ostream & operator<<(std::ostream &o, const wup::Bundle<T> &bundle)
{
    for (int i=0;i<bundle.numRows();++i) {
        o << bundle(i, 0);
        for (int j=1;j<bundle.numCols();++j)
            o << "," << bundle(i, j);
        o << "\n";
    }
    return o;
}

}
#endif // BUNDLE_HPP
