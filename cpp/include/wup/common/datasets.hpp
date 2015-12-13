#ifndef __WUP__IO_HPP
#define __WUP__IO_HPP

#include <boost/algorithm/string.hpp>

namespace wup {

template <typename T>
class seq {
public:

	seq(const seq<T> & other) :
		_size(other._size),
		_mem(_size==0?NULL:new T[_size])
	{
		for (int i=0;i<_size;++i)
			_mem[i] = other._mem[i];
	}

	template <typename... Args>
	seq(const Args&... args) :
		_size(__size(args...)),
		_mem(_size==0?NULL:new T[_size])
	{ __build<0>(args...); }

	~seq()
	{ if (_mem != NULL) delete [] _mem; }

	void resize(const int size)
	{
		if (size < 0)
			throw WUPException("Negative size for seq");

		if (size == 0) {
			if (_mem != NULL)
				delete [] _mem;
			_mem = NULL;
			_size = 0;
		}

		else {
			T * newmem = new T[size];
			memcpy(newmem, _mem, sizeof(T) * min(size, _size));
			delete [] _mem;

			_mem = newmem;
			_size = size;
		}
	}

	seq<T> & operator=(const seq<T> & other)
	{
		if (_size != other._size) {
			if (_mem != NULL)
				delete [] _mem;
			_size = other._size;
			_mem = _size==0?NULL:new T[_size];
		}

		for (int i=0;i<_size;++i)
			_mem[i] = other._mem[i];

		return *this;
	}

	const T & operator[](const int index) const
	{
		if (_mem==NULL || index<0 || index >=_size)
			throw WUPException("Out of bounds");
		return _mem[index];
	}

	T & operator[](const int index)
	{
		if (_mem == NULL || index<0 || index >=_size)
			throw WUPException("Out of bounds");
		return _mem[index];
	}

	int size() const
	{ return _size; }

private:

	int __size()
	{ return 0; }

	template <typename P1, typename... Args>
	int __size(const P1 & p1, const Args&... args)
	{ return 1 + __size(args...); }

	template <int index>
	void __build()
	{ }

	template <int index, typename P1, typename... Args>
	void __build(const P1 & p1, const Args&... args)
	{
		_mem[index] = p1;
		__build<index+1>(args...);
	}

private:
	int _size;
	T * _mem;
};

template <typename T>
std::ostream & operator<<(std::ostream & o, const seq<T> & l)
{
	if (l.size() == 0) {
		o << "[]";
	} else {
		o << '[' << l[0];
		for (int i=1;i<l.size();++i)
			o << ',' << l[i];
		o << ']';
	}
	return o;
}

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
    Bundle() : _columns(1)
	{ }

    Bundle(const int columns) : _columns(columns)
    { }

    Bundle(const std::string filename, const std::string &delimiter="\t",
            int ignoreRows=0) : _columns(0)
    {
	    // Abre o arquivo para leitura
	    std::ifstream file_in(filename);
	    if (!file_in.good())
	        throw WUPException("Could not open file");
	
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
    
    void clear()
    { _data.clear(); }

    void push_back(const T * const array, const int length)
    {
    	for (int i=0;i<length;++i)
    		_data.push_back(array[i]);
    }

private:
    int _columns;
    std::vector<T> _data;
};

class Feature {
public:
    Feature(double * const mem, const int columns) :
        _size(columns),
        _mem(mem)
    { }
    
    Feature(Bundle<double> &data) : Feature(data, 0)
    { }
    
    Feature(Bundle<double> &data, const int row) :
        _size(data.numCols()),
        _mem(&data(row, 0))
    { }
    
    Feature(const Feature & other) :
        _size(other._size),
        _mem(other._mem)
    { }
    
    double * data()
    { return _mem; }
    
    const double * data() const
	{ return _mem; }

    int size() const
    { return _size; }
    
    double & operator[](const int index)
    {
        if (index < 0 || index >= _size) throw WUPException("Out of range");
        return _mem[index];
    }
    
    double operator[](const int index) const
    {
        if (index < 0 || index >= _size) throw WUPException("Out of range");
        return _mem[index];
    }
    
    Feature & operator=(const Feature & other)
    {
        if (other._size != _size) throw WUPException("Features size differ");
        memcpy(_mem, other._mem, sizeof(double) * _size);
        return *this;
    }
    
    template <typename T>
    Feature & operator=(const T & value)
    {
    	for (int i=0;i<_size;++i)
    		_mem[i] = value;
    	return *this;
    }

private:
    int _size;
    double * _mem;
};

class Sample {
public:
    Sample(const int id, const int target, const int subtarget, const int group, 
            Bundle<double> &data) :
        Sample(id, target, subtarget, group, data, 0, data.numRows()) 
    { }
    
    Sample(const int id, const int target, const int subtarget, const int group, 
            Bundle<double> &data, const int start, const int end) : 
        _id(id),
        _target(target),
        _subtarget(subtarget),
        _group(group)
    {
        for (int i=start;i<end;++i)
            _features.push_back(new Feature(data, i));
    }
    
    Sample(const Sample & other) :
        _id(other._id),
        _target(other._target),
        _subtarget(other._subtarget),
        _group(other._group)
    {
        for (auto & feature : other)
            _features.push_back(new Feature(feature));
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
    std::vector<Feature*> _features;
    int _id;
    int _target;
    int _subtarget;
    int _group;
};

class Dataset {
public:
    
    Dataset(const std::string prefix) :
        _data(prefix + "_data"),
        _attr(prefix + "_attr")
    {
        const int attr_rows = _attr.numRows();
        const int attr_cols = _attr.numCols();
        
        _classes = -1;
        int start = 0;
        for (int i=0;i<attr_rows;++i) {
            const int target    = _attr(i, 0);
            const int end       = attr_cols > 1 ? start + _attr(i, 1) : 1;
            const int group     = attr_cols > 2 ? _attr(i, 2) : 0;
            const int subtarget = attr_cols > 3 ? _attr(i, 3) : target;
            
            _samples.push_back(new Sample(
                    i, target, subtarget, group, _data, start, end));

            if (target > _classes)
				_classes = target;

            start = end;
        }

        ++_classes;
    }
    
    Dataset(const Dataset & other) :
        _data(other._data),
        _attr(other._attr),
		_classes(other._classes)
    {
        for (auto &sample : other)
            _samples.push_back(new Sample(sample));
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
    
    const Bundle<double> * data()
    { return & _data; }
    
    const Bundle<double> * attr()
    { return & _attr; }
    
    int numFeatures() const
    { return _data.numCols(); }

    int size() const
    { return _samples.size(); }

    int classes() const
    { return _classes; }
    
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
    std::vector<Sample*> _samples;
    int _classes;
};

inline double sdistance(const wup::Feature & f1, const wup::Feature & f2)
{
    if (f1.size() != f2.size()) throw wup::WUPException("Features size differ");
    return sdistance(f1.data(), f2.data(), f1.size());
}

inline double distance(const wup::Feature & f1, const wup::Feature & f2)
{ return sqrt(sdistance(f1, f2)); }

std::ostream & operator<<(std::ostream & o, const wup::Feature & f)
{
    if (f.size() == 0)
        return o;
    
    o << f[0];
    for (int i=1;i<f.size();++i)
        o << " " << f[i];
    
    return o;
}

template <typename T>
std::ostream & operator<<(std::ostream & o, const Bundle<T> &b)
{
	for (int i=0;i<b.numRows();++i) {
		printn(i," : ", b(i,0));
		for (int j=1;j<b.numCols();++j)
			printn(", ", b(i,j));
		print();
	}
	return o;
}

} /* wup */

#endif
