// $Id: TTbarPythiaAlg.cpp,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
// Include files 
// local
#include "TTbarPythiaAlg.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "HepMC/IO_HEPEVT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TTbarPythiaAlg
//
// 2004-12-16 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TTbarPythiaAlg>          s_factory ;
const        IAlgFactory& TTbarPythiaAlgFactory = s_factory ; 

// Forward declarations of selection functions
bool IsTop    ( const HepMC::GenParticle * p ) ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TTbarPythiaAlg::TTbarPythiaAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : PythiaAlg ( name , pSvcLocator ) { } ;

//=============================================================================
// Destructor
//=============================================================================
TTbarPythiaAlg::~TTbarPythiaAlg() {};

//=============================================================================
// Some initialization
//=============================================================================
StatusCode TTbarPythiaAlg::genInitialize() {
  
  m_bId     = m_ppSvc -> find( "b"  )     ;
  m_bbarId  = m_ppSvc -> find( "anti-b" ) ;
  m_tId     = m_ppSvc -> find( "t"  )     ;
  m_tbarId  = m_ppSvc -> find( "anti-t" ) ;
  
  return PythiaAlg::genInitialize() ;                       
} 

//=============================================================================
// Convert HEPEVT into HepMC format (simple conversion )
//=============================================================================
StatusCode TTbarPythiaAlg::fillEvt( HepMC::GenEvent * evt ) {
  HepMC::IO_HEPEVT hepio ;
  hepio.fill_next_event( evt ) ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Selection function
//=============================================================================
bool TTbarPythiaAlg::selectEvt( HepMC::GenEvent * evt ) {
  debug() << "==> Analyze event" << endreq ;

  // Find ts
  std::list< HepMC::GenParticle * > topList ;

  // Loop over HepMC events
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( topList )     , IsTop     ) ;
  
  if ( 2 != topList.size() ) {
    warning() <<  "No top in this event !" << endreq ;
    return false ;
  }
  
  // Get the 2 b quarks
  std::list< HepMC::GenParticle * >::iterator iterTop = topList.begin() ;
  HepMC::GenParticle * thet1     = *( iterTop ) ;
  HepMC::GenParticle * thet2     = *( ++iterTop ) ;

  HepMC::GenParticle * theb1( 0 ) , * theb2( 0 )  ;

  HepMC::GenVertex::particle_iterator iter ;
  
  for ( iter  = thet1 -> end_vertex() -> particles_begin( HepMC::children ) ;
        iter != thet1 -> end_vertex() -> particles_end( HepMC::children ) ; 
        ++iter ) {
    if ( ( (*iter) -> pdg_id() == m_bId -> jetsetID() ) ||
         ( (*iter) -> pdg_id() == m_bbarId -> jetsetID() ) ) { 
      theb1 = (*iter) ;
      break ;
    }
  }  

  for ( iter  = thet2 -> end_vertex() -> particles_begin( HepMC::children ) ;
        iter != thet2 -> end_vertex() -> particles_end( HepMC::children ) ; 
        ++iter ) {
    if ( ( (*iter) -> pdg_id() == m_bId -> jetsetID() ) ||
         ( (*iter) -> pdg_id() == m_bbarId -> jetsetID() ) ) { 
      theb2 = (*iter) ;
      break ;
    }
  }  
  
  if ( ( 0 == theb1 ) || ( 0 == theb2 ) ) {
    Exception( "Not enough b in the event in this event !" ) ;
    return false ;
  }

  double pxb1 = theb1 -> momentum() . px() ;
  double pyb1 = theb1 -> momentum() . py() ;
  double pzb1 = theb1 -> momentum() . pz() ;
  double ppb1 = sqrt( pxb1*pxb1 + pyb1*pyb1 + pzb1*pzb1 ) ;
  double thetab1 = acos( fabs( pzb1 ) / ppb1 ) ;
    
  double pxb2 = theb2 -> momentum() . px() ;
  double pyb2 = theb2 -> momentum() . py() ;
  double pzb2 = theb2 -> momentum() . pz() ;
  double ppb2 = sqrt( pxb2*pxb2 + pyb2*pyb2 + pzb2*pzb2 ) ;
  double thetab2 = acos( fabs( pzb2 ) / ppb2 ) ;
    
  if ( ( ( thetab1 <= m_thetamax ) && ( pzb1 >= 0. ) ) &&
       ( ( thetab2 <= m_thetamax ) && ( pzb2 >= 0. ) ) ) {
    debug() << "Event passed [thetab1 = " << thetab1 
            << "] [thetab2 = " << thetab2 << "] " 
            << "[pzb1 = " << pzb1 << "] [pzb2 = " << pzb2 << "]" << endreq ;
    return true ;
  } else {
    debug() << "Event rejected [thetab1 = " << thetab1 
            << "] [thetab2 = " << thetab2 << "] " 
            << "[pzb1 = " << pzb1 << "] [pzb2 = " << pzb2 << "]" << endreq ;
    return false ;
  }
  
  return false ;
}

//=============================================================================
// Function to find top ( -> W b)  particle in event
//=============================================================================
bool IsTop( const HepMC::GenParticle * p ) 
{
  if ( 6 != abs( p -> pdg_id() ) ) return false ;

  HepMC::GenVertex * theEV = p -> end_vertex() ;
  if ( 0 == theEV ) return false ;

  HepMC::GenVertex::particle_iterator iter ;
  
  for ( iter  = theEV -> particles_begin( HepMC::children ) ;
        iter != theEV -> particles_end  ( HepMC::children ) ; ++iter ) {
    if ( 5 == abs( (*iter) -> pdg_id() ) ) return true ;
  }
  
  return false ;
}
