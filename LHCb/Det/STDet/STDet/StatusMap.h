// 
#ifndef _StatusMap_H 
#define _StatusMap_H 1

#include<map>
#include <string>

#include "STDet/DeSTSector.h"

namespace StatusMap{
  typedef std:: map<DeSTSector::Status,std::string> StatusToStringMap;
  const StatusToStringMap& statusDescription();  
}

/** stream operator for status */
std::ostream& operator << (std::ostream& s, DeSTSector::Status e);

#endif
