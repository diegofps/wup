#include <string>
#include <vector>
#include <cmath>

#include <wup/common/msgs.hpp>
using namespace wup;


using std::string;
using std::vector;
typedef unsigned char uchar;


class base64
{

public:

    const char *binary2char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    uchar char2binary[256];

public:

    base64()
    {
        for (uchar i=0;i!=64;++i)
            char2binary[uint(binary2char[i])] = i;
        
        char2binary['+'] = char2binary['-'];
        char2binary['/'] = char2binary['_'];
        char2binary['='] = 0;
    }
    
    template <typename T>
    int encode(const vector<T> & data, const size_t size, string & chars)
    {
        return encode(&data[0], data.size() * sizeof(T), chars);
    }

    int encode(const void * const data, const size_t size, string & chars)
    {
        chars.resize(0);
        chars.reserve(ceil(chars.size() * 6.0 / 8.0) + 3);
        
        uchar * _data = (uchar*) data;
        int bufferPos = 0;
        uchar buffer;
        
        for (size_t i=0;i!=size;++i)
        {
            uchar const & c = _data[i];

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

    int decode(const string & chars, vector<uchar> & bits)
    {
        bits.resize(0);
        bits.reserve(ceil(chars.size() * 6.0 / 8.0));
    
        int bufferPos = 0;
        uchar buffer;
        
        for (const uchar & c : chars)
        {
            uchar b = char2binary[c];
            
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


