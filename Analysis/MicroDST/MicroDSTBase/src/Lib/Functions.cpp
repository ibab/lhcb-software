// $Id: Functions.cpp,v 1.3 2008-04-03 14:35:22 jpalac Exp $
// Include files 



// local
#include "MicroDST/Functions.h"
//Gaudi
#include <GaudiKernel/IRegistry.h>
#include <GaudiKernel/IInterface.h>
#include <GaudiKernel/IProperty.h>
#include <GaudiKernel/Property.h>
#include <GaudiKernel/DataObject.h>

namespace MicroDST 
{
  //===========================================================================
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
  //===========================================================================
  const std::string objectLocation(const DataObject* pObject)
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }
  //===========================================================================
  StatusCode synchroniseProperty(const IInterface* parent,
                                 IInterface* daughter,
                                 const std::string& name  )
  {
    StatusCode sc(StatusCode::FAILURE);
    if (Gaudi::Utils::hasProperty(parent, name) )
    {
      
      sc = Gaudi::Utils::setProperty(daughter, 
                                     name, 
                                     Gaudi::Utils::getProperty(parent, name));
    }
    return sc;
  }
  //===========================================================================
}
