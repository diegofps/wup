#ifndef WUP__IO_HPP_
#define WUP__IO_HPP_

//#include <boost/algorithm/string.hpp>

#include <wup/common/iterators.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/bundle.hpp>
#include <wup/common/seq.hpp>
#include <wup/common/ref_vector.hpp>

namespace wup {

class Feature {
public:

    Feature(double * const mem, const uint columns) :
        _size(columns),
        _mem(mem)
    {

    }
    
    Feature(Bundle<double> &data) : Feature(data, 0)
    {

    }
    
    Feature(Bundle<double> &data, const uint row) :
        _size(data.numCols()),
        _mem(&data(row, 0))
    {

    }
    
    Feature(const Feature & other) :
        _size(other._size),
        _mem(other._mem)
    {

    }
    
    void remap(double * const mem, const uint columns)
    {
        _size = columns;
        _mem = mem;
    }

    double * data()
    {
        return _mem;
    }
    
    const double * data() const
    {
        return _mem;
    }

    void
    copyTo(double * const dst) const
    {
        memcpy(dst, _mem, sizeof(double) * _size);
    }

    uint size() const
    {
        return _size;
    }
    
    double & operator[](const uint index)
    {
        if (index >= _size)
            throw WUPException("Out of range");
        return _mem[index];
    }
    
    double & operator[](const uint index) const
    {
        if (index >= _size)
            throw WUPException("Out of range");
        return _mem[index];
    }
    
    Feature & operator=(const Feature & other)
    {
        if (other._size != _size)
            throw WUPException("Features size differ");
        memcpy(_mem, other._mem, sizeof(double) * _size);
        return *this;
    }
    
    template <typename T>
    Feature & operator=(const T & value)
    {
        for (uint i=0; i!=_size; ++i)
            _mem[i] = value;
        return *this;
    }

private:

    uint _size;

    double * _mem;

};

inline std::ostream &
operator<<(std::ostream &o, const wup::Feature &feature)
{
    if (feature.size() == 0)
        return o;

    o << feature[0];
    for (uint i=1; i!=feature.size(); ++i)
        o << "," << feature[i];

    return o;
}

class Sample : public ref_vector<Feature> {
public:

    Sample(const int id, const int target, const int subtarget, const int group, 
            Bundle<double> &data) :
        Sample(id, target, subtarget, group, data, 0, data.numRows()) 
    {

    }
    
    Sample(const int id, const int target, const int subtarget, const int group, 
            Bundle<double> &data, const uint start, const uint end) :
        _id(id),
        _target(target),
        _subtarget(subtarget),
        _group(group)
    {
        for (uint i=start; i!=end; ++i)
            push_back(new Feature(data, i));
    }
    
    Sample(const Sample & other) :
        _id(other._id),
        _target(other._target),
        _subtarget(other._subtarget),
        _group(other._group)
    {
        for (auto & feature : other)
            push_back(new Feature(feature));
    }
    
    ~Sample()
    {
        for (auto &feature : *this)
            delete &feature;
    }
    
    int id() const
    {
        return _id;
    }
    
    int target() const
    {
        return _target;
    }
    
    int subtarget() const
    {
        return _subtarget;
    }
    
    int group() const
    {
        return _group;
    }

    void exportTo( const std::string &filename) const
    {
        std::ofstream fout;
        fout.open(filename.c_str(), std::ios::out);
        for (auto point : *this)
            fout << point << "\n";
    }

private:
    int _id;
    int _target;
    int _subtarget;
    int _group;
};

inline std::ostream &
operator<<(std::ostream &o, const wup::Sample &sample)
{
    return o << sample.id();
}

class Dataset : public ref_vector<Sample> {
public:
    
    /*
     * Accepted dataset format:
     *
     * Data file: tab separated csv with L lines and C columns
     * Attr file: tab separated csv where each row corresponds to a sample. Attributes:
     *
     *     Target (Mandatory)                  : A number indicating its class
     *     Length (Optional, default=1)        : Length of the time series
     *     Group  (Optional, default 0)        : Defines a subgroup in case you have a predefined division
     *     Subtarget (Otional, default=target) : A more specific target, e.g., to differentiate lowercase vs uppercase
     *
     * */

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
            
            Sample * newSample = new Sample(i, target, subtarget, group, _data, start, end);
            push_back(newSample);

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
        for (auto &sample : other) {
            Sample * newSample = new Sample(sample);
            push_back(newSample);
        }
    }
    
    ~Dataset()
    {
        for (auto &sample : *this)
            delete &sample;
    }

    const Bundle<double> & data()
    {
        return _data;
    }

    const Bundle<double> & attr()
    {
        return _attr;
    }

    const Bundle<double> & data() const
    {
        return _data;
    }

    const Bundle<double> & attr() const
    {
        return _attr;
    }

    int numFeatures() const
    {
        return _data.numCols();
    }

    int classes() const
    {
        return _classes;
    }

private:
    Bundle<double> _data;
    Bundle<double> _attr;
    int _classes;
};

inline std::ostream &
operator<<(std::ostream &o, const wup::Dataset &dataset)
{
    for (auto &sample : dataset) o << sample;
    return o;
}

inline double sdistance(const wup::Feature & f1, const wup::Feature & f2)
{
    if (f1.size() != f2.size()) throw wup::WUPException("Features size differ");
    return sdistance(f1.data(), f2.data(), f1.size());
}

inline double distance(const wup::Feature & f1, const wup::Feature & f2)
{ return sqrt(sdistance(f1, f2)); }

} /* wup */

#endif
