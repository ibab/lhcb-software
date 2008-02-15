// $Id: Functions.cpp,v 1.2 2008-02-15 11:57:41 jpalac Exp $
// Include files 



// local
#include "MicroDST/Functions.h"

namespace MicroDST 
{
  const std::string niceLocationName(const std::string& location)
  {
    std::string tmp(location);
    const std::string tmpString = "/Event/";
    const std::string::size_type loc       = tmp.find(tmpString);
    if ( loc != std::string::npos) {
      tmp.replace(loc, tmpString.length(), "");
    }
    return tmp;
  }

  const std::string objectLocation(const DataObject* pObject)
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

}
