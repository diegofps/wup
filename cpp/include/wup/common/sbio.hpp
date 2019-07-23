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

namespace wup
{

/////////////////////////////////////////////////////////////////////////////////

/// Saves a sequence of bytes into a file.
///
/// filename - Path to the file that will receive the content
/// data - Pointer to the data to be saved
/// size - Number of bytes to be saved (this is not the number of elements)
template <typename T>
void
saveBytes(const string filename,
          const T * const data,
          const uint64_t size)
{
    FILE * f = fopen(filename.c_str(), "wb");

    if (!f)
        throw WUPException(cat("Could not write bytes to file: ", filename));

    fwrite(data, 1, size, f);
    fclose(f);
}

/// Loads a sequence of bytes from a file.
///
/// filename - Path to the file that holds the content to be read
/// data - Pointer that will hold the data (will be allocated by the function with new[])
/// size - Number of bytes read (this is not the number of elements)
template <typename T>
void
loadBytes(const string filename,
          T *& data,
          uint64_t & size)
{
    FILE * f = fopen(filename.c_str(), "rb");

    if (!f)
        throw WUPException(cat("Could not read bytes from file: ", filename));

    fseek(f, 0, SEEK_END);
    long tmp = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t * tmpData;

    if (tmp)
    {
        uint64_t bufferSize = 512*1024;
        uint8_t * buffer = new uint8_t[bufferSize];

        uint64_t capacity = bufferSize;
        tmpData = new uint8_t[capacity];

        size = 0;
        uint64_t read;
        while((read = fread(buffer, 1, bufferSize, f)) > 0)
        {
            if (size + read > capacity)
            {
                uint64_t newCapacity = (size + read) * 2;
                uint8_t * newData = new uint8_t[newCapacity];

                copy(tmpData, tmpData + size, newData);

                delete [] tmpData;

                capacity = newCapacity;
                tmpData = newData;
            }

            copy(buffer, buffer + read, tmpData + size);
            size += read;
        }

        if (size != capacity)
        {
            uint8_t * newData = new uint8_t[size];
            copy(tmpData, tmpData+size, newData);
            delete [] tmpData;
            tmpData = newData;
        }

        delete [] buffer;
    }
    else
    {
        size = uint64_t(tmp);
        tmpData = new uint8_t[size];
        fread(tmpData, 1, size, f);
    }

    fclose(f);

    data = (T*)tmpData;
}

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Source
{
public:

    virtual ~Source() { }

    virtual void get(T &t) = 0;

    virtual void getMany(void * t, const uint64_t len) = 0;

    virtual const T & get() = 0;

    virtual bool good() = 0;

};

template <typename T>
class Sink
{
public:

    virtual ~Sink() { }

    virtual void put(const T &t) = 0;

    virtual void putMany(const T * ptr, const uint64_t len) = 0;

    virtual bool good() = 0;

    // Returns the number of bytes saved inside the memory buffer.
    virtual uint64_t size() = 0;

    // Returns the number of T elements saved inside the memory buffer.
    virtual uint64_t length() = 0;

    // Returns the amount of bytes allocated for this memory buffer.
    virtual uint64_t capacity() = 0;

    // Returns the pointer to the first element of this memory buffer.
    virtual T * data() = 0;
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
class MemSource : public Source<T>
{
    const T * const data;

    uint64_t pos;

    uint64_t size;

public:

    MemSource(const T * const data, const uint64_t length) :
        data(data),
        pos(0),
        size(length)
    {

    }

    virtual
    ~MemSource()
    {

    }

    void
    get(T &t)
    {
        if (pos == size)
            error("Too many reads");

        t = data[pos];
        ++pos;
    }

    const T &
    get()
    {
        if (pos == size)
            error("Too many reads");

        const T & r = data[pos];
        ++pos;
        return r;
    }

    virtual void
    getMany(void * _ptr, const uint64_t len)
    {
        if (pos + len > size)
            error("Too many reads");

        T * ptr = (T*) _ptr;
        std::copy(data+pos, data+pos+len, ptr);
        pos += len;
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
private:

    uint64_t _capacity;

    uint64_t _size;

    T * _data;

public:

    MemSink(const uint64_t initialCapacity) :
        _capacity(initialCapacity ? initialCapacity : 1),
        _size(0),
        _data((T*) malloc(_capacity * sizeof(T)))
    {

    }

    virtual
    ~MemSink()
    {
        free(_data);
    }

    void
    put(const T &t)
    {
        if (_size == _capacity)
        {
            _capacity *= 2;
            _data = (T*) realloc(_data, _capacity * sizeof(T));
            print("Resizing buffer, new size = ", _capacity);
        }

        _data[_size] = t;
        ++_size;
    }

    void
    putMany(const T * ptr, const uint64_t len)
    {
        if (_size + len > _capacity)
        {
            _capacity = _size + len;
            _data = (T*) realloc(_data, _capacity * sizeof(T));
            print("Resizing buffer, new size = ", _capacity);
        }

        std::copy(ptr, ptr + len, _data + _size);
        _size += len;
    }

    bool
    good()
    {
        return true;
    }

    virtual uint64_t
    size()
    {
        return _size * sizeof(T);
    }

    virtual uint64_t
    length()
    {
        return _size;
    }

    virtual uint64_t
    capacity()
    {
        return _capacity;
    }

    virtual T *
    data()
    {
        return _data;
    }

};

template <typename T>
class VectorSink : public Sink<T>
{
private:

    vector<T> & mem;

public:

    VectorSink(vector<T> & mem, const bool clearVector=true) :
        mem(mem)
    {
        if (clearVector)
            mem.clear();
    }

    void
    put(const T &t)
    {
        mem.push_back(t);
    }

    void
    putMany(const T * ptr, const uint64_t len)
    {
        mem.insert(mem.end(), ptr, ptr + len);
    }

    bool
    good()
    {
        return true;
    }

    virtual uint64_t
    size()
    {
        return mem.size() * sizeof(T);
    }

    virtual uint64_t
    length()
    {
        return mem.size();
    }

    virtual uint64_t
    capacity()
    {
        return mem.capacity() * sizeof(T);
    }

    virtual T *
    data()
    {
        return mem.data();
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
    
    virtual
    ~FileSource()
    {
        _stream.close();
        free(_buffer);
    }
    
    void
    get(T &t)
    {
        if (_current == _content) readMore();
        t = _buffer[_current++];
    }

    const T &
    get()
    {
        if (_current == _content) readMore();
        return _buffer[_current++];
    }

    virtual void
    getMany(void * _ptr, const uint64_t len)
    {
        T * ptr = (T*) _ptr;
        uint64_t pos = 0;

        while(pos != len)
        {
            if (_current == _content) readMore();

            const uint64_t remaining = len - pos;
            const uint64_t available = _content - _current;
            const uint64_t toRead = min(available, remaining);

            std::copy(_buffer + _current,
                      _buffer + _current + toRead,
                      ptr + pos);

            _current += toRead;
            pos += toRead;
        }
    }

    bool
    good()
    {
        return _stream.good();
    }

private:

    void readMore()
    {
        _stream.read((char*) _buffer, sizeof(T) * _capacity);
        _content = _stream.gcount() / sizeof(T);
        _current = 0;

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
private:

    ofstream _stream;

    T * _buffer;

    uint64_t _capacity;

    uint64_t _current;

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

    virtual
    ~FileSink()
    {
        if (_current != 0)
        {
            _stream.write((char*) _buffer, sizeof(T) * _current);
        }

        _stream.flush();
        _stream.close();
        free(_buffer);
    }

    void
    put(const T &t)
    {
        _buffer[_current++] = t;

        if (_current == _capacity)
        {
            _stream.write((char*) _buffer, sizeof(T) * _current);
            _current = 0;
        }
    }

    void
    putMany(const T * ptr, const uint64_t len)
    {
        uint64_t pos = 0;

        while(pos != len)
        {
            const uint64_t required = len - pos;
            const uint64_t available = _capacity - _current;
            const uint64_t toWrite = min(required, available);

            std::copy(ptr + pos, ptr + pos + toWrite, _buffer + _current);

            _current += toWrite;
            pos += toWrite;

            if (_current == _capacity)
            {
                _stream.write((char*) _buffer, sizeof(T) * _current);
                _current = 0;
            }
        }
    }

    bool
    good()
    {
        return _stream.good();
    }

    virtual uint64_t
    size()
    {
        return 0;
    }

    virtual uint64_t
    length()
    {
        return 0;
    }

    virtual uint64_t
    capacity()
    {
        return 0;
    }

    virtual T *
    data()
    {
        return nullptr;
    }

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
        const uint64_t numBytes = getSize();
        std::string tmp(numBytes, 'a');
        getBytes((char*)tmp.c_str(), numBytes);
        return tmp;
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

    int32_t
    getInt32()
    {
        return src.get();
    }

    void
    getUInt64(uint64_t & tmp)
    {
        int32_t * root = (int32_t*) (&tmp);

        src.get(root[0]);
        src.get(root[1]);
    }

    uint64_t
    getUInt64()
    {
        uint64_t tmp;
        getUInt64(tmp);
        return tmp;
    }

    uint
    getUInt32()
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

    void
    getSize(uint64_t & tmp)
    {
        getUInt64(tmp);
    }

    uint64_t
    getSize()
    {
        return getUInt64();
    }

    void *
    getBytes()
    {
        const uint64_t numBytes = getSize();
        uint8_t * buffer = new uint8_t[numBytes];
        getBytes(buffer, numBytes);
        return buffer;
    }

    void
    getBytes(void * _buffer, uint64_t numBytes)
    {
        uint8_t * buffer = (uint8_t *) _buffer;
        const uint64_t eltos = numBytes / sizeof(int32_t);
        src.getMany(buffer, eltos);

        if (numBytes % sizeof(int32_t))
        {
            uint8_t * current = buffer + numBytes - 1;
            uint8_t * end = buffer + eltos * sizeof(uint32_t) - 1;

            int32_t tmp = src.get();

            while (current != end)
            {
                *current = tmp & 0xFF;
                tmp = tmp >> 8;
                --current;
            }
        }
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
    putInt32(const int32_t & l)
    {
        snk.put(l);
    }

    void
    putUInt64(const uint64_t & l)
    {
        const int32_t * root = (const int32_t*)( &l );

        snk.put(root[0]);
        snk.put(root[1]);
    }

    void
    putSize(const uint64_t & l)
    {
        putUInt64(l);
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
    putUInt(const uint & ui)
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

    void
    putString(const string & str)
    {
        putBytes(str.data(), str.size());
    }

    template <typename T>
    void
    putBytes(const T * const data, const uint64_t len)
    {
        const uint64_t bytes = len * sizeof(T);

        const uint8_t * data2 = (uint8_t*) data;
        const int32_t * data3 = (const int32_t*)data;

        const int eltos = bytes / sizeof(int32_t);

        putUInt64(bytes);
        snk.putMany(data3, eltos);

        if (bytes % sizeof(int32_t))
        {
            int32_t tmp = 0;

            for (uint i=eltos*sizeof(int32_t);i!=bytes;++i)
                tmp = (tmp << 8) | data2[i];

            snk.put(tmp);
        }
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

