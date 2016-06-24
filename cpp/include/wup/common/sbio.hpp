#ifndef INCLUDE_WUP_SBIO_HPP
#define INCLUDE_WUP_SBIO_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using std::ofstream;
using std::ifstream;

namespace wup {

template <typename T>
class sbwriter {
public:
    
    sbwriter(const std::string &filename) : sbwriter(filename, 10240)
    { }
    
    sbwriter(const std::string &filename, const int capacity) :
        _stream(filename.c_str(), std::ios::binary | std::ios::trunc)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _current = 0;
        _capacity = capacity;
    }
    
    ~sbwriter()
    {
        if (_current != 0) {
            _stream.write((char*) _buffer, sizeof(T) * _current);
        }

        _stream.flush();
        _stream.close();        
        free(_buffer);
    }
    
    void put(const T &t)
    {
        _buffer[_current++] = t;
        
        if (_current == _capacity) {
            _stream.write((char*) _buffer, sizeof(T) * _current);
            _current = 0;
        }
    }
    
    bool good()
    { return _stream.good(); }
    
private:
    ofstream _stream;
    T * _buffer;
    int _capacity;
    int _current;
};

template <typename T>
class sbreader {
public:
    
    sbreader(const std::string &filename) : sbreader(filename, 10240)
    { }
    
    sbreader(const std::string &filename, const int capacity) :
        _stream(filename.c_str(), std::ios::binary | std::ios::in)
    {
        _buffer = (T*) malloc(sizeof(T) * capacity);
        _content = 0;
        _current = 0;
        _capacity = capacity;
    }
    
    ~sbreader()
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
    { return _stream.good(); }

private:

    void readMore() {
        _stream.read((char*) _buffer, sizeof(T) * _capacity);
        int chars = _stream.gcount();
        _content = _stream.gcount() / sizeof(T);
        _current = 0;
//        print("Read ", chars, " characters, giving ", _content, " entities");

        if (_current == _content)
            throw wup::WUPException();
    }

private:
    ifstream _stream;
    T * _buffer;
    int _capacity;
    int _current;
    int _content;
};

} /* wup */

#endif /* INCLUDE_WUP_SBIO_HPP */

