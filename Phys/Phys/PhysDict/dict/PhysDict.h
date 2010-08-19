// $Id: PhysDict.h,v 1.11 2010-08-19 09:18:26 jpalac Exp $
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
#include "Event/Particle.h"
#include "Kernel/LHCbID.h"
#include "MicroDST/Particle2LHCbIDVectorMap.h"
namespace 
{

  struct _Instantiations 
  {
    std::pair< std::string, LHCb::HltDecReport >               _i1;
    std::vector< std::pair< std::string, LHCb::HltDecReport > > _i2;
    GaudiUtils::VectorMap< std::string, LHCb::HltDecReport >   _i3;
    std::vector<LHCb::RecVertex> _i4;
    std::vector<LHCb::LHCbID> _i5;
    GaudiUtils::VectorMap< const LHCb::Particle*, std::vector<LHCb::LHCbID> >   _i6;
  };
  
}


#endif // DICT_PHYSDICT_H
