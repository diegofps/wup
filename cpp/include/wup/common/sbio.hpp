#ifndef INCLUDE_WUP_SBIO_HPP
#define INCLUDE_WUP_SBIO_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cmath>

#include <wup/common/exceptions.hpp>

using std::ofstream;
using std::ifstream;

namespace wup {

template <typename T>
class sbwriter {
public:
    
    sbwriter(const std::string &filename) : sbwriter(filename, 10240)
    {

    }
    
    sbwriter(const std::string &filename, const uint64_t capacity) :
        _stream(filename.c_str(), std::ios::binary | std::ios::trunc)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _current = 0l;
        _capacity = capacity;
    }
    
    virtual ~sbwriter()
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

template <typename T>
class sbreader {
public:
    
    sbreader(const std::string &filename) : sbreader(filename, 10240)
    {

    }
    
    sbreader(const std::string &filename, const uint64_t capacity) :
        _stream(filename.c_str(), std::ios::binary | std::ios::in)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _content = 0;
        _current = 0;
        _capacity = capacity;
    }
    
    virtual ~sbreader()
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

class ireader : public sbreader<int32_t>
{
public:
    
    ireader(const std::string & filename) : 
        sbreader( filename )
    {

    }
    
    ireader(const std::string & filename, const uint64_t capacity) :
        sbreader( filename, capacity )
    {
        
    }
    
    std::string 
    getString()
    {
        std::stringstream ss;

        int32_t tmp;
        while ((tmp = get()) != 0)
            ss << (char) tmp;

        return ss.str();
    }

    double 
    getDouble()
    {
        const int32_t i = get();
        const int32_t j = get();

        //std::cout << "Got " << i << " " << j << std::endl;
        
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
            get(*start);
            start += 1;
        }

        return l;
    }

    int64_t
    getInt64()
    {
        int64_t tmp;

        int32_t * root = (int32_t*) (&tmp);

        get(root[0]);
        get(root[1]);

        return tmp;
    }

    uint64_t
    getUInt64()
    {
        uint64_t tmp;

        int32_t * root = (int32_t*) (&tmp);

        get(root[0]);
        get(root[1]);

        return tmp;
    }

    uint
    getUnsignedInt()
    {
        uint l;
        int32_t * const root = (int32_t*) (&l);
        get(root[0]);
        return l;
    }

    bool 
    getBool()
    {
        return get() != 0;
    }
    
};


class iwriter : public sbwriter<int32_t>
{
public:
    
    iwriter(const std::string & filename) : 
        sbwriter( filename )
    {

    }
    
    iwriter(const std::string & filename, const int capacity) :
        sbwriter( filename, capacity )
    {
        
    }
    
    void 
    putBool(const bool & b)
    {
        put((int) b);
    }

    void
    putInt64(const int64_t & l)
    {
        const int32_t * root = (int32_t*)( &l );

        put(root[0]);
        put(root[1]);
    }

    void
    putUInt64(const uint64_t & l)
    {
        const uint32_t * root = (uint32_t*)( &l );

        put(root[0]);
        put(root[1]);
    }

    void
    putLong(const long & l)
    {
        const int * start = (int*)( &l );
        const int * const end = (int*)( (&l) + 1 );

        while(start != end)
        {
            put(*start);
            start += 1;
        }
    }

    void
    putUnsignedInt(const uint & ui)
    {
        const int * const root = (const int*) (& ui);
        put(root[0]);
    }

    void 
    putDouble(const double & n)
    {
        const int j = n == 0.0 ? 0 : n < 0.0 ? floor(log10(-n)) : floor(log10(n));
        const int i = n / pow(10, j - 8);

        put(i);
        put(j);
    }
    
    template <typename T>
    void 
    putString(T & str)
    {
        for (int i=0; str[i]!='\0'; ++i)
            put((int)str[i]);
        put(0);
    }

};

} /* wup */

#endif /* INCLUDE_WUP_SBIO_HPP */

