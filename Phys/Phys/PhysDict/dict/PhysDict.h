// $Id: PhysDict.h,v 1.5 2009-06-04 08:18:08 jpalac Exp $
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
  static bool operator==(const LHCb::HltDecReport& lhs, 
                         const LHCb::HltDecReport& rhs) 
  {
    return &lhs == & rhs;
  }

  static bool operator<(const LHCb::HltDecReport& lhs, 
                         const LHCb::HltDecReport& rhs) 
  {
    return &lhs < & rhs;
  }
  

  struct _Instantiations 
  {
    GaudiUtils::VectorMap< std::string, LHCb::HltDecReport > _i2;
  };
  
}


#endif // DICT_PHYSDICT_H
