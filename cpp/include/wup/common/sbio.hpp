#ifndef INCLUDE_WUP_SBIO_HPP
#define INCLUDE_WUP_SBIO_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include <wup/common/exceptions.hpp>
#include <wup/common/generic.hpp>

using std::ofstream;
using std::ifstream;

namespace wup {

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Source
{
public:
    virtual ~Source() { }
    virtual void get(T &t) = 0;
    virtual const T & get() = 0;
    virtual bool good() = 0;
};

template <typename T>
class Sink
{
public:
    virtual ~Sink() { }
    virtual void put(const T &t) = 0;
    virtual bool good() = 0;
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
class MemSource : public Source<T>
{
    const T * const data;

    uint64_t pos;

    uint64_t size;

public:

    MemSource(const T * const data, const uint64_t size) :
        data(data),
        pos(0),
        size(size)
    {

    }

    virtual ~MemSource()
    {

    }

    void
    get(T &t)
    {
        if (pos == size)
            throw wup::WUPException();

        t = data[pos];
        ++pos;
    }

    const T &
    get()
    {
        if (pos == size)
            throw wup::WUPException();

        const T & r = data[pos];
        ++pos;
        return r;
    }

    bool
    good()
    {
        return true;
    }

};

template <typename T>
class MemSink : public Sink<T>
{
public:

    uint64_t capacity;

    uint64_t size;

    T * data;

    MemSink(const uint64_t initialCapacity) :
        capacity(initialCapacity ? initialCapacity : 1),
        size(0),
        data((T*) malloc(capacity * sizeof(T)))
    {

    }

    virtual ~MemSink()
    {
        free(data);
    }

    void
    put(const T &t)
    {
        if (size == capacity)
        {
            capacity *= 2;
            data = (T*) realloc(data, capacity);
        }

        data[size] = t;
        ++size;
    }

    bool
    good()
    {
        return true;
    }

};

template <typename T>
class FileSource : public Source<T>
{
public:
    
    FileSource(const std::string &filename, bool abortOnOpenFail=true) :
        FileSource(filename, 10240, abortOnOpenFail)
    {

    }
    
    FileSource(const std::string &filename, const uint64_t capacity, bool abortOnOpenFail=true) :
        _stream(filename.c_str(), std::ios::binary | std::ios::in)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _content = 0;
        _current = 0;
        _capacity = capacity;

        if (abortOnOpenFail && !good())
            throw WUPException(cat("Failed to open ", filename, " for reading"));
    }
    
    virtual ~FileSource()
    {
        _stream.close();
        free(_buffer);
    }
    
    void get(T &t)
    {
        if (_current == _content) readMore();
        t = _buffer[_current++];
    }

    const T & get()
    {
        if (_current == _content) readMore();
        return _buffer[_current++];
    }

    bool good()
    {
        return _stream.good();
    }

private:

    void readMore()
    {
        _stream.read((char*) _buffer, sizeof(T) * _capacity);
        //int chars = _stream.gcount();
        _content = _stream.gcount() / sizeof(T);
        _current = 0;
//        print("Read ", chars, " characters, giving ", _content, " entities");

        if (_current == _content)
            throw wup::WUPException();
    }

private:
    ifstream _stream;
    T * _buffer;
    uint64_t _capacity;
    uint64_t _current;
    uint64_t _content;
};

template <typename T>
class FileSink : public Sink<T>
{
public:

    FileSink(const std::string &filename, bool abortOnOpenFail=true) :
        FileSink(filename, 10240, abortOnOpenFail)
    {

    }

    FileSink(const std::string &filename, const uint64_t capacity, bool abortOnOpenFail=true) :
        _stream(filename.c_str(), std::ios::binary | std::ios::trunc)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _current = 0l;
        _capacity = capacity;

        if (abortOnOpenFail && !good())
            throw WUPException(cat("Failed to open ", filename, " for writing"));
    }

    virtual ~FileSink()
    {
        if (_current != 0)
        {
            _stream.write((char*) _buffer, sizeof(T) * _current);
        }

        _stream.flush();
        _stream.close();
        free(_buffer);
    }

    void put(const T &t)
    {
        _buffer[_current++] = t;

        if (_current == _capacity)
        {
            _stream.write((char*) _buffer, sizeof(T) * _current);
            _current = 0;
        }
    }

    bool good()
    {
        return _stream.good();
    }

private:
    ofstream _stream;
    T * _buffer;
    uint64_t _capacity;
    uint64_t _current;
};

/////////////////////////////////////////////////////////////////////////////////

class IntReader
{
public:

    Source<int32_t> & src;

public:

    IntReader(Source<int32_t> & src) : src(src)
    {

    }

    virtual ~IntReader()
    {

    }

    bool
    good()
    {
        return src.good();
    }

    void
    get(int32_t & v)
    {
        src.get(v);
    }

    int32_t
    get()
    {
        return src.get();
    }

    std::string
    getString()
    {
        std::stringstream ss;

        int32_t tmp;
        while ((tmp = src.get()) != 0)
            ss << (char) tmp;

        return ss.str();
    }

    double
    getDouble()
    {
        const int32_t & i = src.get();
        const int32_t & j = src.get();
        return i * pow(10, j - 8);
    }

    long
    getLong()
    {
        long l;

        int32_t * start = (int32_t*)( &l );
        int32_t * const end = (int32_t*)( (&l) + 1 );

        while(start != end)
        {
            src.get(*start);
            start += 1;
        }

        return l;
    }

    int64_t
    getInt64()
    {
        int64_t tmp;

        int32_t * root = (int32_t*) (&tmp);

        src.get(root[0]);
        src.get(root[1]);

        return tmp;
    }

    uint64_t
    getUInt64()
    {
        uint64_t tmp;

        int32_t * root = (int32_t*) (&tmp);

        src.get(root[0]);
        src.get(root[1]);

        return tmp;
    }

    uint
    getUnsignedInt()
    {
        uint l;
        int32_t * const root = (int32_t*) (&l);
        src.get(root[0]);
        return l;
    }

    bool
    getBool()
    {
        return src.get() != 0;
    }

};

class IntWriter
{
public:

    Sink<int32_t> & snk;

public:

    IntWriter(Sink<int32_t> & snk) :
        snk(snk)
    {

    }

    virtual ~IntWriter()
    {

    }

    bool
    good()
    {
        return snk.good();
    }

    void
    put(const int32_t & v)
    {
        snk.put(v);
    }

    void
    putBool(const bool & b)
    {
        const int tmp(b);
        snk.put(tmp);
    }

    void
    putInt64(const int64_t & l)
    {
        const int32_t * root = (const int32_t*)( &l );

        snk.put(root[0]);
        snk.put(root[1]);
    }

    void
    putUInt64(const uint64_t & l)
    {
        const int32_t * root = (const int32_t*)( &l );

        snk.put(root[0]);
        snk.put(root[1]);
    }

    void
    putLong(const long & l)
    {
        const int32_t * start = (const int32_t*)( &l );
        const int32_t * const end = (const int32_t*)( (&l) + 1 );

        while(start != end)
        {
            snk.put(*start);
            start += 1;
        }
    }

    void
    putUnsignedInt(const uint & ui)
    {
        const int32_t * const root = (const int*) (& ui);
        snk.put(root[0]);
    }

    void
    putDouble(const double & n)
    {
        const int32_t j = n == 0.0 ? 0 : n < 0.0 ? floor(log10(-n)) : floor(log10(n));
        const int32_t i = n / pow(10, j - 8);

        snk.put(i);
        snk.put(j);
    }

    template <typename T>
    void
    putString(T & str)
    {
        for (int i=0; str[i]!='\0'; ++i)
            snk.put((int32_t)str[i]);
        snk.put(0);
    }

};

/////////////////////////////////////////////////////////////////////////////////

class IntFileReader : public IntReader
{
public:

    FileSource<int32_t> src;

    IntFileReader(const std::string & filename, const uint64_t capacity=1024*1024, bool abortOnOpenFail=true) :
        IntReader(src),
        src( filename, capacity, abortOnOpenFail )
    { }

    virtual ~IntFileReader()
    { }
};

class IntFileWriter : public IntWriter
{
public:

    FileSink<int32_t> snk;

    IntFileWriter(const std::string & filename, const uint64_t capacity=1024*1024, bool abortOnOpenFail=true) :
        IntWriter(snk),
        snk( filename, capacity, abortOnOpenFail )
    { }


    virtual ~IntFileWriter()
    { }
};

class IntMemReader : public IntReader
{
public:

    MemSource<int32_t> src;

    IntMemReader(const int32_t * data, const uint64_t size) :
        IntReader(src),
        src( data, size )
    { }

    virtual ~IntMemReader()
    { }
};

class IntMemWriter : public IntWriter
{
public:

    MemSink<int32_t> snk;

    IntMemWriter(const uint64_t initialCapacity) :
        IntWriter(snk),
        snk( initialCapacity )
    { }

    virtual ~IntMemWriter()
    { }
};

} /* wup */

#endif /* INCLUDE_WUP_SBIO_HPP */

