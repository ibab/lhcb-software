// $Id: GetTrackerHitsAlg.cpp,v 1.2 2005-10-30 21:49:15 gcorti Exp $
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
  declareProperty( "GiGaService",    m_gigaSvcName    = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName    = IGiGaCnvSvcLocation::Kine );
  declareProperty( "MCHitsLocation", m_hitsLocation   = "/Event/MC/BCM/Hits" );
  declareProperty( "CollectionName", m_colName        = "BCMSDet/Hits" );
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

  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName ); // GiGa has to already exist!

  // get kineCnv service that hold the MCParticle/Geant4 table list
  m_gigaKineCnvSvc = svc<IGiGaKineCnvSvc>( m_kineSvcName );

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
  MCHits* hits = new MCHits();
  StatusCode sc = put( hits, m_hitsLocation );
  if( sc.isFailure() ) {
    return Error( " Unable to register MCHits in " + m_hitsLocation );
  }                    
  
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
  if( !( exist<MCParticles>( MCParticleLocation::Default ) ) ) {
    return Error( "MCParticles do not exist at'" 
                  + MCParticleLocation::Default +"'" );
  }
  
  // MCParticles* parts = get<MCParticles>( MCParticleLocation::Default );

  // Get the Geant4->MCParticle table
  GiGaKineRefTable& table = kineSvc()->table();
  
  // reserve elements on output container
  int numOfHits = hitCollection->entries();
  hits->reserve( numOfHits );
  
  // tranform G4Hit into MCHit
  for( int iHit = 0; iHit < numOfHits; ++iHit ) { 

    // create hit
    MCHit* mcHit = new MCHit();

    // fill data members
    HepPoint3D entry = (*hitCollection)[iHit]->GetEntryPos();
    HepPoint3D exit  = (*hitCollection)[iHit]->GetExitPos();
    mcHit->setEntry( entry );
    mcHit->setDisplacement( exit-entry );
    mcHit->setEnergy( (*hitCollection)[iHit]->GetEdep() );
    mcHit->setTimeOfFlight( (*hitCollection)[iHit]->GetTimeOfFlight() );

    // fill reference to MCParticle   
    int trackID = (*hitCollection)[iHit]->GetTrackID();
    if( table[trackID].particle() ) {
      mcHit->setMCParticle( table[trackID].particle() );
    } else {
      warning() << "No pointer to MCParticle for MCHit associated to G4 trackID: "
                << trackID << endmsg;
    }

    // insert it in container
    hits->insert( mcHit );
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


