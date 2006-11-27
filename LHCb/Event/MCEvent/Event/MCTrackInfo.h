// $Id: MCTrackInfo.h,v 1.2 2006-11-27 09:13:14 erodrigu Exp $
#ifndef MCEVENT_MCTRACKINFO_H 
#define MCEVENT_MCTRACKINFO_H 1

// Include files
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "Event/MCProperty.h"
#include "Event/MCTrackInfoBits.h"

// Forward declarations
class IDataProviderSvc;

namespace LHCb 
{
  class MCParticle;
}

/** @class MCTrackInfo MCTrackInfo.h Event/MCTrackInfo.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2004-01-08
 */
class MCTrackInfo {
public: 
  /// Standard constructor
  MCTrackInfo( IDataProviderSvc* eventSvc,
               IMessageSvc* msgSvc ) {
    SmartDataPtr<LHCb::MCProperty> flags( eventSvc, 
                                          LHCb::MCPropertyLocation::TrackInfo );
    if ( 0 == flags ) {
      MsgStream msg( msgSvc, "MCTrackInfo" );
      msg << MSG::ERROR 
          << "*** Flag container " << LHCb::MCPropertyLocation::TrackInfo
          << " not found." << endreq;
    }
    m_flags = flags;
  }; 

  virtual ~MCTrackInfo( ) {}; ///< Destructor

  //== Main accessors: Velo, TT, T and Velo+T for cluster and acceptance

  bool hasVelo( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskHasVelo );
  }
  
  bool hasTT( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskHasTT );
  }
  
  bool hasT( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskHasT );
  }
  
  bool hasVeloAndT( const LHCb::MCParticle* part ) {
    int want = MCTrackInfoBits::maskHasVelo + MCTrackInfoBits::maskHasT;
    return testMask( part, want );
  }

  bool accVelo( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskAccVelo );
  }
  
  bool accTT( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskAccTT );
  }
  
  bool accT( const LHCb::MCParticle* part ) {
    return testMask( part,  MCTrackInfoBits::maskAccT );
  }
  
  bool accVeloAndT( const LHCb::MCParticle* part ) {
    int want = MCTrackInfoBits::maskAccVelo + MCTrackInfoBits::maskAccT;
    return testMask( part, want );
  }

  int fullInfo( const LHCb::MCParticle* part ) {
    return m_flags->property( part );
  }

  //============= Access to individual bits, 'has' = enough digits/clusters

  bool hasVeloR( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskVeloR );
  }
  
  bool hasVeloPhi( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskVeloPhi );
  }
  
  bool hasTT1( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskTT1 );
  }
  
  bool hasTT2( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskTT2 );
  }
  
  bool hasT1X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT1X );
  }
  
  bool hasT1S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT1S );
  }
  
  bool hasT2X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT2X );
  }
  
  bool hasT2S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT2S );
  }
  
  bool hasT3X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT3X );
  }
  
  bool hasT3S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskT3S );
  }

  //=== Number of Velo clusters

  int nbVeloR( const LHCb::MCParticle* part ) { 
    int word = m_flags->property( part ) & MCTrackInfoBits::maskMultVeloR;
    return word >> MCTrackInfoBits::multVeloR;
  }
  
  int nbVeloPhi( const LHCb::MCParticle* part ) { 
    int word = m_flags->property( part ) & MCTrackInfoBits::maskMultVeloPhi;
    return word >> MCTrackInfoBits::multVeloPhi;
  }

  //============= Access to individual bits for acceptance = MCHits

  bool accVeloR( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccVeloR );
  }
  
  bool accVeloPhi( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccVeloPhi );
  }
  
  bool accTT1( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccTT1 );
  }
  
  bool accTT2( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccTT2 );
  }
  
  bool accT1X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT1X );
  }
  
  bool accT1S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT1S );
  }
  
  bool accT2X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT2X );
  }
  
  bool accT2S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT2S );
  }
  
  bool accT3X( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT3X );
  }
  
  bool accT3S( const LHCb::MCParticle* part ) { 
    return  testMask( part, MCTrackInfoBits::maskAccT3S );
  }
  
protected:
  bool testMask(  const LHCb::MCParticle* part , int mask )  {
    if ( 0 == m_flags ) return false;
    int word = m_flags->property( part );
    return ( mask == (word & mask ) );
  }
  
private:
  LHCb::MCProperty* m_flags;
};
#endif // MCEVENT_MCTRACKINFO_H
