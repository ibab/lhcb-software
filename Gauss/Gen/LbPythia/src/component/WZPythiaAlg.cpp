// $Id: WZPythiaAlg.cpp,v 1.1 2005-10-03 12:08:54 robbep Exp $
// Include files 
// local
#include "WZPythiaAlg.h"

#include "Generators/IEvtGenTool.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Generators/HepMCUtils.h"

#include "HepMC/IO_HEPEVT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : WZPythiaAlg
//
// 2004-12-16 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<WZPythiaAlg>          s_factory ;
const        IAlgFactory& WZPythiaAlgFactory = s_factory ; 

// Forward declarations of selection functions
bool IsLepton    ( const HepMC::GenParticle * p ) ;
bool Isb         ( const HepMC::GenParticle * p ) ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
WZPythiaAlg::WZPythiaAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : PythiaAlg ( name , pSvcLocator ){
    declareProperty( "Useb" , m_useb = false ) ;
}

//=============================================================================
// Destructor
//=============================================================================
WZPythiaAlg::~WZPythiaAlg() {};

//=============================================================================
// Some initialization
//=============================================================================
StatusCode WZPythiaAlg::genInitialize() {  
  return PythiaAlg::genInitialize() ;                       
} 

//=============================================================================
// Convert HEPEVT into HepMC format (simple conversion )
//=============================================================================
StatusCode WZPythiaAlg::fillEvt( HepMC::GenEvent * evt ) {
  HepMC::IO_HEPEVT hepio ;
  hepio.fill_next_event( evt ) ;

  // We have to decay all particles with EvtGen before analyzing the event
  GenParticles theHepMCVector ;
  HepMCUtils::SortHepMC( theHepMCVector , evt -> particles_size() ,
                               evt -> particles_begin() ,
                               evt -> particles_end() ) ;
  
  // Disable Flip
  m_evtTool -> disableFlip( ) ;

  GenParticles::iterator iter ;
  for ( iter = theHepMCVector.begin( ) ; iter != theHepMCVector.end( ) ; 
        ++iter ) {
    if ( 1 == (*iter) -> status( ) ) {
      if ( m_evtTool -> isKnownToEvtGen( (*iter) -> pdg_id() ) ) {
        (*iter) -> set_status( 888 ) ;
        if ( 0 == (*iter) -> end_vertex( ) ) {
          StatusCode sc = m_evtTool -> generateDecay( evt , (*iter) ) ;
          if ( ! sc.isSuccess() ) 
            return Error( "Unable to decay particle in EvtGen" , sc ) ;
        }
      }
    }
  }        

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Selection function
//=============================================================================
bool WZPythiaAlg::selectEvt( HepMC::GenEvent * evt ) {
  debug() << "==> Analyze event" << endreq ;

  // Find leptons
  std::list< HepMC::GenParticle * > leptonList ;

  // Loop over HepMC events
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( leptonList )     , IsLepton        ) ;
  
  if ( leptonList.empty() ) {
    debug() <<  "No leptons in this event !" << endreq ;
    return false ;
  }  

  bool result = false ;

  std::list< HepMC::GenParticle *>::iterator iter ;
  double px, py, pz, pp, theta, pt ;
  for ( iter = leptonList.begin() ; iter != leptonList.end() ; ++iter ) {
    px = (*iter) -> momentum() . px() ;
    py = (*iter) -> momentum() . py() ;
    pz = (*iter) -> momentum() . pz() ;
    pp = sqrt( px*px + py*py + pz*pz ) ;
    pt = sqrt( px*px + py*py ) ;
    theta = acos( fabs( pz ) / pp ) ;
    if ( ( theta <= m_thetamax ) && ( pz >= 0. ) && ( pt >= m_pmin ) ) {
      debug() << "Event passed [theta = " << theta << "] "
              << "[pz = " << pz << "] "
              << "[pt = " << pt << "]" << endreq ;
      result = true ;
      break ;
    } else {
      debug() << "Particle failed cut [theta = " << theta << "] " 
              << "[pz = " << pz << "] "
              << "[pt = " << pt << "]" << endreq ;
    }
  }

  if ( ! m_useb ) return result ;
  if ( ! result ) return result ;

  // Find b
  std::list< HepMC::GenParticle * > bList ;

  // Loop over HepMC events
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( bList ) , Isb                     ) ;
  
  if ( bList.empty() ) {
    debug() <<  "No b in this event !" << endreq ;
    return false ;
  } else if ( bList.size() > 2 ) {
    debug() << "Too many b in event." << endreq ;
    return false ;
  }

  for ( iter = bList.begin() ; iter != bList.end() ; ++iter ) {
    pz = (*iter) -> momentum() . pz() ;
    if ( pz <= 0 ) {
      debug() << "Event rejected [pz = " << pz << "]" << endreq ;
      return false ;
    }
  }

  return result ;
}

//=============================================================================
// Function to find lepton (from all sources) in event
//=============================================================================
bool IsLepton( const HepMC::GenParticle * p ) 
{
  if ( ( 13 == abs( p -> pdg_id() ) ) || ( 11 == abs( p -> pdg_id() ) ) )
    return true ;
  
  return false ;
}

//=============================================================================
// Function to find b/bbar quarks in event coming from Z0
//=============================================================================
bool Isb( const HepMC::GenParticle * p ) 
{
  if ( 5 == abs( p -> pdg_id() ) ) {
    HepMC::GenVertex * thePV = p -> production_vertex() ;
    HepMC::GenVertex::particle_iterator iter ;
    for ( iter = thePV -> particles_begin( HepMC::parents ) ;
          iter != thePV -> particles_end( HepMC::parents ) ; ++iter ) {
      if ( 23 == abs( (*iter) -> pdg_id() ) ) return true ;
    }
  }
  return false ;
}
