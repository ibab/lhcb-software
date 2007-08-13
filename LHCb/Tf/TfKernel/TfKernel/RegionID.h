#ifndef TF_REGIONID_H
#define TF_REGIONID_H

#include "VeloDet/DeVeloSensor.h"
#include "Kernel/VeloChannelID.h"

namespace Tf
{
  
  class RegionID
  {
  public:
    enum DetType { VeloR=0, VeloPhi=1, TT=2, IT=3, OT=4, UNKNOWN=5 } ;      
    RegionID() : m_dettype(UNKNOWN),m_station(0),m_layer(0),m_region(0) {}
    RegionID( const LHCb::OTChannelID& id) ;
    RegionID( const LHCb::STChannelID& id) ;
    RegionID( const LHCb::VeloChannelID& id, const DeVeloSensor* sensor) ;
    
    unsigned char type()     const { return m_dettype ; }
    unsigned char station()  const { return m_station ; }
    unsigned char layer()    const { return m_layer ; }
    unsigned char region()   const { return m_region ; }
    unsigned char veloHalf() const { return m_layer ; }
    unsigned char zone()     const { return m_region ; }
    
    struct OTIndex {
      enum { kMinStation=0, kMaxStation=2, kNStations=3, 
             kMinLayer=0, kMaxLayer=3, kNLayers=4, 
             kMinRegion=0, kMaxRegion=1, kNRegions=2 } ;
    } ;
    
    struct ITIndex {
      enum { kMinStation=0, kMaxStation=2, kNStations=3,
             kMinLayer=0, kMaxLayer=3, kNLayers=4,
             kMinRegion=0, kMaxRegion=3, kNRegions=4 } ;
    } ;


    //struct TTIndex {
    // enum { kMinStation=0, kMaxStation=1, kNStations=2,
    //	kMinLayer=0, kMaxLayer=1, kNLayers=2,
    //	kMinRegion=0, kMaxRegion=47, kNRegions=48 } ;
    //  } ;
       
       struct TTIndex {
       enum { kMinStation=0, kMaxStation=1, kNStations=2,
           MinLayer=0, kMaxLayer=1, kNLayers=2,
           kMinRegion=0, kMaxRegion=11, kNRegions=12 } ;
       } ;

    struct VeloRIndex {
      enum { kMinStation=0, kMaxStation=20, kNStations=21, 
             kMinHalf=0, kMaxHalf=1, kNHalfs=2,
             kMinZone=0, kMaxZone=3, kNZones=4 } ;
    } ;

    struct VeloPhiIndex {
      enum { kMinStation=0, kMaxStation=20, kNStations=21, 
             kMinHalf=0, kMaxHalf=1, kNHalfs=2,
             kMinZone=0, kMaxZone=1, kNZones=2 } ;
    } ;

    bool operator==(const RegionID& rhs) const {
      return m_dettype==rhs.m_dettype && m_station==rhs.m_station && m_layer==rhs.m_layer && m_region==rhs.m_region ; }
    
  private:
    unsigned char m_dettype ;
    unsigned char m_station ; ///< all indices start numbering from 0
    unsigned char m_layer   ; ///< also encodes VELO half
    unsigned char m_region  ; ///< also encodes zone on a VELO sensor in the global frame
  } ;
  
  inline RegionID::RegionID( const LHCb::OTChannelID& id )
    : m_dettype(OT), m_station( id.station()-1 ), m_layer( id.layer() ), m_region( id.quarter()/2 ) {}
  
  inline RegionID::RegionID( const LHCb::STChannelID& id )
    : m_dettype(id.isTT() ? TT : IT), m_station( id.station()-1 ), m_layer( id.layer()-1 ), 
      m_region(id.isTT() ? (id.detRegion()-1)*4 + (id.sector()-1)%4  :  id.detRegion()-1 ) {}

  inline RegionID::RegionID( const LHCb::VeloChannelID& id, const DeVeloSensor* sensor)
    : m_dettype(VeloR+static_cast<int>(sensor->isPhi()))
    , m_station(0)
    , m_layer(static_cast<unsigned char>(sensor->isRight()))
    , m_region(sensor->isR() && sensor->isDownstream() ? 3-sensor->zoneOfStrip(id.strip()) : sensor->zoneOfStrip(id.strip()))
    {;}
}

#endif
    
