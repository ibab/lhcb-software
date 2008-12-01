#ifndef INDEXEDMODULEDATAHOLDER_H
#define INDEXEDMODULEDATAHOLDER_H

#include "Kernel/OTChannelID.h"

namespace OTDAQ
{
  template<class MODULE>
  class IndexedModuleDataHolder
  {
  public:
    enum { OffsetStations=1, OffsetLayers=1, OffsetQuadrants=1, OffsetModules=0 } ;
    enum { NumStations=3, NumLayers=4, NumQuadrants=4, NumModules=9 } ;
    typedef MODULE value_type ;
    typedef value_type* iterator ;
    typedef const value_type* const_iterator ;

    IndexedModuleDataHolder() {}
    
    MODULE& module(const unsigned int station, const unsigned int layer, 
                   const unsigned int quarter, const unsigned int module) {
      return m_modules[station-1][layer][quarter][module-1] ;
    }
    
    MODULE& module(const LHCb::OTChannelID& id) {
      return module(id.station(),id.layer(),id.quarter(),id.module()) ;
    }

    iterator begin() { return &(m_modules[0][0][0][0]) ; }
    iterator end() { return &(m_modules[NumStations-1][NumLayers-1][NumQuadrants-1][NumModules]); }
    const_iterator begin() const { return &(m_modules[0][0][0][0]) ; }
    const_iterator end() const { return &(m_modules[NumStations-1][NumLayers-1][NumQuadrants-1][NumModules]); }
    
    bool isvalidID(const unsigned int station, const unsigned int layer, 
                   const unsigned int quarter, const unsigned int module) const {
      return station-1<=NumStations-1 && layer<=NumLayers-1  && quarter<=NumQuadrants-1 && module-1<=NumModules-1 ; }
  private: 
    MODULE m_modules[NumStations][NumLayers][NumQuadrants][NumModules];
  } ;
  
}

#endif
