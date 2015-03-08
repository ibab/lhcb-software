#ifndef FFCONFIG_H
#define FFCONFIG_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

class FFConfig
{
public:
    static void print();
    static void read(const std::string& filename);
    template<typename T> static T get(const std::string& key);

private:
    static std::string clean(const std::string& str);

    static std::map<std::string, std::string> config;
};

template<typename T>
T FFConfig::get(const std::string& key)
{
    std::string svalue = FFConfig::config.at(key);
    if(svalue == "true") svalue = "1";
    else if(svalue == "false") svalue = "0";

    std::istringstream ss(svalue);
    T value;
    if(!(ss >> value))
    {
        std::cout << "Conversion not possible for key: " + key << std::endl;
        exit(0);
    }

    return value;
}

#endif // FFCONFIG_H
