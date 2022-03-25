#ifndef WUP_BASE64_HPP
#define WUP_BASE64_HPP

#include <wup/common/msgs.hpp>
#include <string>
#include <vector>
#include <cmath>

namespace wup
{

class base64
{

public:

    const char *binary2char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    unsigned char char2binary[256];

public:

    base64()
    {
        for (unsigned char i=0;i!=64;++i)
            char2binary[uint(binary2char[i])] = i;
        
        char2binary[(unsigned char)'+'] = char2binary[(unsigned char)'-'];
        char2binary[(unsigned char)'/'] = char2binary[(unsigned char)'_'];
        char2binary[(unsigned char)'='] = 0;
    }
    
    template <typename T>
    int encode(const std::vector<T> & data, const size_t size, std::string & chars)
    {
        return encode(&data[0], data.size() * sizeof(T), chars);
    }

    int encode(const void * const data, const size_t size, std::string & chars)
    {
        chars.resize(0);
        chars.reserve(ceil(chars.size() * 6.0 / 8.0) + 3);
        
        unsigned char * _data = (unsigned char*) data;
        int bufferPos = 0;
        unsigned char buffer;
        
        for (size_t i=0;i!=size;++i)
        {
            unsigned char const & c = _data[i];

            if (bufferPos == 0)
            {
                buffer = c >> 2;
                chars.push_back(binary2char[buffer]);

                buffer = (c & 0x3) << 4;
                bufferPos = 2;
            }
            
            else if (bufferPos == 2)
            {
                buffer = buffer | (c >> 4);
                chars.push_back(binary2char[buffer]);

                buffer = (c & 0xf) << 2;
                bufferPos = 4;
            }
            
            else if (bufferPos == 4)
            {
                buffer = buffer | (c >> 6);
                chars.push_back(binary2char[buffer]);

                buffer = c & 0x3f;
                chars.push_back(binary2char[buffer]);
                
                bufferPos = 0;
            }
        }
        
        if (bufferPos != 0)
        {
            chars.push_back(binary2char[buffer]);
            chars.push_back('=');
            
            if (bufferPos == 2)
                chars.push_back('=');
        }
        
        return chars.size();
    }

    int
    decode(std::string const & chars, std::vector<unsigned char> & bits)
    {
        bits.resize(0);
        bits.reserve(ceil(chars.size() * 6.0 / 8.0));
    
        int bufferPos = 0;
        unsigned char buffer;
        
        for (char const & c : chars)
        {
            unsigned char b = char2binary[(unsigned char)c];
            
            if (bufferPos == 0)
            {
                buffer = b << 2;
                bufferPos = 6;
            }
            
            else if (bufferPos == 6)
            {
                buffer = buffer | (b >> 4);
                bits.push_back(buffer);
                
                buffer = (b & 0xf) << 4;
                bufferPos = 4;
            }
            
            else if (bufferPos == 4)
            {
                buffer = buffer | (b >> 2);
                bits.push_back(buffer);
                
                buffer = (b & 0x3) << 6;
                bufferPos = 2;
            }
            
            else if (bufferPos == 2)
            {
                buffer = buffer | b;
                bits.push_back(buffer);
                
                bufferPos = 0;
            }
            
            if (c == '=')
            {
                bits.resize(bits.size() - 1);
                break;
            }
        }
        
        return bits.size();
    }

};

}

#endif /* WUP_BASE64_HPP */
