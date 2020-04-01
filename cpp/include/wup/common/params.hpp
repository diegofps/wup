#ifndef __WUP__COMMON__PARAMS
#define __WUP__COMMON__PARAMS

#include <wup/common/generic.hpp>
#include <vector>
#include <string>
#include <map>

namespace wup
{

class Params {
public:

    Params()
    {
        _mem["_"] = new std::vector<std::string>();
    }

    Params(const int argc, const char ** argv)
    {
        std::vector<std::string> *list;
        int i = 0;
        
        list = new std::vector<std::string>();
        _mem["_"] = list;
        
        while(i < argc) {
            while(i < argc && argv[i][0] != '-') {
                list->push_back(argv[i]);
                ++i;
            }
            
            while(i < argc && argv[i][0] == '-') {
                list = new std::vector<std::string>();
                _mem[&argv[i][1]] = list;
                ++i;
            }
        }
    }

    Params(const int argc, char ** argv) : Params(argc, const_cast<const char **>(argv))
    {

    }

    ~Params()
    {
        for (auto it : _mem)
            delete it.second;
    }

    template <typename T>
    void
    put(const char * const cmd, T data)
    {
        auto it = _mem.find(cmd);
        if (it == _mem.end())
        {
            std::vector<string> *list = new std::vector<std::string>();
            list->push_back(cat(data));
            _mem[cmd] = list;
        }
        else
        {
            it->second->push_back(cat(data));
        }
    }

    bool has(const char * const cmd) const
    { return !misses(cmd); }

    bool has(const char * const cmd, const int index) const
    { return !misses(cmd, index); }
    
    bool misses(const char * const cmd) const
    { return _mem.find(cmd) == _mem.end(); }

    bool misses(const char * const cmd, const uint index) const
    {
        if (misses(cmd)) return true;
        return _mem.at(cmd)->size() <= index;
    }
    
    // length
    int len(const char * const cmd) const
    {
        if (misses(cmd)) missingCommand( cmd );
        return _mem.at(cmd)->size();
    }
    
    // getAll
    const std::vector<std::string> &
    all(const char * const cmd) const
    {
        if (misses(cmd)) missingCommand(cmd);
        return *(_mem.at(cmd));
    }


    // getStrings
    const char * getStringAt(const char * const cmd, const int index) const
    {
        if (misses(cmd, index)) missingCommand( cmd );
        return _mem.at(cmd)->operator[]( index ).c_str();
    }

    void setStringAt(const char * const cmd, const int index, const string str)
    {
        if (misses(cmd, index)) missingCommand( cmd );
        _mem.at(cmd)->operator[]( index ) = str;
    }

    const char * getStringAt(const char * const cmd, const int index, const char * const default_value) const
    {
        if (misses(cmd, index)) return default_value;
        return _mem.at(cmd)->operator[]( index ).c_str();
    }

    const char * getString(const char * const cmd) const
    { return getStringAt(cmd, 0); }

    const char * getString(const char * const cmd, const char * const default_value) const
    { return getStringAt(cmd, 0, default_value); }

    void getResolution(const char * const cmd, const char * const defValue, uint & width, uint & height) const
    {
        vector<string> cells;
        const string vp = getString(cmd, defValue);
        split(vp, 'x', cells);

        try
        {
            if (cells.size() == 2)
            {
                width  = parse_uint(cells[0]);
                height = parse_uint(cells[1]);
                return;
            }

            else if (cells.size() == 1)
            {
                width  = parse_uint(cells[0]);
                height = width;
                return;
            }
        } catch(ParsersException &e) { }

        error("Invalid size for -vp, format is: <WIDTH>x<HEIGHT>");
    }

    // get long
    long getLongAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_long); }

    long getLongAt(const char * const cmd, const int index, const long default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_long); }

    long getLong(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_long); }

    long getLong(const char * const &cmd, const long default_value) const
    { return parseValue(cmd, default_value, wup::parse_long); }


    // get ulong
    ulong getULongAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_ulong); }

    ulong getULongAt(const char * const cmd, const int index, const ulong default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_ulong); }

    ulong getULong(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_ulong); }

    ulong getULong(const char * const &cmd, const ulong default_value) const
    { return parseValue(cmd, default_value, wup::parse_ulong); }


    // get int
    int getIntAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_int); }

    int getIntAt(const char * const cmd, const int index, const int default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_int); }

    int getInt(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_int); }

    int getInt(const char * const &cmd, const int default_value) const
    { return parseValue(cmd, default_value, wup::parse_int); }


    // get uint
    uint getUIntAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_uint); }

    uint getUIntAt(const char * const cmd, const int index, const uint default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_uint); }

    uint getUInt(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_uint); }

    uint getUInt(const char * const &cmd, const uint default_value) const
    { return parseValue(cmd, default_value, wup::parse_uint); }


    // get short
    short getShortAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_short); }

    short getShortAt(const char * const cmd, const int index, const short default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_short); }

    short getShort(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_short); }

    short getShort(const char * const &cmd, const short default_value) const
    { return parseValue(cmd, default_value, wup::parse_short); }


    // get ushort
    ushort getUShortAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_ushort); }

    ushort getUShortAt(const char * const cmd, const int index, const ushort default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_ushort); }

    ushort getUShort(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_ushort); }

    ushort getUShort(const char * const &cmd, const ushort default_value) const
    { return parseValue(cmd, default_value, wup::parse_ushort); }


    // getBools
    bool getBoolAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_bool); }

    bool getBoolAt(const char * const cmd, const int index, const bool default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_bool); }

    bool getBool(const char * const &cmd, const bool default_value) const
    {
        if (misses(cmd))
            return default_value;

        if (len(cmd) == 0)
            return true;

        return parse_bool(_mem.at(cmd)->operator[]( 0 ));
    }

    bool getBool(const char * const &cmd) const
    {
      return getBool(cmd, false);
    }

    // get double
    double getDoubleAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, parse_double); }

    double getDoubleAt(const char * const cmd, const int index, const double default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_double); }

    double getDouble(const char * const cmd) const
    { return parseValue(cmd, parse_double); }

    double getDouble(const char * const cmd, const double default_value) const
    { return parseValue(cmd, default_value, wup::parse_double); }


    // get float
    float getFloatAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, parse_float); }

    float getFloatAt(const char * const cmd, const int index, const float default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_float); }

    float getFloat(const char * const cmd) const
    { return parseValue(cmd, parse_float); }

    float getFloat(const char * const cmd, const float default_value) const
    { return parseValue(cmd, default_value, wup::parse_float); }


    // get enum
    template <typename KEY_TYPE, typename ENUM_TYPE>
    ENUM_TYPE getEnum(const char * const cmd,
                      map<KEY_TYPE, ENUM_TYPE> & decoder,
                      const ENUM_TYPE defaultValue) const
    {
        if (misses(cmd)) return defaultValue;
        else return getEnum(cmd, decoder);
    }

    template <typename KEY_TYPE, typename ENUM_TYPE>
    ENUM_TYPE getEnum(const char * const cmd,
                      map<KEY_TYPE, ENUM_TYPE> & decoder) const
    {
        auto value = getString(cmd);
        auto it = decoder.find(value);

        if (it != decoder.end())
            return it->second;

        printn(value, "is not a valid value for", cmd, ". Valid values are:");

        for (auto pair : decoder)
            printn(" ", pair.first);

        error("Invalid value for", cmd);
    }

    // parseValues
    template <typename T> T
    parseValueAt(const char * const cmd, const int index, T (*parseFunc)(const std::string &)) const
    {
        if (misses(cmd, index)) missingParameter( cmd );
        return parseFunc( _mem.at(cmd)->operator[]( index ) );
    }

    template <typename T> T
    parseValueAt(const char * const cmd, const int index, const T &default_value, T (*parseFunc)(const std::string &)) const
    {
        if (misses(cmd, index))
            return default_value;
        else
            return parseFunc( _mem.at(cmd)->operator[]( index ) );
    }
    
    template <typename T> T
    parseValue(const char * const cmd, T (*parseFunc)(const std::string &)) const
    { return parseValueAt( cmd, 0, parseFunc ); }

    template <typename T> T
    parseValue(const char * const cmd, const T & default_value, T (*parseFunc)(const std::string &)) const
    { return parseValueAt( cmd, 0, default_value, parseFunc ); }

private:

    void missingCommand(const char * const cmd) const
    { throw ParsersException(cat("Missing command '", cmd, "'")); }

    void missingParameter(const char * const cmd) const
    { throw ParsersException(cat("Missing parameter for '", cmd, "'")); }

    void missingParameter(const char * const cmd, const int i) const
    { throw ParsersException(cat("Missing parameter number ", i," for '", cmd, "'")); }

private:
    
    std::map<std::string, std::vector<std::string>* > _mem;
    
};

}; /* wup */

#endif /* __WUP__COMMON__PARAMS */

