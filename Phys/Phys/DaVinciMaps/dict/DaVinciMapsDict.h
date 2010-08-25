#ifndef DICT_DAVINCIMAPSDICT_H 
#define DICT_DAVINCIMAPSDICT_H 1

// Include files

/** @file DaVinciMapsDict dict/DaVinciMapsDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-20
 */
#include "Event/Particle.h"
#include "Kernel/LHCbID.h"
#include "Kernel/Particle2LHCbIDs.h"
#include "Kernel/Particle2VertexBase.h"

namespace 
{
  typedef GaudiUtils::VectorMap< SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > _P2LHCbIDs;

    typedef GaudiUtils::VectorMap< SmartRef<LHCb::Particle>, SmartRef<LHCb::VertexBase> > _P2PV;

  struct _Instantiations 
  {
    std::vector<LHCb::LHCbID> _i0;
    std::pair<SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > _i1;
    std::vector< std::pair<SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > > _i2;
    _P2LHCbIDs _i3;

    std::pair<SmartRef<LHCb::Particle>, SmartRef<LHCb::VertexBase> > _i4;
    std::vector< std::pair<SmartRef<LHCb::Particle>, SmartRef<LHCb::VertexBase> > > _i5;
    _P2LHCbIDs _i6;


  };
  
}


#endif // DICT_DAVINCIMAPSDICT_H
