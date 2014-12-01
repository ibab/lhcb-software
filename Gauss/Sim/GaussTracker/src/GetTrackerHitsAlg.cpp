// $Id: GetTrackerHitsAlg.cpp,v 1.16 2009-03-26 21:52:20 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaHitsByName.h"

// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
#include "GiGaCnv/GiGaKineRefTable.h"

// from Geant4
#include "G4HCofThisEvent.hh"

// from LHCb
#include "Event/MCExtendedHit.h"
#include "DetDesc/DetectorElement.h"

// local
#include "GetTrackerHitsAlg.h"
#include "TrackerHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetTrackerHitsAlg
//
// 2005-10-02 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetTrackerHitsAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetTrackerHitsAlg::GetTrackerHitsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
{
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa",
                   "The service handling the intreface to Geant4" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine,
                   "The service keeping the relation between Geant4 kinematic and MCTruth" );
  declareProperty( "ExtendedInfo",   m_extendedInfo = false, 
                   "Flag to control filling of MCExtendedHits instead of MCHits (def = false)" );
  declareProperty( "MCHitsLocation", m_hitsLocation = "",
                   "Location in TES where to put resulting MCHits" );
  declareProperty( "CollectionName", m_colName = "",
                   "Name of Geant4 collection where to retrieve hits" );
  declareProperty( "Detectors",      m_detName,
                   "List of detector paths in TDS for which to retrieve the hits (most of the time one" );
  declareProperty( "MCParticles",    m_mcParticles = LHCb::MCParticleLocation::Default,
		   "Location of MCParticles" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
GetTrackerHitsAlg::~GetTrackerHitsAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetTrackerHitsAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if( "" == m_hitsLocation ) {
    fatal() << "Property MCHitsLocation need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }
  if( "" == m_colName ) {
    fatal() << "Property CollectionName need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }
  if( !m_detName.size() ) {
    fatal() << "Property Detector need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::iterator itDet;
  debug() << " The hits " << m_hitsLocation  << endmsg;
  debug() << " will be taken from G4 collection " << m_colName  << endmsg;
  debug() << " for detector(s) ";
  for( itDet=m_detName.begin(); itDet!=m_detName.end(); itDet++ ){
    debug() << *itDet << " ";
  }
  debug() << endmsg;
  
  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

  // get the detector element
  for( itDet=m_detName.begin(); itDet!=m_detName.end(); itDet++ ){
    m_detector.push_back( getDet<DetectorElement>(*itDet) );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetTrackerHitsAlg::execute() {

  debug() << "==> Execute" << endmsg;

  if( 0 == gigaSvc() ) {
    return Error( " execute(): IGiGaSvc* points to NULL" );
  }

  // Create the MCHits and put them in the TES
  // Cannot use 
  // MCHits* hits = getOrCreate<MCHits,MCHits>( m_hitsLocation );
  // because triggers convertion
  LHCb::MCHits* hits = new LHCb::MCHits();
  put( hits, m_hitsLocation );
  
  // Get the G4 necessary hit collection from GiGa
  GiGaHitsByName col( m_colName );
  *gigaSvc() >> col;   // also StatusCode sc = retrieveHitCollection( col );
                       // in TRY/CATCH&PRINT
  
  if( 0 == col.hits() ) { 
    return Warning( "The hit collection='" + m_colName + "' is not found!",
                    StatusCode::SUCCESS ); 
  }
  
  const TrackerHitsCollection* hitCollection = trackerHits( col.hits() );
  if( 0 == hitCollection ) { 
    return Error( "Wrong Collection type" );
  }
  
  // The MCParticles should have already been filled
  if( !( exist<LHCb::MCParticles>( m_mcParticles ) ) ) {
    return Error( "MCParticles do not exist at'" 
                  + m_mcParticles +"'" );
  }
  
  // reserve elements on output container
  int numOfHits = hitCollection->entries();
  if( numOfHits > 0 ) {
    hits->reserve( numOfHits );
  }

  // tranform G4Hit into MCHit and insert it in container
  for( int iG4Hit = 0; iG4Hit < numOfHits; ++iG4Hit ) { 
    
    // create hit or extended hit depending on choice
    if ( m_extendedInfo ) {
      LHCb::MCExtendedHit* newHit = new LHCb::MCExtendedHit();
      fillHit( (*hitCollection)[iG4Hit], newHit );
      Gaudi::XYZVector mom( (*hitCollection)[iG4Hit]->GetMomentum() );
      newHit->setMomentum( mom );
      hits->add( newHit );
    }
    else {
      LHCb::MCHit* newHit = new LHCb::MCHit();
      fillHit( (*hitCollection)[iG4Hit], newHit );
      hits->add( newHit );
    }
  }
  
  // Check that all hits have been transformed
  if( (size_t) hits->size() != (size_t) hitCollection->entries() ) {
    return Error("MCHits and G4TrackerHitsCollection have different sizes!");
  }  

  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetTrackerHitsAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Fill MCHit 
//=============================================================================

void GetTrackerHitsAlg::fillHit( TrackerHit* g4Hit, LHCb::MCHit* mcHit ) {
  
  // fill data members
  Gaudi::XYZPoint entry( g4Hit->GetEntryPos() );
  Gaudi::XYZPoint exit( g4Hit->GetExitPos() );
  mcHit->setEntry( entry );
  mcHit->setDisplacement( exit-entry );
  mcHit->setEnergy( g4Hit->GetEdep() );
  mcHit->setTime( g4Hit->GetTimeOfFlight() );
  mcHit->setP( g4Hit->GetMomentum().mag() );
 
  // get sensitive detector identifier using mid point
  int detID = -1;
  std::vector<const DetectorElement*>::iterator itDet;
  for( itDet=m_detector.begin(); itDet!=m_detector.end(); itDet++){
    if( (*itDet)->isInside(mcHit->midPoint()) ){
      detID = (*itDet)->sensitiveVolumeID( mcHit->midPoint() );
      break;
    }
  }
  mcHit->setSensDetID(detID);

  // fill reference to MCParticle using the Geant4->MCParticle table
  GiGaKineRefTable& table = kineSvc()->table();
  int trackID = g4Hit->GetTrackID();
  if( table[trackID].particle() ) {
    mcHit->setMCParticle( table[trackID].particle() );
  } else {
    warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: "
              << trackID << endmsg;
  }
  
}

//=============================================================================
