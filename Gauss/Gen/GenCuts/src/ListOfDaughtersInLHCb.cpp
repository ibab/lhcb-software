// $Id: ListOfDaughtersInLHCb.cpp,v 1.7 2008-07-09 14:36:03 robbep Exp $
// Include files 

// local
#include "ListOfDaughtersInLHCb.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h" 

// from Kernel
#include "Kernel/ParticleID.h"
#include "Kernel/IParticlePropertySvc.h" 
#include "Kernel/ParticleProperty.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ListOfDaughtersInLHCb
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( ListOfDaughtersInLHCb );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ListOfDaughtersInLHCb::ListOfDaughtersInLHCb( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin"  , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
    declareProperty( "ChargedThetaMax"  , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMin"  , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMax"  , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "DaughtersPIDList" , m_pidVector ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
ListOfDaughtersInLHCb::~ListOfDaughtersInLHCb( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode ListOfDaughtersInLHCb::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;
  
  if ( m_pidVector.empty() )
    return Error( "No PID given to cut tool" ) ;
  
  // transform vector into set
  for ( std::vector< int >::iterator it = m_pidVector.begin() ;
        it != m_pidVector.end() ; ++it ) m_daughtersPIDList.insert( *it ) ;
  
  LHCb::IParticlePropertySvc * ppSvc = 
    svc< LHCb::IParticlePropertySvc > ( "LHCb::ParticlePropertySvc" ) ;
  
  info() << "Cutting at generator level on stable daughters of " ;
  PIDs::const_iterator it ;
  
  for ( it = m_daughtersPIDList.begin() ; it != m_daughtersPIDList.end() ; 
        ++it ) {
    const LHCb::ParticleProperty* prop = ppSvc->find( LHCb::ParticleID( *it ));
    info() << prop -> particle() << " " ;
  }
  info() << endmsg ;
  release( ppSvc ) ;
  
  return sc ;
}


//=============================================================================
// Acceptance function
//=============================================================================
bool ListOfDaughtersInLHCb::applyCut( ParticleVector & theParticleVector ,
                                      const HepMC::GenEvent * /* theEvent */ ,
                                      const LHCb::GenCollision * 
                                      /* theHardInfo */ ) const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {
    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }
  
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool ListOfDaughtersInLHCb::passCuts( const HepMC::GenParticle * theSignal ) 
  const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles selectedDaughters ;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( std::binary_search( m_daughtersPIDList.begin() , 
                             m_daughtersPIDList.end() , 
                             (*iter) -> pdg_id() ) ) 
      selectedDaughters.push_back( *iter ) ;
  }

  // if no found, try CP conjugates
  if ( selectedDaughters.empty() ) {
    for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
          iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
      if ( std::binary_search( m_daughtersPIDList.begin() , 
                               m_daughtersPIDList.end() , 
                               -(*iter) -> pdg_id() ) ) 
        selectedDaughters.push_back( *iter ) ;
    }
  }  
  
  debug() << "New event" << endmsg ;  

  if ( selectedDaughters.empty() )
    Exception( "Signal has no daughters of specified PID !" ) ;

  if ( selectedDaughters.size() != m_daughtersPIDList.size() ) 
    Exception( "Not enough daughters !" ) ;
  
  for ( Particles::const_iterator itD = selectedDaughters.begin() ; 
        itD != selectedDaughters.end() ; ++itD ) {
    
    Particles stables ;
    EV = (*itD) -> end_vertex() ;
    
    if ( 0 == EV ) stables.push_back( *itD ) ;
    else {
      for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
            iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
        if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
      }  
    }
    
    double angle( 0. ) ;
    double firstpz = stables.front() -> momentum().pz() ;
    
    debug() << "New selected daughter " << (*itD) -> pdg_id() << endmsg ;
    
    for ( Particles::const_iterator it = stables.begin() ;
          it != stables.end() ; ++it ) {

      debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
              << (*it) -> momentum().theta() / Gaudi::Units::mrad 
              << " mrad." << endmsg ;
      
      // Remove neutrinos
      if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
           ( 14 == abs( (*it) -> pdg_id() ) ) || 
           ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
      
      // Don't use daughters of Lambda and KS:
      HepMC::GenParticle * theParent ;
      theParent = 
        *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
      if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
      if ( 310 == theParent -> pdg_id() ) continue ;
      
      // Consider only gammas from pi0 and eta
      if ( 22 == (*it) -> pdg_id() ) {
        if ( ( 111 != theParent -> pdg_id() ) &&
             ( 221 != theParent -> pdg_id() ) ) continue ;
      }
      
      // All particles in same direction
      if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;
      
      angle = (*it) -> momentum().theta() ;
      
      LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
      if ( 0 == pid.threeCharge() ) {
        if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
          return false ;
        if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
          return false ;
      } else {
        if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
          return false ;
        if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) )
          return false ;
      }
    }
  }
  
  debug() << "Event passed !" << endmsg ;

  return true ;
}
