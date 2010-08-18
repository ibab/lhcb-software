// $Id: $
#ifndef EVENT_VELONZSINFO_H 
#define EVENT_VELONZSINFO_H 1

// Include files
#include <string>
#include <ostream>
#include <map>
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "Tell1Kernel/VeloTell1Core.h"

/** @class VeloNZSInfo VeloNZSInfo.h Event/VeloNZSInfo.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2010-07-07
 *
 *  Data class that is meant to be used as a information
 *  storage for the emulation of the Tell1 board for VELO
 */

namespace VeloNZSInfoLocation{
  static const std::string& Default="Raw/Velo/NZSInfo";
}

class VeloNZSInfo: public ContainedObject{
public: 
  
  /// Standard constructor
  VeloNZSInfo( ): m_cntMap ( ),
                  m_isNZSStream ( false )
  {  }

  virtual ~VeloNZSInfo( ) { }   ///< Destructor

  void setIsNZSStreamPresent(bool isActive);
  bool isNZSStreamPresent() const;
  void setTell1CountersMap(VeloTELL1::Tell1CountersMap inMap);
  VeloTELL1::Tell1CountersMap Tell1CountersMap() const;
  unsigned int counter(unsigned int tell1);
  
protected:

private:
  
  VeloTELL1::Tell1CountersMap m_cntMap; 
  bool m_isNZSStream;

};
//
inline void VeloNZSInfo::setIsNZSStreamPresent(bool isActive)
{
  m_isNZSStream=isActive;
}
//
inline bool VeloNZSInfo::isNZSStreamPresent() const
{
  return ( m_isNZSStream );
}
//
inline void VeloNZSInfo::setTell1CountersMap(VeloTELL1::Tell1CountersMap map)
{
  m_cntMap=map;
}
//
inline VeloTELL1::Tell1CountersMap VeloNZSInfo::Tell1CountersMap() const
{
  return ( m_cntMap );
}
//
inline unsigned int VeloNZSInfo::counter(unsigned int tell1)
{
  unsigned int cnt=m_cntMap[tell1];
  return ( cnt );
}

typedef ObjectVector<VeloNZSInfo> VeloNZSInfos;
#endif // EVENT_VELONZSINFO_H
