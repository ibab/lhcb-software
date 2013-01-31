// $Id: $
// Include files
#include "Photos/Photos.h"
#include "Photos/PhotosHepMCEvent.h"
#include "Photos/PhotosHepMCParticle.h"
#include "Photos/PhotosParticle.h"

// Generators
#include "Generators/RandomForGenerator.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/HepMCEvent.h"

// local
#include "ApplyPhotos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ApplyPhotos
//
// 2011-05-27 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ApplyPhotos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ApplyPhotos::ApplyPhotos( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {
    // Location of the input from the generation
    declareProperty ( "HepMCEventLocation" , m_hepMCEventLocation =
                      LHCb::HepMCEventLocation::Default ) ;
    declareProperty ( "PDGId" , m_pdgIdList ) ;
}
//=============================================================================
// Destructor
//=============================================================================
ApplyPhotos::~ApplyPhotos() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ApplyPhotos::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

  if ( m_pdgIdList.empty() ) 
    return Error( "PDGId list property is not set" ) ;

  for ( std::vector<int>::iterator it = m_pdgIdList.begin() ; 
        it != m_pdgIdList.end() ; ++it ) m_pdgIds.insert( *it ) ;
  
  // Initialize PHOTOS
  Photospp::Photos::initialize();
  
  // Give the Gauss random generator to Gauss 
  // It is already initialized in Generation !
  Photospp::Photos::setRandomGenerator( RandomForGenerator::flat );
  // Set minimum photon energy (50keV at 1 GeV scale)
  Photospp::Photos::setInfraredCutOff(1.e-7);
  // Increase the maximum possible value of the interference weight
  Photospp::Photos::maxWtInterference(64.0); // 2^n, where n = number of charges (+,-)
  Photospp::Photos::setInterference( true ) ;
  Photospp::Photos::setExponentiation( true ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ApplyPhotos::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Retrieve info from the TES
  LHCb::HepMCEvents* theEvents =
    get<LHCb::HepMCEvents>( m_hepMCEventLocation );

  LHCb::HepMCEvents::iterator it ;

  for ( it = theEvents -> begin() ; it != theEvents -> end() ; ++it ) {
    HepMC::GenEvent * ev = (*it) -> pGenEvt() ;
    for ( HepMC::GenEvent::particle_iterator itP = ev -> particles_begin() ;
          itP != ev -> particles_end() ; ++itP ) {
      if ( LHCb::HepMCEvent::DocumentationParticle != (*itP) -> status() ) {
        if ( std::binary_search( m_pdgIds.begin() , m_pdgIds.end() , 
                                 abs( (*itP) -> pdg_id() ) ) ) {
          HepMC::GenVertex * EV = (*itP) -> end_vertex() ;
          if ( 0 == EV ) continue ;

          // Make one event with only the "signal" and its decay products
          HepMC::GenEvent * newEvent = new HepMC::GenEvent( ) ;

          // Fill the event
          newEvent -> add_vertex( EV ) ;
          HepMC::GenVertex::particle_iterator iterDes ;
          for ( iterDes = EV -> particles_begin( HepMC::descendants ) ;
                iterDes != EV -> particles_end( HepMC::descendants ) ; ++iterDes ) {
            if ( 0 != (*iterDes) -> end_vertex() ) 
              newEvent -> add_vertex( (*iterDes) -> end_vertex() ) ;
          }

         // Process the event in PHOTOS
          Photospp::PhotosHepMCEvent photosEvent( newEvent );
          photosEvent.process();

          ev -> add_vertex( EV ) ;
          for ( iterDes = EV -> particles_begin( HepMC::descendants ) ;
                iterDes != EV -> particles_end( HepMC::descendants ) ; ++iterDes ) {
            if ( 0 != (*iterDes) -> end_vertex() ) 
              ev -> add_vertex( (*iterDes) -> end_vertex() ) ;
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ApplyPhotos::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

