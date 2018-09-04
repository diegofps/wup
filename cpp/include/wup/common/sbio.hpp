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
    
    sbwriter(const std::string &filename, const uint capacity) :
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
    uint _capacity;
    uint _current;
};

template <typename T>
class sbreader {
public:
    
    sbreader(const std::string &filename) : sbreader(filename, 10240)
    {

    }
    
    sbreader(const std::string &filename, const int capacity) :
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
    uint _capacity;
    uint _current;
    uint _content;
};

class ireader : public sbreader<int>
{
public:
    
    ireader(const std::string & filename) : 
        sbreader( filename )
    {

    }
    
    ireader(const std::string & filename, const int capacity) :
        sbreader( filename, capacity )
    {
        
    }
    
    std::string 
    getString()
    {
        std::stringstream ss;

        int tmp;
        while ((tmp = get()) != 0)
            ss << (char) tmp;

        return ss.str();
    }

    double 
    getDouble()
    {
        const int i = get();
        const int j = get();

        //std::cout << "Got " << i << " " << j << std::endl;
        
        return i * pow(10, j - 8);
    }
    
    long 
    getLong()
    {
        long l;
        int * const root = (int*) & l;
        
        get(root[0]);
        get(root[1]);
        
        return l;
    }
    
    bool 
    getBool()
    {
        return get() != 0;
    }
    
};

class iwriter : public sbwriter<int>
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
    putLong(const long & l)
    {
        const int * const root = (int*) & l;
        
        put(root[0]);
        put(root[1]);
    }
    
    void 
    putDouble(const double & n)
    {
        const int j = n == 0.0 ? 0 : n < 0.0 ? floor(log10(-n)) : floor(log10(n));
        const int i = n / pow(10, j - 8);
        
        //std::cout << "Saving " << i << " " << j << std::endl;

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

