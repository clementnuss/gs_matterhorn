
#ifndef GS_MATTERHORN_CONFIGPARSER_H
#define GS_MATTERHORN_CONFIGPARSER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


class ConfigParserSingleton {
public:
    static ConfigParserSingleton &getInstance() {
        static ConfigParserSingleton instance;
        return instance;
    }

    template<class T>
    std::vector<T> getList(const std::string &propertyName) {
        std::vector<T> v;

        for (auto &item : pt.get_child(propertyName))
            v.push_back(item.second.get_value<T>());

        return v;
    }

    template<class T>
    T getValue(const std::string &propertyName, const T &defaultValue) {
        return pt.get(propertyName, defaultValue);
    }

    void loadConfig(const std::string &configPath) {
        if (!pt.empty()) {
            pt.clear();
        }

        //TODO: check existence of config file first
        boost::property_tree::read_json(configPath, pt);
    }

private:
    explicit ConfigParserSingleton() : pt{} {}

    boost::property_tree::ptree pt;

public:

    ConfigParserSingleton(ConfigParserSingleton const &) = delete;

    void operator=(ConfigParserSingleton const &)  = delete;
};


#endif //GS_MATTERHORN_CONFIGPARSER_H
