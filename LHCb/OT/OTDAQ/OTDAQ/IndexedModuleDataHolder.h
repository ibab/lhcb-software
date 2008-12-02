#ifndef INDEXEDMODULEDATAHOLDER_H
#define INDEXEDMODULEDATAHOLDER_H

#include "Kernel/OTChannelID.h"
#include "OTDAQ/OTEnum.h"

namespace OTDAQ
{
  class IndexedModuleCounter
  {
  private:
    size_t m_counter ;
  public:
    IndexedModuleCounter() : m_counter(0) {}
    bool operator==(const IndexedModuleCounter& rhs) const { return m_counter == rhs.m_counter ; }
    IndexedModuleCounter& operator++() { ++m_counter ; return *this ; }
    IndexedModuleCounter& operator--() { --m_counter ; return *this ; }
    size_t station()  const { return OffsetStations  + m_counter / (NumLayers*NumQuadrants*NumModules) ; }
    size_t layer()    const { return OffsetLayers    + (m_counter/(NumQuadrants*NumModules))%NumLayers ; }
    size_t quadrant() const { return OffsetQuadrants + (m_counter/NumModules)%NumQuadrants ; }
    size_t module()   const { return OffsetModules   + (m_counter%NumModules) ; }
    size_t counter()  const { return m_counter ; }
    LHCb::OTChannelID channel() const { return LHCb::OTChannelID(station(),layer(),quadrant(),module(),0) ; }
  } ;

  template<class MODULE>
  class IndexedModuleDataConstIterator : public IndexedModuleCounter
  {
  private:
    const MODULE* m_data ;
  public:
    IndexedModuleDataConstIterator(const MODULE* data) : m_data(data) {}
    bool operator==(const IndexedModuleDataConstIterator& rhs) const { return IndexedModuleCounter::operator==(rhs) ; }
    bool operator==(const MODULE* pointer) const { return m_data + counter() == pointer ; }
    bool operator!=(const MODULE* pointer) const { return m_data + counter() != pointer ; }
    const MODULE& operator*() const { return *(m_data + counter()) ; }
    const MODULE* operator->() const { return m_data + counter() ; }
  } ;

  template<class MODULE>
  class IndexedModuleDataIterator : public IndexedModuleCounter
  {
  private:
    MODULE* m_data ;
  public:
    IndexedModuleDataIterator(MODULE* data) : m_data(data) {}
    bool operator==(const IndexedModuleDataIterator& rhs) const { return IndexedModuleCounter::operator==(rhs) ; }
    bool operator==(const MODULE* pointer) const { return m_data + counter() == pointer ; }
    bool operator!=(const MODULE* pointer) const { return m_data + counter() != pointer ; }
    MODULE& operator*() const { return *(m_data + counter()) ; }
    MODULE* operator->() const { return m_data + counter() ; }
  } ;
  
  template<class MODULE>
  class IndexedModuleDataHolder
  {
  public:
    typedef MODULE value_type ;
    typedef value_type* iterator ;
    typedef const value_type* const_iterator ;
    typedef IndexedModuleDataConstIterator<value_type> const_mapiterator ;
    typedef IndexedModuleDataIterator<value_type> mapiterator ;
    
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
