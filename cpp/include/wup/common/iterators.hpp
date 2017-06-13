#ifndef __WUP__ITERATORS_HPP
#define __WUP__ITERATORS_HPP

#include <iterator>

template <typename T>
class const_ref_iterator {
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef int value_type;
    typedef int difference_type;
    typedef int* pointer;
    typedef int& reference;

    const_ref_iterator(T * const * ds) : _ds(ds) { }

    const T & operator*() const
    { return **_ds; }

    void operator++()
    { ++_ds; }

    bool operator!=(const const_ref_iterator<T> &other) const
    { return _ds != other._ds; }

    bool operator==(const const_ref_iterator<T> &other) const
    { return _ds == other._ds; }

    int operator-(const const_ref_iterator<T> &other) const
    { return _ds - other._ds; }

    const_ref_iterator<T> & operator+=(const int n)
    {
        _ds += n;
        return *this;
    }

private:
    const T * const * _ds;
};

template <typename T>
class ref_iterator {
public:
    typedef int difference_type;

    ref_iterator(T ** ds) : _ds(ds) { }

    T & operator*() const
    { return **_ds; }

    void operator++()
    { ++_ds; }

    bool operator!=(const ref_iterator<T> &other) const
    { return _ds != other._ds; }

    bool operator==(const ref_iterator<T> &other) const
    { return _ds == other._ds; }

    int operator-(const ref_iterator<T> &other) const
    { return _ds - other._ds; }

    ref_iterator<T> & operator+=(const int n)
    {
        _ds += n;
        return *this;
    }

private:
    T ** _ds;
};

namespace std {

template <typename T>
int distance(const const_ref_iterator<T> &it1, const const_ref_iterator<T> &it2)
{ return it2 - it1; }

template <typename T>
int distance(const ref_iterator<T> &it1, const ref_iterator<T> &it2)
{ return it2 - it1; }

}

#endif // __WUP__ITERATORS_HPP
