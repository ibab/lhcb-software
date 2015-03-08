#include "FFConfig.h"

/**
 *  @class FFConfig
 *  @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
 *
 *  @brief Reads and stores the configuration parameters
 *
 *  Comments in the config file start with '#'.
 *
 *  Read the configuration file:
 *  @code
 *      FFConfig::read(filename);
 *  @endcode
 *
 *  Print all the configuration in console:
 *  @code
 *      FFConfig::print();
 *  @endcode
 *
 *  Calling the different parameters:
 *  @code
 *  double DGs = FFConfig::get<double>("DGs");
 *  std::string file = FFConfig::get<std::string>("FileInput");
 *  @endcode
 */

std::map<std::string, std::string> FFConfig::config;

void FFConfig::print()
{
    std::cout << "***********************" << std::endl;
    std::cout << "**** Configuration ****" << std::endl;
    std::cout << "***********************" << std::endl;

    for(auto it : config)
        std::cout << it.first + " = " + it.second << std::endl;

    std::cout << std::endl;
}

void FFConfig::read(const std::string& filename)
{
    std::ifstream fin(filename.c_str());

    if(!fin.is_open())
        throw "Cannot open the configuration file: " + filename;

    std::string delim = "=";
    std::string line;
    while(!fin.eof())
    {
        std::getline(fin, line);

        unsigned int pos = line.find('#');
        if(pos != std::string::npos)
            line = line.substr(0, pos);

        if(line[0] == '\n' || line.empty())
            continue;

        std::string key = clean( line.substr(0, line.find(delim)) );
        std::string value = clean( line.substr(line.find(delim) + delim.length(), line.size()) );

        config[key] = value;
    }

}

std::string FFConfig::clean(const std::string& str)
{
    unsigned int first = str.find_first_not_of(" \t");

    if(first != std::string::npos)
    {
        unsigned int last = str.find_last_not_of(" \t");

        return str.substr(first, last + 1);
    }

    return "";
}
