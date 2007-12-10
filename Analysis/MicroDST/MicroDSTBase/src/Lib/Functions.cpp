// $Id: Functions.cpp,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
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
      std::cout << "TES location ID is now " << tmp << std::endl;
    }
    return tmp;
  }

  const std::string objectLocation(const DataObject* pObject)
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

}
