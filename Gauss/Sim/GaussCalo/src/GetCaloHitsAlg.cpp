// $Id: GetCaloHitsAlg.cpp,v 1.6 2009-03-26 21:38:44 robbep Exp $
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

// Event 
#include "Event/MCCaloHit.h"

// local
#include "CaloHit.h"
#include "CaloSubHit.h"
#include "GetCaloHitsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetCaloHitsAlg
//
// 2005-11-14 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetCaloHitsAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetCaloHitsAlg::GetCaloHitsAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) ,
    m_gigaSvc( 0 ) , 
    m_gigaKineCnvSvc( 0 ) {
    declareProperty( "GiGaService" , m_gigaSvcName = "GiGa" ) ;
    declareProperty( "KineCnvService" , 
                     m_kineSvcName = IGiGaCnvSvcLocation::Kine ) ;
    declareProperty( "MCHitsLocation" , m_hitsLocation = "" ) ;
    declareProperty( "CollectionName" , m_colName = "" ) ;
    declareProperty( "MCParticles"    , 
                     m_mcParticles = LHCb::MCParticleLocation::Default ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
GetCaloHitsAlg::~GetCaloHitsAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GetCaloHitsAlg::initialize() {
  // Initialize base class
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc ;

  debug() << "==> Initialise" << endmsg ;

  // Get GiGa Service 
  m_gigaSvc = svc< IGiGaSvc >( m_gigaSvcName ) ;

  // Check GiGa service exists
  if ( 0 == m_gigaSvc ) 
    return Error( "execute() : IGiGaSvc* points to NULL" ) ;
  
  // get kineCnv service holding MCParticle/Geant4 table
  m_gigaKineCnvSvc = svc< IGiGaKineCnvSvc >( m_kineSvcName ) ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetCaloHitsAlg::execute() {
  debug() << "==> Execute" << endmsg ;

  // Register output container to contain MCCaloHits
  LHCb::MCCaloHits * hits = new LHCb::MCCaloHits( ) ;
  put( hits , m_hitsLocation ) ;
  
  // Get the G4 hit collections corresponding to Calo
  GiGaHitsByName col( m_colName ) ;
  *m_gigaSvc >> col ;
  if ( 0 == col.hits( ) ) 
    return Warning( "The hit collection='" + m_colName + "' is not found ! " ,
                    StatusCode::SUCCESS ) ;
  
  // Now cast to Calo hits collections
  const CaloHitsCollection * hitCollection = caloHits( col.hits( ) ) ;
  if ( 0 == hitCollection ) return Error( "Wrong collection type" ) ;
  
  // Get the reference table between G4 tracks and MC particles
  if ( ! exist< LHCb::MCParticles >( m_mcParticles ) ) 
    return Error( "LHCb::MCParticles do not exist at'" +
                  m_mcParticles + "'" ) ;
  const GiGaKineRefTable & table = m_gigaKineCnvSvc -> table() ;

  const size_t numOfHits = hitCollection -> entries() ;
  if ( numOfHits > 0 ) {
    hits->reserve( numOfHits );
  }
  
  // transform G4Hit in MCHits:
  // Loop over all hits in collection
  for ( size_t iHit = 0 ; iHit < numOfHits ; ++iHit ) {
    // The calo hit in the calorimeter
    const CaloHit * hit = (*hitCollection)[ iHit ] ;
    if ( 0 == hit ) continue ;

    // Loop over all sub hits of this calo hit
    for ( CaloHit::iterator iter = hit -> begin() ; iter != hit -> end( ) ;
          ++iter ) {
      // The sub hit
      const CaloSubHit * subhit = iter -> second ;
      if ( 0 == subhit ) continue ;
      
      // Pointer to the corresponding LHCb::MCParticle using trackID of the subhit
      const LHCb::MCParticle * mcp = table( subhit -> trackID() ).particle( ) ;
      if ( 0 == mcp ) 
        warning() << "No pointer to LHCb::MCParticle for MCHit associated to G4 "
                  << "trackID: " << subhit -> trackID() << endmsg ;
      
      // Loop over all energy/time deposits strored in the subhit
      for ( CaloSubHit::iterator entry = subhit -> begin() ; 
            entry != subhit -> end() ; ++entry ) {
        // Create the new MCHit
        LHCb::MCCaloHit * mchit = new LHCb::MCCaloHit() ;
        // Fill it with:
        //   - Calorimeter CellID of the hit
        mchit -> setCellID( hit -> cellID() ) ;
        //   - Time when the energy is deposited
        mchit -> setTime( entry -> first ) ;
        //   - Active energy deposited
        mchit -> setActiveE( entry -> second ) ;
        //   - Pointer to the LHCb::MCParticle giving the hit
        mchit -> setParticle( mcp ) ;

        // Now insert in output container
        hits -> add( mchit ) ;
      }
    } 
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetCaloHitsAlg::finalize() {
  debug() << "==> Finalize" << endmsg ;

  release( m_gigaSvc ) ;
  release( m_gigaKineCnvSvc ) ;

  return GaudiAlgorithm::finalize( ) ;
}

//=============================================================================
