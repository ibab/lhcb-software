// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/HepMCEvent.h"

// local
#include "ApplyPhotos.h"

// FORTRAN declarations
extern "C" void phoini_() ;
extern "C" void begevtgenstorex_(int *,int *,int *,int *,
                                int *,int *,int *,int *,
                                double *,double *,double *,
                                double *,double *,double *,
                                double *,double *,double *);
extern "C" void begevtgengetx_(int *,int *,int *,int *,
                              int *,int *,int *,int *,
                              double *,double *,double *,
                              double *,double *,double *,
                              double *,double *,double *);

extern "C" void photos_(int *);

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
    declareProperty ( "PDGId" , m_pdgId = 23 ) ;
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

  // Initialize PHOTOS
  phoini_() ;

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
      if ( m_pdgId == abs( (*itP) -> pdg_id() ) && ( (*itP) -> status() != 3 )) {
        HepMC::GenVertex * EV = (*itP)->end_vertex();
        if ( 0 == EV ) continue ;

        // Store the Z0 parameters in the HEPEVT common block
        int entry = 1 ;
        int eventnum = 1 ;
        int numparticle = 1 ;
        int istat = 2 ;
        int partnum = (*itP) -> pdg_id() ;
        int mother = 0 ;
        int daugfirst = 2 ;
        int dauglast = 1 + EV->particles_out_size();
        double px = (*itP)->momentum().px();
        double py = (*itP)->momentum().py();
        double pz = (*itP)->momentum().pz();
        double e  = (*itP)->momentum().e();
        double m  = (*itP)->momentum().m();
        double x = 0. ;
        double y = 0. ;
        double z = 0. ;
        double t = 0. ;

        begevtgenstorex_(&entry,&eventnum,&numparticle,&istat,&partnum,
                         &mother,&daugfirst,&dauglast,
                         &px,&py,&pz,&e,&m,&x,&y,&z,&t);


        HepMC::GenVertex::particle_iterator iter ;
        for ( iter = EV -> particles_begin( HepMC::children ) ;
              iter != EV -> particles_end( HepMC::children ) ; ++iter ) {
          entry++ ;
          numparticle++ ;
          istat = 1 ;
          partnum = (*iter)->pdg_id() ;
          mother = 1 ;
          daugfirst = 0 ;
          dauglast = 0 ;
          px = (*iter)->momentum().px() ;
          py = (*iter)->momentum().py() ;
          pz = (*iter)->momentum().pz() ;
          e  = (*iter)->momentum().e()  ;
          m  = (*iter)->momentum().m()  ;

          begevtgenstorex_(&entry,&eventnum,&numparticle,&istat,&partnum,
                           &mother,&daugfirst,&dauglast,
                           &px,&py,&pz,&e,&m,&x,&y,&z,&t);

        }
        // call PHOTOS
        entry = 1 ;
        photos_( &entry ) ;

        // get back the parameters and update the event
        int numparticlephotos ;
        begevtgengetx_(&entry,&eventnum,&numparticlephotos,&istat,&partnum,
                       &mother,&daugfirst,&dauglast,
                       &px,&py,&pz,&e,&m,&x,&y,&z,&t);
        if ( numparticlephotos == numparticle ) continue ;

        int i = 0 ;
        int np ;
        for ( iter = EV -> particles_begin( HepMC::children ) ;
              iter != EV -> particles_end( HepMC::children ) ; ++iter ) {
          entry = i + 2 ;

          begevtgengetx_(&entry,&eventnum,&np,&istat,&partnum,
                          &mother,&daugfirst,&dauglast,
                          &px,&py,&pz,&e,&m,&x,&y,&z,&t);

          double mp = (*iter)->momentum().m() ;
          e = sqrt(mp*mp+px*px+py*py+pz*pz) ;
          (*iter)->set_momentum(HepMC::FourVector(px,py,pz,e)) ;
          i++ ;
        }

	// Add the new photons
	for ( entry = numparticle + 1 ; entry <= numparticlephotos ; ++entry ) {
          begevtgengetx_(&entry,&eventnum,&np,&istat,&partnum,
			 &mother,&daugfirst,&dauglast,
			 &px,&py,&pz,&e,&m,&x,&y,&z,&t) ;
	  e = sqrt( px*px + py*py + pz*pz ) ;
	  HepMC::GenParticle * new_photon = new
	    HepMC::GenParticle( HepMC::FourVector( px , py , pz , e ) , 22 , 
				LHCb::HepMCEvent::StableInDecayGen ) ;
	  EV -> add_particle_out( new_photon ) ;
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

