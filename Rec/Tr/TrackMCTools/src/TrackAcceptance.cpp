// $Id: TrackAcceptance.cpp,v 1.5 2006-03-13 16:26:46 erodrigu Exp $
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
  declareProperty( "MinNVeloRHits",   m_minNVeloRHits   = 3 );
  declareProperty( "MinNVeloPhiHits", m_minNVeloPhiHits = 3 );
  declareProperty( "MinNTTHits", m_minNTTHits = 1 );
  declareProperty( "MinNT1Hits", m_minNT1Hits = 1 );
  declareProperty( "MinNT2Hits", m_minNT2Hits = 1 );
  declareProperty( "MinNT3Hits", m_minNT3Hits = 1 );
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

  // Geometry information
  DeOTDetector* otDet = getDet<DeOTDetector>( DeOTDetectorLocation::Default );
  DeSTDetector* ttDet = getDet<DeSTDetector>( DeSTDetLocation::location("TT") );
  DeSTDetector* itDet = getDet<DeSTDetector>( DeSTDetLocation::location("IT") );

  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );

  // get pointers to the stations
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
  int nVeloRHits   = 0;
  int nVeloPhiHits = 0;

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle>
    mcp2mchLink( evtSvc(), msgSvc(), MCParticleLocation::Default+"2MCVeloHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to Velo MCHit Linker table."
            << endreq;
  }
  else {
    // Use first link with highest weight
    MCHit* aMCHit = mcp2mchLink.first( mcPart );
    if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
    while( 0 != aMCHit ) {
      std::string hitType = m_velo->type( aMCHit->sensDetID() );    
      if ( hitType == "VeloR" ) { ++nVeloRHits; }
      else { ++nVeloPhiHits; }
      
      aMCHit = mcp2mchLink.next();
    }
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
  int nTTHits = 0;

  // Retrieve MCParticle to MCHit linker table
  LinkedTo<MCHit,MCParticle>
    mcp2mchLink( evtSvc(), msgSvc(), MCParticleLocation::Default+"2MCTTHits" );
  if( mcp2mchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to TT MCHit Linker table."
            << endreq;
  }
  else {
    // Use first link with highest weight
    MCHit* aMCHit = mcp2mchLink.first( mcPart );
    if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
    while( 0 != aMCHit ) {
      ++nTTHits;
      aMCHit = mcp2mchLink.next();
    }
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
  int nOT1Hits = 0;
  int nOT2Hits = 0;
  int nOT3Hits = 0;
  int nIT1Hits = 0;
  int nIT2Hits = 0;
  int nIT3Hits = 0;

  // Retrieve MCParticle to MCHit linker table for IT
  LinkedTo<MCHit,MCParticle>
    mcp2ITmchLink( evtSvc(), msgSvc(), MCParticleLocation::Default+"2MCITHits" );
  if( mcp2ITmchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to IT MCHit Linker table."
            << endreq;
  }
  else {
    // Use first link with highest weight
    MCHit* aMCHit = mcp2ITmchLink.first( mcPart );
    if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
    while( 0 != aMCHit ) {
      // calculate center point
      XYZPoint midPoint = aMCHit->midPoint();
      // IT1 MCHit?
      if( m_IT1Station->isInside(midPoint) ) { ++nIT1Hits; }
      // IT2 MCHit?
      else if( m_IT2Station->isInside(midPoint) ) { ++nIT2Hits; }
      // IT3 MCHit?
      else if( m_IT3Station->isInside(midPoint) ) { ++nIT3Hits; }
      
      aMCHit = mcp2ITmchLink.next();
    }
  }

  // Retrieve MCParticle to MCHit linker table for OT
  LinkedTo<MCHit,MCParticle>
    mcp2OTmchLink( evtSvc(), msgSvc(), MCParticleLocation::Default+"2MCOTHits" );
  if( mcp2OTmchLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to IT MCHit Linker table."
            << endreq;
  }
  else {
    // Use first link with highest weight
    MCHit* aMCHit = mcp2OTmchLink.first( mcPart );
    if( 0 == aMCHit ) return Error( "No MCHit linked to this MCParticle" );
    while( 0 != aMCHit ) {
      // calculate center point
      XYZPoint midPoint = aMCHit->midPoint();
      // OT1 MCHit?
      if( m_OT1Station->isInside(midPoint) ) { ++nOT1Hits; }
      // OT2 MCHit?
      else if( m_OT2Station->isInside(midPoint) ) { ++nOT2Hits; }
      // OT3 MCHit?
      else if( m_OT3Station->isInside(midPoint) ) { ++nOT3Hits; }
      
      aMCHit = mcp2OTmchLink.next();
    }
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
