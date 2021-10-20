#ifndef __WUP__COMMON__PARAMS
#define __WUP__COMMON__PARAMS

#include <wup/third_party/json.hpp>
#include <wup/common/parsers.hpp>
#include <wup/common/generic.hpp>
#include <wup/common/str.hpp>
#include <vector>
#include <string>
#include <map>

namespace wup
{

template <typename TYPE>
nlohmann::json
map_to_json(std::map<std::string, TYPE*> & mapping)
{
    nlohmann::json data;

    for (auto & p : mapping)
        data[p.first] = p.second->to_json();

    return data;
}

template <typename TYPE>
nlohmann::json
map_to_json(std::map<std::string, TYPE> & mapping)
{
    nlohmann::json data;

    for (auto & p : mapping)
        data[p.first] = p.second;

    return data;
}

template <typename TYPE>
void
map_from_json(nlohmann::json & data,
              std::map<std::string, TYPE*> & mapping,
              bool deleteData=true)
{
    if (deleteData)
        for (auto & p : mapping)
            delete p.second;

    mapping.clear();

    for (auto & p : data.items())
        mapping[p.key()] = new TYPE(p.value());
}

template <typename TYPE>
void
map_from_json(nlohmann::json & data, std::map<std::string, TYPE> & mapping)
{
    mapping.clear();
    for (auto & p : data.items())
        mapping[p.key()] = TYPE(p.value());
}

template <typename TYPE>
nlohmann::json
vector_to_json(std::vector<TYPE*> & listing)
{
    nlohmann::json data;

    for (auto & v : listing)
        data.push_back(v->to_json());

    return data;
}

template <typename TYPE>
nlohmann::json
vector_to_json(std::vector<TYPE> & listing)
{
    return json(listing);
}

template <typename TYPE>
void
vector_from_json(nlohmann::json & data,
                 std::vector<TYPE*> & listing,
                 bool deleteData=true)
{
    if (deleteData)
        for (auto & v : listing)
            delete v;

    listing.clear();

    for (auto & v : data)
        listing.push_back(new TYPE(v));
}

template <typename TYPE>
void
vector_from_json(nlohmann::json & data,
                 std::vector<TYPE> & listing)
{
    listing.clear();
    for (auto & v : data)
        listing.push_back(v.get<TYPE>());
}


class Param
{
public:

    bool active;
    std::vector<std::string> args;

public:

    Param() :
        active(true)
    {

    }

    Param(nlohmann::json & data)
    {
        active = data["active"].get<bool>();
        vector_from_json(data["args"], args);
    }

    nlohmann::json
    to_json()
    {
        nlohmann::json data;
        data["active"] = active;
        data["args"] = args;
        return data;
    }

};


class ParamNameSpace
{
public:

    std::string name;
    std::map<std::string, Param*> mem;

public:

    ParamNameSpace(const char * name) : name(name)
    {
        mem["_"] = new Param();
    }

    ~ParamNameSpace()
    {
        for (auto it : mem)
            delete it.second;
    }

    ParamNameSpace(nlohmann::json & data)
    {
        name = data["name"].get<std::string>();
        map_from_json(data["mem"], mem);
    }

    nlohmann::json
    to_json()
    {
        nlohmann::json data;
        data["name"] = name;
        data["mem"] = map_to_json(mem);
        return data;
    }

};



class Params {
private:

private:

    std::map<std::string, ParamNameSpace*> namespaces;
    ParamNameSpace * ns;

public:

    Params()
    {
        ns = new ParamNameSpace("_");
        namespaces["_"] = ns;
    }

    Params(const int argc, const char * const argv[]) :
        Params()
    {
        Param * param = ns->mem["_"];
        std::string tmp("-ns");
        int i = 0;
        
        while(i < argc)
        {
            while(i < argc && argv[i][0] != '-')
            {
                param->args.push_back(argv[i]);
                ++i;
            }
            
            while(i < argc && tmp == argv[i])
            {
                if (i+1 >= argc || argv[i+1][0] == '-')
                    error("Missing namespace name after -ns");

                auto name = argv[i+1];
                auto newNs = new ParamNameSpace(name);
                auto it = namespaces.find(name);

                if (it != namespaces.end())
                {
                    warn("Namespace", name, "was declared twice");
                    delete it->second;
                }

                namespaces[name] = newNs;
                param = newNs->mem["_"];
                ns = newNs;

                i += 2;
            }

            while(i < argc && tmp != argv[i] && argv[i][0] == '-')
            {
                param = new Param();
                ns->mem[&argv[i][1]] = param;
                ++i;
            }
        }
    }

    Params(const int argc, char ** argv) :
        Params(argc, const_cast<const char **>(argv))
    {

    }

    ~Params()
    {
        for (auto it : namespaces)
            delete it.second;
    }

    Params(nlohmann::json & data)
    {
        map_from_json(data["namespaces"], namespaces);
        ns = namespaces["_"];
    }

    nlohmann::json
    to_json()
    {
        nlohmann::json data;
        data["namespaces"] = map_to_json(namespaces);
        return data;
    }

    void
    show(std::ostream & o) const
    {
        for (auto it : namespaces)
        {
            o << it.first << ":" << std::endl;
            for (auto it2 : it.second->mem)
            {
                o << "\t" << it2.first << ":";
                for (auto & it3 : it2.second->args)
                    o << " " << it3;
                o << std::endl;
            }
        }
    }

    Params &
    use(const char * const nsName)
    {
        auto it = namespaces.find(nsName);

        if (it == namespaces.end())
        {
            ns = new ParamNameSpace(nsName);
            namespaces[nsName] = ns;
        }
        else
        {
            ns = it->second;
        }

        return *this;
    }

    template <typename T>
    void
    put(const char * const cmd, T data)
    {
        auto it = ns->mem.find(cmd);
        if (it == ns->mem.end())
        {
            auto list = new Param();
            list->args.push_back(cat(data));
            ns->mem[cmd] = list;
        }
        else
        {
            it->second->args.push_back(cat(data));
        }
    }

    void
    drop(const char * const cmd)
    {
        auto it = ns->mem.find(cmd);
        if (it != ns->mem.end())
        {
            delete it->second;
            ns->mem.erase(it);
        }
    }

    bool has(const char * const cmd) const
    { return !misses(cmd); }

    bool has(const char * const cmd, const int index) const
    { return !misses(cmd, index); }
    
    bool misses(const char * const cmd) const
    { return ns->mem.find(cmd) == ns->mem.end(); }

    bool misses(const char * const cmd, const uint index) const
    {
        if (misses(cmd)) return true;
        return ns->mem.at(cmd)->args.size() <= index;
    }
    
    // length
    int len(const char * const cmd) const
    {
        if (misses(cmd)) missingCommand( cmd );
        return ns->mem.at(cmd)->args.size();
    }
    
    // getAll
    const std::vector<std::string> &
    all(const char * const cmd) const
    {
        if (misses(cmd)) missingCommand(cmd);
        return ns->mem.at(cmd)->args;
    }


    // getStrings
    const std::string getStringAt(const char * const cmd, const int index) const
    {
        if (misses(cmd, index)) missingCommand( cmd );
        return ns->mem.at(cmd)->args[index].c_str();
    }

    void setStringAt(const char * const cmd, const int index, const std::string str)
    {
        if (misses(cmd, index)) missingCommand( cmd );
        ns->mem.at(cmd)->args[index] = str;
    }

    const std::string getStringAt(const char * const cmd, const int index, const std::string default_value) const
    {
        if (misses(cmd, index)) return default_value.c_str();
        return ns->mem.at(cmd)->args[index].c_str();
    }

    const std::string getString(const char * const cmd) const
    { return getStringAt(cmd, 0); }

    const std::string getString(const char * const cmd, const std::string default_value) const
    { return getStringAt(cmd, 0, default_value); }

    void getResolution(const char * const cmd, const char * const defValue, uint & width, uint & height) const
    {
        std::vector<std::string> cells;
        const std::string vp = getString(cmd, defValue);
        str::split(vp, 'x', cells);

        try
        {
            if (cells.size() == 2)
            {
                width  = parseUInt(cells[0]);
                height = parseUInt(cells[1]);
                return;
            }

            else if (cells.size() == 1)
            {
                width  = parseUInt(cells[0]);
                height = width;
                return;
            }
        } catch(ParsersException &e) { }

        error("Invalid size for -vp, format is: <WIDTH>x<HEIGHT>");
    }

    // get long
    long getLongAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseLong); }

    long getLongAt(const char * const cmd, const int index, const long default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseLong); }

    long getLong(const char * const &cmd) const
    { return parseValue(cmd, wup::parseLong); }

    long getLong(const char * const &cmd, const long default_value) const
    { return parseValue(cmd, default_value, wup::parseLong); }


    // get ulong
    unsigned long getULongAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseULong); }

    unsigned long getULongAt(const char * const cmd, const int index, const unsigned long default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseULong); }

    unsigned long getULong(const char * const &cmd) const
    { return parseValue(cmd, wup::parseULong); }

    unsigned long getULong(const char * const &cmd, const unsigned long default_value) const
    { return parseValue(cmd, default_value, wup::parseULong); }


    // get int
    int getIntAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseInt); }

    int getIntAt(const char * const cmd, const int index, const int default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseInt); }

    int getInt(const char * const &cmd) const
    { return parseValue(cmd, wup::parseInt); }

    int getInt(const char * const &cmd, const int default_value) const
    { return parseValue(cmd, default_value, wup::parseInt); }


    // get uint
    uint getUIntAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseUInt); }

    uint getUIntAt(const char * const cmd, const int index, const uint default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseUInt); }

    uint getUInt(const char * const &cmd) const
    { return parseValue(cmd, wup::parseUInt); }

    uint getUInt(const char * const &cmd, const uint default_value) const
    { return parseValue(cmd, default_value, wup::parseUInt); }


    // get short
    short getShortAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseShort); }

    short getShortAt(const char * const cmd, const int index, const short default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseShort); }

    short getShort(const char * const &cmd) const
    { return parseValue(cmd, wup::parseShort); }

    short getShort(const char * const &cmd, const short default_value) const
    { return parseValue(cmd, default_value, wup::parseShort); }


    // get ushort
    unsigned short getUShortAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseUShort); }

    unsigned short getUShortAt(const char * const cmd, const int index, const unsigned short default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseUShort); }

    unsigned short getUShort(const char * const &cmd) const
    { return parseValue(cmd, wup::parseUShort); }

    unsigned short getUShort(const char * const &cmd, const unsigned short default_value) const
    { return parseValue(cmd, default_value, wup::parseUShort); }


    // getBools
    bool getBoolAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, wup::parseBool); }

    bool getBoolAt(const char * const cmd, const int index, const bool default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseBool); }

    bool getBool(const char * const &cmd, const bool default_value) const
    {
        if (misses(cmd))
            return default_value;

        if (len(cmd) == 0)
            return true;

        return parseBool(ns->mem.at(cmd)->args[0]);
    }

    bool getBool(const char * const &cmd) const
    {
      return getBool(cmd, false);
    }

    // get double
    double getDoubleAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, parseDouble); }

    double getDoubleAt(const char * const cmd, const int index, const double default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseDouble); }

    double getDouble(const char * const cmd) const
    { return parseValue(cmd, parseDouble); }

    double getDouble(const char * const cmd, const double default_value) const
    { return parseValue(cmd, default_value, wup::parseDouble); }


    // get float
    float getFloatAt(const char * const cmd, const int index) const
    { return parseValueAt(cmd, index, parseFloat); }

    float getFloatAt(const char * const cmd, const int index, const float default_value) const
    { return parseValueAt(cmd, index, default_value, wup::parseFloat); }

    float getFloat(const char * const cmd) const
    { return parseValue(cmd, parseFloat); }

    float getFloat(const char * const cmd, const float default_value) const
    { return parseValue(cmd, default_value, wup::parseFloat); }


    // get enum
    template <typename KEY_TYPE, typename ENUM_TYPE>
    ENUM_TYPE getEnum(const char * const cmd,
                      std::map<KEY_TYPE, ENUM_TYPE> & decoder,
                      const ENUM_TYPE defaultValue) const
    {
        if (misses(cmd)) return defaultValue;
        else return getEnum(cmd, decoder);
    }

    template <typename KEY_TYPE, typename ENUM_TYPE>
    ENUM_TYPE getEnum(const char * const cmd,
                      std::map<KEY_TYPE, ENUM_TYPE> & decoder) const
    {
        auto value = getString(cmd);
        auto it = decoder.find(value);

        if (it != decoder.end())
            return it->second;

        printn(value, "is not a valid value for", cmd, ". Valid values are:");

        for (auto pair : decoder)
            printn(" ", pair.first);

        error("Invalid value for", cmd);
        return ENUM_TYPE();
    }


    // pop functions
    template <typename DECODER, typename ENUM>
    void
    popEnum(const char * const cmd, ENUM & variable)
    {
        DECODER decoder;
        variable = getEnum(cmd, decoder, variable);
        drop(cmd);
    }

    template <typename V1>
    void
    popInt(const char * const cmd, V1 & v1)
    {
        v1 = getInt(cmd, v1);
        drop(cmd);
    }

    template <typename V1>
    void
    popUInt(const char * const cmd, V1 & v1)
    {
        v1 = getUInt(cmd, v1);
        drop(cmd);
    }

    template <typename V1>
    void
    popBool(const char * const cmd, V1 & v1)
    {
        v1 = getBool(cmd, v1);
        drop(cmd);
    }

    template <typename V1>
    void
    popDouble(const char * const cmd, V1 & v1)
    {
        v1 = getDouble(cmd, v1);
        drop(cmd);
    }

    template <typename V1>
    void
    popFloat(const char * const cmd, V1 & v1)
    {
        v1 = getFloat(cmd, v1);
        drop(cmd);
    }

    template <typename V1>
    void
    popString(const char * const cmd, V1 & v1)
    {
        v1 = getString(cmd, v1);
        drop(cmd);
    }

    template <typename V1, typename V2>
    void
    popInt(const char * const cmd, V1 & v1, V2 & v2)
    {
        if (has(cmd))
        {
            v1 = getIntAt(cmd, 0, v1);
            v2 = getIntAt(cmd, 1, v1);
            drop(cmd);
        }
    }

    template <typename V1, typename V2>
    void
    popUInt(const char * const cmd, V1 & v1, V2 & v2)
    {
        if (has(cmd))
        {
            v1 = getUIntAt(cmd, 0, v1);
            v2 = getUIntAt(cmd, 1, v1);
            drop(cmd);
        }
    }



    void
    warnUnused()
    {
        for (auto it1 : namespaces)
        {
            for (auto it2 : it1.second->mem)
            {
                if (it2.first != "_")
                    printn(BRIGHTER, "Unused parameter: ", YELLOW, it1.first, ".", it2.first, NORMAL, "\n");

                else if (it2.second->args.size() != (it1.first == "_" ? 1 : 0))
                    printn(BRIGHTER, "Unused parameter(s) at start of namespace ", YELLOW, it1.first, NORMAL, "\n");
            }
        }
    }

    // parseValues
    template <typename T> T
    parseValueAt(const char * const cmd, const int index, T (*parseFunc)(const std::string &)) const
    {
        if (misses(cmd, index)) missingParameter( cmd );
        return parseFunc( ns->mem.at(cmd)->args[index]);
    }

    template <typename T> T
    parseValueAt(const char * const cmd, const int index, const T &default_value, T (*parseFunc)(const std::string &)) const
    {
        if (misses(cmd, index))
            return default_value;
        else
            return parseFunc( ns->mem.at(cmd)->args[index] );
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

};

inline std::ostream &
operator<<(std::ostream & o, const Params & p)
{
    p.show(o);
    return o;
}

}; /* wup */

#endif /* __WUP__COMMON__PARAMS */

