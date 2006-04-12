// $Id: GetTrackerHitsAlg.cpp,v 1.8 2006-04-12 18:43:44 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

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
#include "Event/MCHit.h"
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
static const  AlgFactory<GetTrackerHitsAlg>          s_factory ;
const        IAlgFactory& GetTrackerHitsAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetTrackerHitsAlg::GetTrackerHitsAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_gigaSvc      ( 0 )
  , m_gigaKineCnvSvc ( 0 )
{
  declareProperty( "GiGaService",    m_gigaSvcName  = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName  = IGiGaCnvSvcLocation::Kine );
  declareProperty( "MCHitsLocation", m_hitsLocation = "" );
  declareProperty( "CollectionName", m_colName = "" );
  declareProperty( "Detector",       m_detName = "" );

}

//=============================================================================
// Destructor
//=============================================================================
GetTrackerHitsAlg::~GetTrackerHitsAlg() {}; 

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
  if( "" == m_detName ) {
    fatal() << "Property Detector need to be set! " << endmsg;
    return StatusCode::FAILURE;
  }

  debug() << " The hits " << m_hitsLocation  << endmsg;
  debug() << " will be taken from G4 collection " << m_colName  << endmsg;
  debug() << " for detector " << m_detName << endmsg;

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

  // get the detector element
  m_detector = getDet<DetectorElement>(m_detName);
  
  return StatusCode::SUCCESS;
};

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
  G4HCofThisEvent* hitsCollections = 0;
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
  if( !( exist<LHCb::MCParticles>( LHCb::MCParticleLocation::Default ) ) ) {
    return Error( "MCParticles do not exist at'" 
                  + LHCb::MCParticleLocation::Default +"'" );
  }
  
  // MCParticles* parts = get<MCParticles>( MCParticleLocation::Default );

  // Get the Geant4->MCParticle table
  // Need to get the new one from MCTruthManager
  GiGaKineRefTable& table = kineSvc()->table();
  
  // reserve elements on output container
  int numOfHits = hitCollection->entries();
  hits->reserve( numOfHits );
  
  // tranform G4Hit into MCHit
  for( int iHit = 0; iHit < numOfHits; ++iHit ) { 

    // create hit
    LHCb::MCHit* mcHit = new LHCb::MCHit();

    // fill data members
    Gaudi::XYZPoint entry( (*hitCollection)[iHit]->GetEntryPos() );
    Gaudi::XYZPoint exit( (*hitCollection)[iHit]->GetExitPos() );
    mcHit->setEntry( entry );
    mcHit->setDisplacement( exit-entry );
    mcHit->setEnergy( (*hitCollection)[iHit]->GetEdep() );
    mcHit->setTime( (*hitCollection)[iHit]->GetTimeOfFlight() );
    mcHit->setP( (*hitCollection)[iHit]->GetMomentum() );

    // get sensitive detector identifier using mid point
    int detID = m_detector->sensitiveVolumeID( mcHit->midPoint() );
    mcHit->setSensDetID(detID);

    // fill reference to MCParticle   
    int trackID = (*hitCollection)[iHit]->GetTrackID();
    if( table[trackID].particle() ) {
      mcHit->setMCParticle( table[trackID].particle() );
    } else {
      warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: "
                << trackID << endmsg;
    }

    // insert it in container
    hits->add( mcHit );
  }
  
  // Check that all hits have been transformed
  if( (size_t) hits->size() != (size_t) hitCollection->entries() ) {
    return Error("MCHits and G4TrackerHitsCollection have different sizes!");
  }  

  return StatusCode::SUCCESS;
  
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetTrackerHitsAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


