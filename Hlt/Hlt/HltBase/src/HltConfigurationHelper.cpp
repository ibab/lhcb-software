#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

#include "HltBase/HltConfigurationHelper.h"


int Hlt::ConfigurationHelper::getID(Hlt::Configuration& conf,
                                    const std::string& root, 
                                    const std::string& name) {
  std::string key = root+"/"+name; 
  int id = 0;
  if (conf.has_key(key)) { 
    id = conf.retrieve<int>(key);
  } else {
    boost::hash<std::string> hasher; 
    id = hasher(name); 
    conf.add(key,id); 
    key = root+"/"+ boost::lexical_cast<std::string>(id); 
    conf.add(key,name);
  }
  return id;
}
  
std::string Hlt::ConfigurationHelper::getName(Hlt::Configuration& conf,
                                              const std::string& root, 
                                              int id) {
  std::string key = root+"/"+boost::lexical_cast<std::string>(id);
  return conf.has_key(key) ? conf.retrieve<std::string>(key)
                           : std::string("unknown");
}

bool Hlt::ConfigurationHelper::validID(Hlt::Configuration& conf,
                                       const std::string& root, 
                                       const std::string& name) {
  std::string key = root+"/"+name;
  return conf.has_key(key);
}






