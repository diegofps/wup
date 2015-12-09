#ifndef __WUP__IO_HPP
#define __WUP__IO_HPP

#include <boost/algorithm/string.hpp>

namespace wup {

template <typename T>
class const_iterator {
public:
    const_iterator(T * const * ds, const int pos) : _ds(ds), _pos(pos) { }

    const T & operator*() const
    { return *_ds[_pos]; }

    void operator++()
    { ++_pos; }

    bool operator!=(const const_iterator<T> &other) const
    { return _pos != other._pos; }

private:
    const T * const * _ds;
    int _pos;
};

template <typename T>
class iterator {
public:
    iterator(T ** ds, const int pos) : _ds(ds), _pos(pos) { }

    T & operator*() const
    { return *_ds[_pos]; }

    void operator++()
    { ++_pos; }

    bool operator!=(const iterator<T> &other) const
    { return _pos != other._pos; }

private:
    T ** _ds;
    int _pos;
};


template <typename T>
class Bundle {
public:
    Bundle() : _columns(1) { }

    Bundle(const string filename, const string &delimiter="\t", 
            int ignoreRows=0) : _columns(0)
    {
	    // Abre o arquivo para leitura
	    ifstream file_in(filename);
	    if (!file_in.good())
	        throw WUPException("Could not open file");
	
	    // Carrega linha a linha
	    int rows = 0;
	    string line;
	    vector<string> cells;
	    while(!file_in.eof()) {
		    getline(file_in, line);
		    ++rows;
		    
		    // Ignora cabecalhos e linhas em branco
		    if (line.empty()) continue;
		    if (rows <= ignoreRows) continue;
		    
		    // Quebra a linha em celulas
		    split(cells, line, boost::is_any_of(delimiter));
		
		    // Guarda o numero de colunas
		    if (_columns == 0)
			    _columns = cells.size();
		
		    // Guarda os dados
		    for (uint i=0;i<cells.size();++i)
			    _data.push_back( parse_double(cells[i]) );
	    }
	
	    file_in.close();
    }
    
    T & operator()(const int i, const int j)
    {
        if (i < 0 || j < 0 || j >= _columns)
            throw WUPException("Out of bounds");

        const int index = i * _columns + j;
        
        if (index >= _data.size())
            throw WUPException("Out of bounds");
        return _data[index];
    }
    
    const T & operator()(const int i, const int j) const
    {
        if (i < 0 || j < 0 || j >= _columns)
            throw WUPException("Out of bounds");

        const int index = i * _columns + j;
        
        if (index >= _data.size())
            throw WUPException("Out of bounds");
        return _data[index];
    }
    
    int numCols() const
    { return _columns; }
    
    int numRows() const
    { return ceil(_data.size() / double(_columns)); }
    
    int numItms() const
    { return _data.size(); }
    
private:
    int _columns;
    vector<T> _data;
};

class Feature {
public:
    Feature(const Bundle<double> &data) : Feature(data, 0) { }
    
    Feature(const Bundle<double> &data, const int row) :
        _size(data.numCols()),
        _mem(&data(row, 0))
    { }
    
    int size() const
    { return _size; }
    
    double operator[](const int index)
    {
        if (index < 0 || index >= _size)
            throw WUPException("Out of range");
        return _mem[index];
    }
    
private:
    int _size;
    const double * _mem;
};

class Sample {
public:
    Sample(const int id, const int target, const int subtarget, const int group, 
            const Bundle<double> &data) :
        Sample(id, target, subtarget, group, data, 0, data.numRows()) 
    { }
    
    Sample(const int id, const int target, const int subtarget, const int group, 
            const Bundle<double> &data, const int start, const int end) : 
        _id(id),
        _target(target),
        _subtarget(subtarget),
        _group(group)
    {
        for (int i=start;i<end;++i)
            _features.push_back(new Feature(data, i));
    }
    
    ~Sample()
    {
        for (auto &feature : _features)
            delete feature;
    }
    
    int id() const
    { return _id; }
    
    int target() const
    { return _target; }
    
    int subtarget() const
    { return _subtarget; }
    
    int group() const
    { return _group; }
    
    int size() const
    { return _features.size(); }
    
    wup::Feature & operator[](const int index)
    {
        if (index < 0 || index >= size())
            throw wup::WUPException("Out of range");
        return *_features[index];
    }
    
    const_iterator<Feature> begin() const
    { return const_iterator<Feature>(_features.data(), 0); }

    const_iterator<Feature> end() const
    { return const_iterator<Feature>(NULL, _features.size()); }

    iterator<Feature> begin()
    { return iterator<Feature>(_features.data(), 0); }

    iterator<Feature> end()
    { return iterator<Feature>(NULL, _features.size()); }

private:
    vector<Feature*> _features;
    int _id;
    int _target;
    int _subtarget;
    int _group;
};

class Dataset {
public:
    
    Dataset(const string prefix) : 
        _data(prefix + "_data"),
        _attr(prefix + "_attr")
    {
        const int attr_rows = _attr.numRows();
        const int attr_cols = _attr.numCols();
        
        int start = 0;
        for (int i=0;i<attr_rows;++i) {
            const int target    = _attr(i, 0);
            const int end       = attr_cols > 1 ? start + _attr(i, 1) : 1;
            const int group     = attr_cols > 2 ? _attr(i, 2) : 0;
            const int subtarget = attr_cols > 3 ? _attr(i, 3) : target;
            
            _samples.push_back(new Sample(
                    i, target, subtarget, group, _data, start, end));
        }
    }
    
    ~Dataset()
    {
        for (auto &sample : _samples)
            delete sample;
    }
    
    Sample & operator[](const int index)
    {
        if (index < 0 || index >= _samples.size())
            throw WUPException("Out of range");
        
        return *_samples[index];
    }
    
    int size() const
    { return _samples.size(); }
    
    const_iterator<Sample> begin() const
    { return const_iterator<Sample>(_samples.data(), 0); }

    const_iterator<Sample> end() const
    { return const_iterator<Sample>(NULL, _samples.size()); }

    iterator<Sample> begin()
    { return iterator<Sample>(_samples.data(), 0); }

    iterator<Sample> end()
    { return iterator<Sample>(NULL, _samples.size()); }

private:
    
    Bundle<double> _data;
    
    Bundle<double> _attr;
    
    vector<Sample*> _samples;
    
};

} /* wup */

#endif
