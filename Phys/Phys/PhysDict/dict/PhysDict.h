// $Id: PhysDict.h,v 1.8 2009-06-08 12:18:16 jpalac Exp $
#ifndef DICT_PHYSDICT_H 
#define DICT_PHYSDICT_H 1

// Include files

/** @file PhysDict PhysDict.h dict/PhysDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2006-10-02
 */
#include "Event/HltDecReports.h"

namespace 
{

  struct _Instantiations 
  {
    std::pair< std::string, LHCb::HltDecReport > _i1;
    GaudiUtils::VectorMap< std::string, LHCb::HltDecReport > _i2;
  };
  
}


#endif // DICT_PHYSDICT_H
