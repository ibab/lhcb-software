#ifndef DICT_MICRODSTDICT_H 
#define DICT_MICRODSTDICT_H 1

// Include files

/** @file MicroDstDict dict/MicroDstDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-20
 */
#include "Event/Particle.h"
#include "Kernel/LHCbID.h"
#include "Kernel/Particle2LHCbIDs.h"
#include "Kernel/Particle2VertexBase.h"
#include "Kernel/Particle2UnsignedInts.h"
#include "Kernel/DataObject2ObjectMap.h"

namespace 
{
  typedef GaudiUtils::VectorMap< SmartRef<LHCb::Particle>, 
                                 std::vector<LHCb::LHCbID> > _P2LHCbIDs;

  typedef DaVinci::Map::DataObject2ObjectMap< LHCb::Particle, 
                                              std::vector<LHCb::LHCbID> > _P2LHCbIDMap;

  typedef GaudiUtils::VectorMap< SmartRef<LHCb::Particle>, 
                                 SmartRef<LHCb::VertexBase> > _P2PV;

  typedef GaudiUtils::VectorMap< SmartRef<LHCb::Particle>, 
                                 std::vector<unsigned int> > _P2UInts;
  typedef DaVinci::Map::DataObject2ObjectMap< LHCb::Particle, 
                                              std::vector<unsigned int> > _P2UIntMap;

  struct _Instantiations 
  {
    std::vector<LHCb::LHCbID> _i00;
    std::pair<SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > _i01;
    std::vector< std::pair<SmartRef<LHCb::Particle>, std::vector<LHCb::LHCbID> > > _i02;
    _P2LHCbIDs _i03;
    _P2LHCbIDMap _i04;
    
    std::pair<SmartRef<LHCb::Particle>, SmartRef<LHCb::VertexBase> > _i10;
    std::vector< std::pair<SmartRef<LHCb::Particle>, SmartRef<LHCb::VertexBase> > > _i11;
    _P2LHCbIDs _12;

    std::pair<SmartRef<LHCb::Particle>, std::vector<unsigned int> > _i21;
    std::vector< std::pair<SmartRef<LHCb::Particle>, std::vector<unsigned int> > > _i22;
    _P2UInts _i23;
    _P2UIntMap _i24;


  };
  
}


#endif // DICT_MICRODSTDICT_H
