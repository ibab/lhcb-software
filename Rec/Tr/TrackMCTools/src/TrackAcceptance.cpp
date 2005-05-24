// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from Det
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"

// from Event
#include "Event/MCVeloHit.h"

// local
#include "TrackMCTools/TrackAcceptance.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackAcceptance
//
// 2003-07-04 : J. van Tilburg
//
//  3-7-2002: Rutger van der Eijk, Jeroen van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackAcceptance>    s_factory;
const  IToolFactory& TrackAcceptanceFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAcceptance::TrackAcceptance( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool( type, name, parent )
  , m_velo(0)
  , m_TT1Station(0)
  , m_TT2Station(0)
  , m_IT1Station(0)
  , m_IT2Station(0)
  , m_IT3Station(0)
  , m_OT1Station(0)
  , m_OT2Station(0)
  , m_OT3Station(0)
  , m_p2VeloHitAsct(0)
  , m_p2ITHitAsct(0)
  , m_p2OTHitAsct(0)
{  
  // interfaces
  declareInterface<ITrackReconstructible>(this);

  // declare properties
  declareProperty( "MCP2VeloMCHitAscName",
                   m_p2VeloHitAsctName = "MCP2VeloMCHitAsc");
  declareProperty( "MCP2ITMCHitAscName", m_p2ITHitAsctName = "MCP2ITMCHitAsc");
  declareProperty( "MCP2OTMCHitAscName", m_p2OTHitAsctName = "MCP2OTMCHitAsc");
  declareProperty( "minNVeloRHits", m_minNVeloRHits = 3);
  declareProperty( "minNVeloPhiHits", m_minNVeloPhiHits = 3);
  declareProperty( "minNTTHits", m_minNTTHits = 1);
  declareProperty( "minNT1Hits", m_minNT1Hits = 1);
  declareProperty( "minNT2Hits", m_minNT2Hits = 1);
  declareProperty( "minNT3Hits", m_minNT3Hits = 1);
}

//=============================================================================
// Destructor
//=============================================================================
TrackAcceptance::~TrackAcceptance() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackAcceptance::initialize()
{
  StatusCode sc;

  ///Geometry information
  debug() << " start reading geometry ..." << endreq;

  DeOTDetector* otDet = getDet<DeOTDetector>( DeOTDetectorLocation::Default );

  DeSTDetector* stDet = getDet<DeSTDetector>( DeSTDetectorLocation::Default );

  m_velo = getDet<DeVelo>( "/dd/Structure/LHCb/Velo" );

  // get pointers to the stations
  m_TT1Station = stDet -> station( 1 );
  m_TT2Station = stDet -> station( 2 );
  m_IT1Station = stDet -> station( 3 );
  m_IT2Station = stDet -> station( 4 );
  m_IT3Station = stDet -> station( 5 );
  m_OT1Station = otDet -> station( 1 );
  m_OT2Station = otDet -> station( 2 );
  m_OT3Station = otDet -> station( 3 );

  // Retrieve MCParticle 2 Velo MCHit associator
  std::string ascVeloType = "Associator<MCParticle,MCVeloHit>";
  sc = toolSvc()->retrieveTool(ascVeloType, 
                               m_p2VeloHitAsctName, m_p2VeloHitAsct);
  if ( sc.isFailure() ) { 
    error() << "Unable to retrieve Velo MCHit Associator " 
            << m_p2VeloHitAsctName << endreq;
    return sc;
  }

  // Retrieve MCParticle 2 OT MCHit associator
  std::string ascType = "Associator<MCParticle,MCHit>";
  sc = toolSvc()->retrieveTool(ascType, m_p2OTHitAsctName, m_p2OTHitAsct);
  if ( sc.isFailure() ) { 
    error() << "Unable to retrieve OT MCHit Associator " 
            << m_p2OTHitAsctName << endreq;
    return sc;
  }

  // Retrieve MCParticle 2 IT MCHit associator
  sc = toolSvc()->retrieveTool(ascType, m_p2ITHitAsctName, m_p2ITHitAsct);
  if ( sc.isFailure() ) { 
    error() << "Unable to retrieve IT MCHit Associator " 
            << m_p2ITHitAsctName << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Checks if the MCParticle is in the Velo acceptance
//=============================================================================
bool TrackAcceptance::hasVelo( MCParticle* mcPart ) 
{
  int nVeloRHits   = 0;
  int nVeloPhiHits = 0;
  
  // loop over associated Velo MCHits and count # hits
  MCVeloHitAsct::ToRange mcVeloHitsRange = m_p2VeloHitAsct->rangeFrom(mcPart);
  MCVeloHitAsct::ToIterator it;
  for ( it = mcVeloHitsRange.begin(); it != mcVeloHitsRange.end(); ++it) { 
    // retrieve MCHit
    MCVeloHit* mcVeloHit = it->to();
    if ( !mcVeloHit ) {
      error() << "Failed retrieving Velo MCHit." << endreq;
    }
    else {
      // R or Phi sensor?
      // calculate center point
      HepPoint3D midPoint = (mcVeloHit->entry() + mcVeloHit->exit())/2.0;
      int staNr = m_velo -> sensorNumber( midPoint );   
      if ( m_velo -> isRSensor( staNr ) ) {
        ++nVeloRHits;
      }
      else {
        ++nVeloPhiHits;
      }
    }
  }

  // judge if MCParticle satisfies VELO track acceptance criteria
  bool isVeloTrack = false;
  if ((nVeloRHits >= m_minNVeloRHits) && (nVeloPhiHits >= m_minNVeloPhiHits)) {
    isVeloTrack = true;
  }
  return isVeloTrack;
}

//=============================================================================
// Checks if the MCParticle is in the TT acceptance
//=============================================================================
bool TrackAcceptance::hasTT( MCParticle* mcPart ) 
{  
  int nTTHits = 0;

  // loop over associated IT MCHits and count # TT hits
  MCHitAsct::ToRange mcITHitsRange = m_p2ITHitAsct->rangeFrom(mcPart);
  MCHitAsct::ToIterator it;
  for ( it = mcITHitsRange.begin(); it != mcITHitsRange.end(); ++it) { 
    // retrieve MCHit
    MCHit* mcHit = it -> to();
    if ( !mcHit ) {
      error() << "Failed retrieving TT MCHit" << endmsg;
    }
    else {
      // calculate center point
      HepPoint3D midPoint = (mcHit->entry() + mcHit->exit())/2.0;

      // TT1 MCHit?
      if ( m_TT1Station -> isInside( midPoint.z() ) ) {
        ++nTTHits;
      }
      // TT2 MCHit?
      if ( m_TT2Station -> isInside( midPoint.z() ) ) {
        ++nTTHits;
      }
    }
  }
  // judge if MCParticle satisfies TT1 track acceptance criteria
  bool isTTTrack = false;
  if (nTTHits >= m_minNTTHits) {
    isTTTrack = true;
  }
  return isTTTrack;
}

//=============================================================================
// Checks if the MCParticle is in the T-station acceptance
//=============================================================================
bool TrackAcceptance::hasSeed( MCParticle* mcPart ) 
{  
  int nOT1Hits = 0;
  int nOT2Hits = 0;
  int nOT3Hits = 0;
  int nIT1Hits = 0;
  int nIT2Hits = 0;
  int nIT3Hits = 0;

  // loop over associated OT MCHit s and count # hits
  MCHitAsct::ToRange mcOTHitsRange = m_p2OTHitAsct->rangeFrom(mcPart);
  MCHitAsct::ToIterator ot;
  for ( ot = mcOTHitsRange.begin(); ot != mcOTHitsRange.end(); ++ot) { 
    // retrieve MCHit
    MCHit* mcHit = ot -> to();
    if (!mcHit) {
      error() << "Failed retrieving OT MCHit " << endreq;
    }
    else {
      // calculate center point
      HepPoint3D midPoint = (mcHit->entry() + mcHit->exit())/2.0;
    
      // OT1 MCHit?
      if ( m_OT1Station -> isInside( midPoint ) ) {
        ++nOT1Hits;
      }
    
      // OT2 MCHit?
      if ( m_OT2Station -> isInside( midPoint ) ) {
        ++nOT2Hits;
      }
    
      // OT3 MCHit?
      if ( m_OT3Station -> isInside( midPoint ) ) {
        ++nOT3Hits;
      }
    }
  }

  // loop over associated IT MCHit s and count # hits in T1-T3
  MCHitAsct::ToRange mcITHitsRange = m_p2ITHitAsct->rangeFrom(mcPart);
  MCHitAsct::ToIterator it;
  for ( it = mcITHitsRange.begin(); it != mcITHitsRange.end(); ++it) { 
    // retrieve MCHit
    MCHit* mcHit = it->to();
    if (!mcHit) {
      error() << "Failed retrieving IT MCHit " << endreq;
    }
    else {
      // calculate center point
      HepPoint3D midPoint = (mcHit->entry() + mcHit->exit())/2.0;

      // IT1 MCHit?
      if ( m_IT1Station->isInside(midPoint.z()) ) {
        ++nIT1Hits;
      }
    
      // IT2 MCHit?
      if ( m_IT2Station->isInside(midPoint.z()) ) {
        ++nIT2Hits;
      }
    
      // IT3 MCHit?
      if ( m_IT3Station->isInside(midPoint.z()) ) {
        ++nIT3Hits;
      }
    }
  }

  // judge if MCParticle satisfies SEED track acceptance criteria
  bool isSeedTrack = false;
  if (((nOT1Hits + nIT1Hits) >= m_minNT1Hits) &&
      ((nOT2Hits + nIT2Hits) >= m_minNT2Hits) &&
      ((nOT3Hits + nIT3Hits) >= m_minNT3Hits)) {
    isSeedTrack = true;
  }
  return isSeedTrack;
}

//=============================================================================
// Checks if the MCParticle is in the long-track acceptance
//=============================================================================
bool TrackAcceptance::hasVeloAndSeed( MCParticle* mcPart ) 
{  
  return ( this->hasVelo(mcPart) && this->hasSeed(mcPart) );
}
