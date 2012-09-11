// $Id: $
// Include files
#include "Photos/Photos.h"
#include "Photos/PhotosHepMCEvent.h"
#include "Photos/PhotosHepMCParticle.h"
#include "Photos/PhotosParticle.h"

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

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if ( m_pdgIdList.empty() ) 
    return Error( "PDGId list property is not set" ) ;

  for ( std::vector<int>::iterator it = m_pdgIdList.begin() ; 
        it != m_pdgIdList.end() ; ++it ) m_pdgIds.insert( *it ) ;
  
  // Initialize PHOTOS
  Photospp::Photos::initialize();
  // Set minimum photon energy (50keV at 1 GeV scale)
  Photospp::Photos::setInfraredCutOff(50.0e-6);
  // Increase the maximum possible value of the interference weight
  Photospp::Photos::maxWtInterference(4.0); // 2^n, where n = number of charges (+,-)

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
	if ( std::binary_search( m_pdgIds.begin() , m_pdgIds.end() , abs( (*itP) -> pdg_id() ) ) ) {
	  HepMC::GenVertex * EV = (*itP)->end_vertex();
	  if ( 0 == EV ) continue ;
	  
	  // Create an event with only the initial particle itP and its immediate
	  // daughters. Then call Photos++ on this event. Next, modify the original
	  // event information with the new Photos++ results.

	  Photospp::PhotosHepMCEvent photosEvent(ev);

	  // Run the Photos algorithm. 
	  // New photons should be automatically added to the (original) GenEvent pointer.
	  photosEvent.process();

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

