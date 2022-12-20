#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <wup/third_party/json.hpp>
#include <wup/common/msgs.hpp>
#include <wup/common/str.hpp>
#include <wup/common/parsers.hpp>
#include <wup/common/repr.hpp>
#include <wup/common/io.hpp>
#include <unordered_set>


namespace wup
{

using json = nlohmann::json;

template<class T>
using has_config_constructor = std::is_constructible<T, json*>;

class ConfigReader
{
public:

    json _data;
    json * data;

    ConfigReader(std::string filepath="./configs.json") :
        data(nullptr)
    {
        info("Creating ConfigReader from file '", filepath, "'");

        std::ifstream f(filepath);
        _data = json::parse(f);
        data = &_data;

        std::unordered_map<std::string, json*> configurations;

        for (auto & pair : _data.items()) {
            std::string key = cat("%", pair.key(), "%");
            configurations[key] = & pair.value();
        }

        while(replaceReferences(_data, configurations));
    }

    ConfigReader(IntReader & reader)
    {
        info("Creating ConfigReader from reader");

        std::string jsonStr = reader.getString();
        _data = json::parse(jsonStr);
        data = &_data;

        reader.getMilestone();
    }

    void
    exportTo(IntWriter & writer)
    {
        std::string jsonStr = _data.dump();
        writer.putString(jsonStr);
        writer.putMilestone();
    }

    bool
    replaceReferences(json & data, std::unordered_map<std::string, json*> & configurations)
    {
        bool found_one_or_more = false;

        for (auto & pair : data.items())
        {
            auto & value = pair.value();

            if (value.is_string()) {
                const std::string & key = value.get<std::string>();
                if (str::startsWith(key, "%") && str::endsWith(key, "%")) {
                    auto it = configurations.find(key);
                    if (it != configurations.end()) {
                        data[pair.key()] = *it->second;
                        found_one_or_more = true;
                    } else {
                        warn("Unknown referenced name in config file:", key);
                    }
                }
            }

            else if (pair.value().is_object()) {
                if (replaceReferences(pair.value(), configurations))
                    found_one_or_more = true;
            }
        }

        return found_one_or_more;
    }

    void
    update(const std::vector<std::string> & pairs)
    {
        if (data == nullptr)
            error("Trying to update an empty config object");

        std::vector<std::string> cells1;
        std::vector<std::string> cells2;

        for (const std::string & pair : pairs) {
            str::split(pair, '=', cells1);

            if (cells1.size() != 2)
                error("Invalid config parameter:", pair);

            str::split(cells1[0], '.', cells2);

            json * current = data;

            for (std::string & key : cells2)
                current = &(*current)[key];

            std::string & value = cells1[1];

            if (current->is_boolean())
                *current = parseBool(value);

            else if (current->is_number_integer())
                *current = parseInt(value);

            else if (current->is_number_float())
                *current = parseDouble(value);

            else if (current->is_string())
                *current = value;

            else if (current->is_null()) {
                if (value == "true")
                    *current = true;

                else if (value == "false")
                    *current = false;

                else {
                    try {
                        *current = parseInt(value);
                    } catch (ParsersException&) {
                        try {
                            *current = parseDouble(value);
                        } catch (ParsersException&) {
                            *current = value;
                        }
                    }
                }
            }

            else
                error("Unknown cell type at", cells1[0]);
        }
    }

    template <typename CONFIG>
    CONFIG *
    parse()
    {
        return new CONFIG(data);
    }

};

class Config : public Repr
{
public:

    json _data;
    json * data;
    std::unordered_set<std::string> visited;
    std::string name;

    Config(json * const data, const char * const name) :
        data(data),
        name(name)
    {
        if (data != nullptr && data->contains("type"))
        {
            std::string type = (*data)["type"].get<std::string>();
            if (name != cat(type, "Config"))
                error("Configuration name is", name, "but type is present with value", type, ". This following json is not for this Config class: \n", data->dump(2));
        }
    }

    virtual
    ~Config()
    {

    }

    template <typename A>
    A * as()
    {
//        return static_cast<A>(*this);
//        return dynamic_cast<A>(*this);
//        return *dynamic_cast<A*>(this);
        return (A*)(this);
    }

    template <typename T>
    void getArray(const char * key, std::vector<T> & ds)
    {
        ds.clear();

        if (data != nullptr && data->contains(key))
        {
            json arr = (*data)[key];

            if (arr.is_array())
                for (auto & el : arr)
                    ds.push_back(el.get<T>());

            (*data).erase(key);
        }
    }

    template <typename T>
    void get(const char * key, T & attr)
    {
        if (data != nullptr && data->contains(key) && !(*data)[key].is_null())
        {
            try
            {
                attr = (*data)[key].get<T>();
                visited.insert(key);
            }
            catch(nlohmann::detail::type_error &e)
            {
                error("Failed to retrieve value from", key, "-", e.what());
            }
        }
    }

    void getBool(const char * key, bool & attr)
    {
        get<bool>(key, attr);
    }

    void getInt(const char * key, int & attr)
    {
        get<int>(key, attr);
    }

    void getUInt(const char * key, unsigned int & attr)
    {
        get<unsigned int>(key, attr);
    }

    void getString(const char * key, std::string & attr)
    {
        get<std::string>(key, attr);
    }

    void getDouble(const char * key, double & attr)
    {
        get<double>(key, attr);
    }

    void getFloat(const char * key, float & attr)
    {
        get<float>(key, attr);
    }

    template <typename CONFIG>
    void
    getConfig(const char * key, CONFIG *& attr)
    {
        if (data != nullptr && data->contains(key))
        {
            if constexpr (has_config_constructor<CONFIG>::value)
                attr = new CONFIG(&(*data)[key]);
            else
                createConfig(&(*data)[key], attr);

            visited.insert(key);
        }
        else
        {
            warn(name, "is missing config key", key);
            if constexpr (has_config_constructor<CONFIG>::value)
                attr = new CONFIG(nullptr);
            else
                createConfig(nullptr, attr);
        }
    }

    void
    finish()
    {
        if (data != nullptr)
            for (auto & pair : data->items())
                if (visited.find(pair.key()) == visited.end())
                    warn(name, "ignored attribute", pair.key(), "=", pair.value());
    }

    std::string
    repr() const
    {
        return data->dump(2);
    }

};

}

#endif
