#ifndef TrackKeys_H
#define TrackKeys_H 1

#include "Event/Track.h"
#include <map>

//! @author J.A. Hernando
//! @date 13/04/2005
  

class TrackKeys {
  
public:
  
  TrackKeys();
  
  static TrackKeys s_trackKeys;  
  
public:
  
  static std::vector<std::string> History;
  
  static std::vector<std::string> HistoryFit;
  
  static std::vector<std::string> Types;
  
  static std::vector<std::string> Status;
  
  static std::vector<std::string> Flags;
};

namespace Keys {

  unsigned int index(const std::vector<std::string>& keys, 
                     const std::string& key);
  
  unsigned int bitIndex(const std::vector<std::string>& keys, 
                        const std::string& key);
  
  const std::string& key(const std::vector<std::string>& keys, 
                        unsigned int index);
}

#endif
