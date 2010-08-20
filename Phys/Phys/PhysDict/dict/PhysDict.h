// $Id: PhysDict.h,v 1.12 2010-08-20 09:50:09 jpalac Exp $
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
#include "Event/RecVertex.h"
namespace 
{

  struct _Instantiations 
  {
    std::pair< std::string, LHCb::HltDecReport >               _i1;
    std::vector< std::pair< std::string, LHCb::HltDecReport > > _i2;
    GaudiUtils::VectorMap< std::string, LHCb::HltDecReport >   _i3;
    std::vector<LHCb::RecVertex> _i4;
  };
  
}


#endif // DICT_PHYSDICT_H
