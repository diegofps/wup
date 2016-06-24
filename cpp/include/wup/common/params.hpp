#ifndef __WUP__COMMON__PARAMS
#define __WUP__COMMON__PARAMS

#include <vector>
#include <string>
#include <map>

namespace wup
{

class Params {
public:
    
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
    
    bool has(const char * const cmd) const
    { return !misses(cmd); }

    bool has(const char * const cmd, const int index) const
    { return !misses(cmd, index); }
    
    bool misses(const char * const cmd) const
    { return _mem.find(cmd) == _mem.end(); }

    bool misses(const char * const cmd, const int index) const
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

    const char * getStringAt(const char * const cmd, const int index, const char * const default_value) const
    {
    	if (misses(cmd, index)) return default_value;
		return _mem.at(cmd)->operator[]( index ).c_str();
    }

    const char * getString(const char * const cmd) const
    { return getStringAt(cmd, 0); }

    const char * getString(const char * const cmd, const char * const default_value) const
    { return getStringAt(cmd, 0, default_value); }


    // getIntegers
    int getIntAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parse_int); }

    int getIntAt(const char * const cmd, const int index, const int default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_int); }

    int getInt(const char * const &cmd) const
    { return parseValue(cmd, wup::parse_int); }

    int getInt(const char * const &cmd, const int default_value) const
    { return parseValue(cmd, default_value, wup::parse_int); }


    // getDoubles
    double getDoubleAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, parse_double); }

    double getDoubleAt(const char * const cmd, const int index, const double default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parse_double); }

    double getDouble(const char * const cmd) const
    { return parseValue(cmd, parse_double); }

    double getDouble(const char * const cmd, const double default_value) const
    { return parseValue(cmd, default_value, wup::parse_double); }


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
    	if (misses(cmd, index)) return default_value;
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

