// $Id: HiggsPythiaAlg.cpp,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $

// This class
#include "HiggsPythiaAlg.h"

// Include files 
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "HepMC/IO_HEPEVT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HiggsPythiaAlg
//
// 2004-12-16 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<HiggsPythiaAlg>          s_factory ;
const        IAlgFactory& HiggsPythiaAlgFactory = s_factory ; 

// Forward declarations of selection functions
bool IsHiggs( const HepMC::GenParticle * p ) ;
bool IsW    ( const HepMC::GenParticle * p ) ;
bool IsZ0   ( const HepMC::GenParticle * p ) ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HiggsPythiaAlg::HiggsPythiaAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : PythiaAlg ( name , pSvcLocator ) ,
    m_useLepton ( true ) ,
    m_usebb     ( false ) ,
    m_HiggsId( 0 ) ,
    m_WpId( 0 ) ,
    m_WmId( 0 ) ,
    m_bId( 0 ) ,
    m_bbarId( 0 ) ,
    m_epId( 0 ) ,
    m_emId( 0 ) ,
    m_mupId( 0 ) ,
    m_mumId( 0 ) ,
    m_Z0Id( 0 ) {
  declareProperty( "UseLepton" , m_useLepton ) ;
  declareProperty( "Usebb"     , m_usebb     ) ;
}

//=============================================================================
// Destructor
//=============================================================================
HiggsPythiaAlg::~HiggsPythiaAlg() {};

//=============================================================================
// Some initialization
//=============================================================================
StatusCode HiggsPythiaAlg::genInitialize() {
  m_HiggsId = m_ppSvc -> find( "H_10" )   ;
  m_WpId    = m_ppSvc -> find( "W+" )     ;
  m_WmId    = m_ppSvc -> find( "W-" )     ;
  m_bId     = m_ppSvc -> find( "b"  )     ;
  m_bbarId  = m_ppSvc -> find( "anti-b" ) ;
  m_epId    = m_ppSvc -> find( "e+" )     ;
  m_emId    = m_ppSvc -> find( "e-" )     ;
  m_mupId   = m_ppSvc -> find( "mu+" )    ;
  m_mumId   = m_ppSvc -> find( "mu-" )    ;
  m_Z0Id    = m_ppSvc -> find( "Z0"  )    ;

  if ( m_useLepton ) info() << "Require lepton in acceptance" << endreq ;
  else info() << "Do not require lepton in acceptance" << endreq ;
  if ( m_usebb )     info() << "Require both b quarks  in acceptance" 
                            << endreq ;
  else info() << "Do not require any b quark in acceptance" << endreq ;
  
  return PythiaAlg::genInitialize() ;                       
} 

//=============================================================================
// Convert HEPEVT into HepMC format (simple conversion )
//=============================================================================
StatusCode HiggsPythiaAlg::fillEvt( HepMC::GenEvent * evt ) {
  HepMC::IO_HEPEVT hepio ;
  hepio.fill_next_event( evt ) ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Selection function
//=============================================================================
bool HiggsPythiaAlg::selectEvt( HepMC::GenEvent * evt ) {
  debug() << "==> Analyze event" << endreq ;

  // Find Higgs
  std::list< HepMC::GenParticle * > higgsList , WList , Z0List ;

  // Loop over HepMC events
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( higgsList ) , IsHiggs ) ;
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( WList )     , IsW     ) ;
  HepMC::copy_if ( evt->particles_begin() , evt->particles_end()   ,
                   back_inserter( Z0List )    , IsZ0    ) ;

  
  if ( higgsList.empty( ) ) {
    Exception( "No Higgs particle in this event !" ) ;
    return false ;
  }  
  
  if ( WList.empty() && Z0List.empty() ) {
    Exception( "No W in this event !" ) ;
    return false ;
  }

  HepMC::GenParticle * theHiggs = *(higgsList.begin()) ;

  // The higgs mother
  HepMC::GenParticle * theHiggsMother( 0 ) ;
  HepMC::GenVertex   * theHiggsPV = theHiggs -> production_vertex() ;
  HepMC::GenVertex::particle_iterator iter ;
  for ( iter  = theHiggsPV -> particles_begin( HepMC::parents ) ;
        iter != theHiggsPV -> particles_end( HepMC::parents ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == m_HiggsId -> pdgID() ) {
      theHiggsMother = ( *iter ) ;
      break ;
    }
  }
  if ( 0 == theHiggsMother ) {
    Exception( "No Higgs mother in this event !" ) ;
    return false ;
  }
  
  // Get the 2 b quarks
  HepMC::GenParticle * thebHard( 0 )  , * thebbarHard( 0 ) ;
  HepMC::GenVertex   * theHiggsEV = theHiggsMother -> end_vertex() ;
  for ( iter  = theHiggsEV -> particles_begin( HepMC::children ) ;
        iter != theHiggsEV -> particles_end( HepMC::children ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == m_bId -> pdgID() ) thebHard = (*iter) ;
    else if ( (*iter) -> pdg_id() == m_bbarId -> pdgID() ) 
      thebbarHard = (*iter) ;
  }

  if ( ( 0 == thebHard ) || ( 0 == thebbarHard ) ) {
    Exception( "No Hard b in this event !" ) ;
    return false ;
  }
  
  // Get the 2 "real" b quarks
  HepMC::GenParticle * thebFrag( 0 ) , * thebbarFrag( 0 ) ;
  HepMC::GenVertex   * thebEV = thebHard -> end_vertex() ;
  HepMC::GenVertex   * thebbarEV = thebbarHard -> end_vertex() ;
  for ( iter  = thebEV -> particles_begin( HepMC::children ) ;
        iter != thebEV -> particles_end( HepMC::children ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == m_bId -> pdgID() ) { 
      thebFrag = (*iter) ;
      break ;
    }
  }

  for ( iter  = thebbarEV -> particles_begin( HepMC::children ) ;
        iter != thebbarEV -> particles_end( HepMC::children ) ; ++iter) {
    if ( (*iter) -> pdg_id() == m_bbarId -> pdgID() ) { 
      thebbarFrag = (*iter) ;
      break ;
    }
  }  

  if ( ( 0 == thebbarFrag ) || ( 0 == thebFrag ) ) {
    Exception( "No real b quark in this event !" ) ;
    return false ;
  }

  // In case of an event with a W :
  if ( ! WList.empty() ) {
    HepMC::GenParticle * theW     = *(WList.begin()    ) ;
    HepMC::GenParticle * theWMother( 0 ) ;
    HepMC::GenVertex   * theWPV = theW -> production_vertex() ;
    for ( iter  = theWPV -> particles_begin( HepMC::parents ) ;
          iter != theWPV -> particles_end( HepMC::parents ) ; ++iter ) {
      if ( ( (*iter) -> pdg_id() == m_WpId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_WmId -> pdgID() ) ) {
        theWMother = ( *iter ) ;
        break ;
      }
    }
    
    if ( 0 == theWMother ) {
      Exception( "No W mother in this event !" ) ;
      return false ;
    }
    
    // Get the lepton
    HepMC::GenParticle * theLepton( 0 ) ;
    HepMC::GenVertex   * theWEV = theWMother -> end_vertex() ;
    for ( iter  = theWEV -> particles_begin( HepMC::children ) ;
          iter != theWEV -> particles_end( HepMC::children ) ; ++iter ) {
      if ( ( (*iter) -> pdg_id() == m_epId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_emId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_mupId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_mumId -> pdgID() ) ) { 
        theLepton = (*iter) ;
        break ;
      }
    }  
    
    if ( 0 == theLepton ) {
      Exception( "No lepton in this event !" ) ;
      return false ;
    }

    double pxb = thebFrag -> momentum() . px() ;
    double pyb = thebFrag -> momentum() . py() ;
    double pzb = thebFrag -> momentum() . pz() ;
    double ppb = sqrt( pxb*pxb + pyb*pyb + pzb*pzb ) ;
    double thetab = acos( fabs( pzb ) / ppb ) ;
    
    double pxbbar = thebbarFrag -> momentum() . px() ;
    double pybbar = thebbarFrag -> momentum() . py() ;
    double pzbbar = thebbarFrag -> momentum() . pz() ;
    double ppbbar = sqrt( pxbbar*pxbbar + pybbar*pybbar + pzbbar*pzbbar ) ;
    double thetabbar = acos( fabs( pzbbar ) / ppbbar ) ;
    
    double pxl = theLepton -> momentum() . px() ;
    double pyl = theLepton -> momentum() . py() ;
    double pzl = theLepton -> momentum() . pz() ;
    double ppl = sqrt( pxl*pxl + pyl*pyl + pzl*pzl ) ;
    double thetal = acos( fabs( pzl ) / ppl ) ;
    
    if ( ( ( ! m_usebb ) || 
           ( ( ( thetab <= m_thetamax ) && ( pzb >= 0. ) ) &&
             ( ( thetabbar <= m_thetamax ) && ( pzbbar >= 0. ) ) ) ) &&
         ( ( ! m_useLepton ) || ( ( thetal <= m_thetamax ) 
                                  && ( pzl >= 0. ) ) ) )
      {
        debug() << "W Event passed [thetab = " << thetab 
                << "] [thetabbar = " << thetabbar << "] " 
                << "[pzb = " << pzb << "] [pzbbar = " << pzbbar
                << "] [usebb = " << m_usebb 
                << "] [useLepton = " << m_useLepton 
                << "] [thetal = " << thetal << "] [pzl = "
                << pzl << "]" << endreq ;
        return true ;
      }
    else {
      debug() << "W Event rejected [thetab = " << thetab 
              << "] [thetabbar = " << thetabbar << "] " 
              << "[pzb = " << pzb << "] [pzbbar = " << pzbbar
              << "] [usebb = " << m_usebb
              << "] [useLepton = " << m_useLepton 
              << "] [thetal = " << thetal << "] [pzl = "
              << pzl << "]" << endreq ;
      return false ;
    }
  }
  else if ( ! Z0List.empty() ) {
    HepMC::GenParticle * theZ0    = *(Z0List.begin()   ) ;
    HepMC::GenParticle * theZ0Mother( 0 ) ;
    HepMC::GenVertex   * theZ0PV = theZ0 -> production_vertex() ;
    for ( iter  = theZ0PV -> particles_begin( HepMC::parents ) ;
          iter != theZ0PV -> particles_end( HepMC::parents ) ; ++iter ) {
      if ( (*iter) -> pdg_id() == m_Z0Id -> pdgID() ) {
        theZ0Mother = ( *iter ) ;
        break ;
      }
    }
    
    if ( 0 == theZ0Mother ) {
      Exception( "No Z0 mother in this event !" ) ;
      return false ;
    }
    
    // Get the leptons
    HepMC::GenParticle * theLepton1( 0 ) , * theLepton2( 0 ) ;
    HepMC::GenVertex   * theZ0EV = theZ0Mother -> end_vertex() ;
    for ( iter  = theZ0EV -> particles_begin( HepMC::children ) ;
          iter != theZ0EV -> particles_end( HepMC::children ) ; ++iter ) {
      if ( ( (*iter) -> pdg_id() == m_epId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_emId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_mupId -> pdgID() ) ||
           ( (*iter) -> pdg_id() == m_mumId -> pdgID() ) ) { 
        if ( 0 == theLepton1 ) theLepton1 = (*iter) ;
        else {
          theLepton2 = (*iter) ;
          break ;
        }
      }
    }  
    
    if ( ( 0 == theLepton1 ) || ( 0 == theLepton2 ) ) {
      Exception( "No leptons in this event !" ) ;
      return false ;
    }
    
    double pxb = thebFrag -> momentum() . px() ;
    double pyb = thebFrag -> momentum() . py() ;
    double pzb = thebFrag -> momentum() . pz() ;
    double ppb = sqrt( pxb*pxb + pyb*pyb + pzb*pzb ) ;
    double thetab = acos( fabs( pzb ) / ppb ) ;
    
    double pxbbar = thebbarFrag -> momentum() . px() ;
    double pybbar = thebbarFrag -> momentum() . py() ;
    double pzbbar = thebbarFrag -> momentum() . pz() ;
    double ppbbar = sqrt( pxbbar*pxbbar + pybbar*pybbar + pzbbar*pzbbar ) ;
    double thetabbar = acos( fabs( pzbbar ) / ppbbar ) ;
    
    double pxl1 = theLepton1 -> momentum() . px() ;
    double pyl1 = theLepton1 -> momentum() . py() ;
    double pzl1 = theLepton1 -> momentum() . pz() ;
    double ppl1 = sqrt( pxl1*pxl1 + pyl1*pyl1 + pzl1*pzl1 ) ;
    double thetal1 = acos( fabs( pzl1 ) / ppl1 ) ;

    double pxl2 = theLepton2 -> momentum() . px() ;
    double pyl2 = theLepton2 -> momentum() . py() ;
    double pzl2 = theLepton2 -> momentum() . pz() ;
    double ppl2 = sqrt( pxl2*pxl2 + pyl2*pyl2 + pzl2*pzl2 ) ;
    double thetal2 = acos( fabs( pzl2 ) / ppl2 ) ;
    
    if ( ( ( ! m_usebb ) ||
           ( ( ( thetab <= m_thetamax ) && ( pzb >= 0. ) ) &&
             ( ( thetabbar <= m_thetamax ) && ( pzbbar >= 0. ) ) ) ) &&
         ( ( ! m_useLepton ) || 
           ( ( thetal1 <= m_thetamax ) && ( pzl1 >= 0. ) ) ||
           ( ( thetal2 <= m_thetamax ) && ( pzl2 >= 0. ) ) ) ) {
      debug() << "Z0 Event passed [thetab = " << thetab 
              << "] [thetabbar = " << thetabbar << "] " 
              << "[pzb = " << pzb << "] [pzbbar = " << pzbbar
              << "] [usebb = " << m_usebb
              << "] [useLepton = " << m_useLepton 
              << "] [thetal1 = " << thetal1 << "] [pzl1 = "
              << pzl1 << "] [thetal2 = " << thetal2 << "] [pzl2 = "
              << pzl2 << "]" << endreq ;
      return true ;
    }
    else {
      debug() << "Z0 Event rejected [thetab = " << thetab 
              << "] [thetabbar = " << thetabbar << "] " 
              << "[pzb = " << pzb << "] [pzbbar = " << pzbbar
              << "] [usebb = " << m_usebb
              << "] [useLepton = " << m_useLepton 
              << "] [thetal1 = " << thetal1 << "] [pzl1 = "
              << pzl1 << "] [thetal2 = " << thetal2 << "] [pzl2 = "
              << pzl2 << "]" << endreq ;
      return false ;
    }
  }
  
  return false ;
}

//=============================================================================
// Function to find Higgs particle in event
//=============================================================================
bool IsHiggs( const HepMC::GenParticle * p ) 
{
  if ( ( 25 == p -> pdg_id() ) && 
       ( 0 == p -> end_vertex() ) ) return true ;
  return false ;
}

//=============================================================================
// Function to find W+/- particle in event
//=============================================================================
bool IsW( const HepMC::GenParticle * p ) 
{
  if ( ( ( 24 == p -> pdg_id() ) || 
         ( -24 == p -> pdg_id() ) ) && 
       ( 0 == p -> end_vertex() ) ) return true ;
  return false ;
}

//=============================================================================
// Function to find W+/- particle in event
//=============================================================================
bool IsZ0( const HepMC::GenParticle * p ) 
{
  if ( ( 23 == p -> pdg_id() ) && 
       ( 0 == p -> end_vertex() ) ) return true ;
  return false ;
}
