// $Id: TrackAcceptance.cpp,v 1.4 2006-03-03 14:18:25 ebos Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/IRegistry.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"

// from Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// local
#include "TrackAcceptance.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackAcceptance>    s_factory;
const  IToolFactory& TrackAcceptanceFactory = s_factory;

/// Standard constructor, initializes variables
TrackAcceptance::TrackAcceptance( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool( type, name, parent )
{  
  // interfaces
  declareInterface<ITrackReconstructible>(this);

  // declare properties
  declareProperty( "minNVeloRHits",   m_minNVeloRHits   = 3 );
  declareProperty( "minNVeloPhiHits", m_minNVeloPhiHits = 3 );
  declareProperty( "minNTTHits", m_minNTTHits = 1 );
  declareProperty( "minNT1Hits", m_minNT1Hits = 1 );
  declareProperty( "minNT2Hits", m_minNT2Hits = 1 );
  declareProperty( "minNT3Hits", m_minNT3Hits = 1 );
}

/// Default destructor
TrackAcceptance::~TrackAcceptance() {};

//-------------------------------------------------------------------------
/// Initialization
//-------------------------------------------------------------------------
StatusCode TrackAcceptance::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  /// Geometry information
  debug() << " start reading geometry ..." << endreq;

  DeOTDetector* otDet = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  DeSTDetector* ttDet = getDet<DeSTDetector>( DeSTDetLocation::location("TT") );
  DeSTDetector* itDet = getDet<DeSTDetector>( DeSTDetLocation::location("IT") );

  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/Velo" );

  // get pointers to the stations

  // CHECK
  error() << "Number of TTStations is : " << ttDet->nStation() << " and should be 2 !!!" << endreq;
  error() << "Number of ITStations is : " << itDet->nStation() << " and should be 3 !!!" << endreq;

  std::vector<DeSTStation*> ttStations = ttDet->stations();
  std::vector<DeSTStation*> itStations = itDet->stations();
  m_TT1Station = ttStations[0];
  m_TT2Station = ttStations[1];
  m_IT1Station = itStations[0];
  m_IT2Station = itStations[1];
  m_IT3Station = itStations[2];
  m_OT1Station = otDet->station(1);
  m_OT2Station = otDet->station(2);
  m_OT3Station = otDet->station(3);

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
/// Checks if the MCParticle is in the Velo acceptance
//-------------------------------------------------------------------------
bool TrackAcceptance::hasVelo( MCParticle* mcPart )
{

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle> mcp2mchLink( evtSvc(), msgSvc(),
                                          MCParticleLocation::Default+"2MCHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to MCHit Linker table." << endreq;
  }

  int nVeloRHits   = 0;
  int nVeloPhiHits = 0;
  
  // Use first link with highest weight.
  MCHit* aMCHit = mcp2mchLink.first( mcPart );
  if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
  while( 0 != aMCHit ) {
    const ObjectContainerBase* parentCont = aMCHit->parent();
    const IRegistry* reg =  parentCont->registry();
    const std::string& idName = reg->identifier();

    debug() << "idName = " << idName << endreq;

    if( idName == "Velo" ) {
      std::string hitType = m_velo->type( aMCHit->sensDetID() );

      debug() << "hitType = " << hitType << endreq;
      
      if( hitType == "VeloR" ) { ++nVeloRHits; }
      else { ++nVeloPhiHits; }
    }
    aMCHit = mcp2mchLink.next();
  }

  // judge if MCParticle satisfies VELO track acceptance criteria
  bool isVeloTrack = false;
  if(( nVeloRHits >= m_minNVeloRHits ) && ( nVeloPhiHits >= m_minNVeloPhiHits ))
  {
    isVeloTrack = true;
  }

  return isVeloTrack;
}

//-------------------------------------------------------------------------
/// Checks if the MCParticle is in the TT acceptance
//-------------------------------------------------------------------------
bool TrackAcceptance::hasTT( MCParticle* mcPart ) 
{  

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle> mcp2mchLink( evtSvc(), msgSvc(),
                                          MCParticleLocation::Default+"2MCHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to MCHit Linker table." << endreq;
  }

  int nTTHits = 0;
  
  // Use first link with highest weight.
  MCHit* aMCHit = mcp2mchLink.first( mcPart );
  if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
  while( 0 != aMCHit ) {
    const ObjectContainerBase* parentCont = aMCHit->parent();
    const IRegistry* reg =  parentCont->registry();
    const std::string& idName = reg->identifier();

    debug() << "idName = " << idName << endreq;

    if( idName == "TT" ) { ++nTTHits; }
    aMCHit = mcp2mchLink.next();
  }

  // judge if MCParticle satisfies TT1 track acceptance criteria
  bool isTTTrack = false;
  if (nTTHits >= m_minNTTHits) { isTTTrack = true; }

  return isTTTrack;
}

//-------------------------------------------------------------------------
/// Checks if the MCParticle is in the T-station acceptance
//-------------------------------------------------------------------------
bool TrackAcceptance::hasSeed( MCParticle* mcPart ) 
{  

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle> mcp2mchLink( evtSvc(), msgSvc(),
                                          MCParticleLocation::Default+"2MCHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to MCHit Linker table." << endreq;
  }

  int nOT1Hits = 0;
  int nOT2Hits = 0;
  int nOT3Hits = 0;
  int nIT1Hits = 0;
  int nIT2Hits = 0;
  int nIT3Hits = 0;
  
  // Use first link with highest weight.
  MCHit* aMCHit = mcp2mchLink.first( mcPart );
  if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
  while( 0 != aMCHit ) {
    const ObjectContainerBase* parentCont = aMCHit->parent();
    const IRegistry* reg =  parentCont->registry();
    const std::string& idName = reg->identifier();

    debug() << "idName = " << idName << endreq;

    if( idName == "IT" ) { 
      // calculate center point
      XYZPoint midPoint = aMCHit->midPoint();
      // IT1 MCHit?
      if( m_IT1Station->isInside(midPoint) ) { ++nIT1Hits; }
      // IT2 MCHit?
      if( m_IT2Station->isInside(midPoint) ) { ++nIT2Hits; }
      // IT3 MCHit?
      if( m_IT3Station->isInside(midPoint) ) { ++nIT3Hits; }
    }
    else if( idName == "OT" ) { 
      // calculate center point
      XYZPoint midPoint = aMCHit->midPoint();
      // OT1 MCHit?
      if( m_OT1Station->isInside(midPoint) ) { ++nOT1Hits; }
      // OT2 MCHit?
      if( m_OT2Station->isInside(midPoint) ) { ++nOT2Hits; }
      // OT3 MCHit?
      if( m_OT3Station->isInside(midPoint) ) { ++nOT3Hits; }
    }
    aMCHit = mcp2mchLink.next();
  }
  
  // judge if MCParticle satisfies SEED track acceptance criteria
  bool isSeedTrack = false;
  if(((nOT1Hits + nIT1Hits) >= m_minNT1Hits) &&
     ((nOT2Hits + nIT2Hits) >= m_minNT2Hits) &&
     ((nOT3Hits + nIT3Hits) >= m_minNT3Hits)) { isSeedTrack = true; }

  return isSeedTrack;
}

//-------------------------------------------------------------------------
/// Checks if the MCParticle is in the long-track acceptance
//-------------------------------------------------------------------------
bool TrackAcceptance::hasVeloAndSeed( MCParticle* mcPart ) 
{  
  return ( this->hasVelo(mcPart) && this->hasSeed(mcPart) );
}
