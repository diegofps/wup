#ifndef SEQ_HPP
#define SEQ_HPP

#include <wup/common/exceptions.hpp>

#include <initializer_list>
#include <iostream>
#include <cstring>

namespace wup {

template <typename T>
class seq {
public:

    seq(const size_t size) :
        _size(size),
        _mem(_size==0?NULL:new T[_size])
    {

    }

    seq(const int * list, const uint length) :
        seq(length)
    {
        std::copy(list, list + length, _mem);
    }

    seq(const std::initializer_list<T> & list) :
        seq(list.size())
    {
        std::copy(list.begin(), list.end(), _mem);
    }

    seq(const seq<T> & other) :
        _size(other._size),
        _mem(_size==0?NULL:new T[_size])
    {
        for (uint i=0;i!=_size;++i)
            _mem[i] = other._mem[i];
    }

    ~seq()
    {
        if (_mem != NULL)
            delete [] _mem;
    }

    void resize(const uint size)
    {
        if (size == 0)
            throw WUPException("seq size must be bigger than 0");

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

    const T & operator[](const uint index) const
    {
        if (_mem==NULL || index >=_size)
            throw WUPException("Out of bounds");
        return _mem[index];
    }

    T & operator[](const uint index)
    {
        if (_mem == NULL || index >=_size)
            throw WUPException("Out of bounds");
        return _mem[index];
    }

    uint size() const
    {
        return _size;
    }

    T * data()
    {
        return _mem;
    }

    const T * data() const
    {
        return _mem;
    }

private:
    uint _size;
    T * _mem;
};

template <typename T>
std::ostream & operator<<(std::ostream & o, const seq<T> & l)
{
    if (l.size() == 0)
    {
        o << "[]";
    }
    else
    {
        o << '[' << l[0];
        for (uint i=1; i!=l.size(); ++i)
            o << ',' << l[i];
        o << ']';
    }

    return o;
}

}

#endif // SEQ_HPP
