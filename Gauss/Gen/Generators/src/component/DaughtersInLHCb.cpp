// $Id: DaughtersInLHCb.cpp,v 1.4 2007-02-22 13:30:24 robbep Exp $
// Include files 

// local
#include "DaughtersInLHCb.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "Kernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "Generators/HepMCUtils.h"
#include "Generators/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCb
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCb );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCb::DaughtersInLHCb( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * mrad ) ;
    declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * mrad ) ;
    declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * mrad ) ;
    declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * mrad ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCb::~DaughtersInLHCb( ) { ; }

//=============================================================================
// Acceptance function
//=============================================================================
bool DaughtersInLHCb::applyCut( ParticleVector & theParticleVector ,
                                const HepMC::GenEvent * /* theEvent */ ,
                                const LHCb::GenCollision * /* theHardInfo */ )
  const {
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
bool DaughtersInLHCb::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
  }  

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / mrad << " mrad." << endmsg ;
   
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

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}

//=============================================================================
// Boost the Tree theSignal to theVector frame and attach it to theMother
//=============================================================================
StatusCode DaughtersInLHCb::boostTree( HepMC::GenParticle * theSignal ,
                                       const HepMC::GenParticle * 
                                       theSignalAtRest ,
                                       const ROOT::Math::Boost& theBoost )
  const {
  if ( 0 == theSignalAtRest -> end_vertex() ) return StatusCode::SUCCESS ;
    
  if ( 0 != theSignal -> end_vertex() ) 
    return Error( "The particle has already a decay vertex !" ) ;

  if ( 0 == theSignalAtRest -> production_vertex() )
    return Error( "The particle has no production vertex !" ) ;
  
  // Displacement in original frame
  Gaudi::LorentzVector positionEnd , positionBegin ;
  positionEnd.SetXYZT( theSignalAtRest -> end_vertex() -> position() . x() ,
                       theSignalAtRest -> end_vertex() -> position() . y() ,
                       theSignalAtRest -> end_vertex() -> position() . z() ,
                       theSignalAtRest -> end_vertex() -> position() . t() *
                       CLHEP::c_light ) ;
  
  positionBegin.SetXYZT(theSignalAtRest->production_vertex()->position().x() ,
                        theSignalAtRest->production_vertex()->position().y() ,
                        theSignalAtRest->production_vertex()->position().z() ,
                        theSignalAtRest->production_vertex()->position().t() 
                        * CLHEP::c_light ) ;
  
  Gaudi::LorentzVector position = positionEnd - positionBegin ;
  
  // Displacement in new frame after boost.
  Gaudi::LorentzVector newPosition = theBoost( position ) ;

  // LHCb Units
  Gaudi::LorentzVector newP ;
  newP.SetXYZT( newPosition.X() , newPosition.Y() , newPosition.Z() ,
                newPosition.T() / CLHEP::c_light ) ;
  
  // Add original position
  Gaudi::LorentzVector 
    originalPosition( theSignal -> production_vertex() -> position() ) ;
  newP += originalPosition ;

  // Create new HepMC vertex after boost and add it to the current event    
  HepMC::GenVertex * newVertex = 
    new HepMC::GenVertex( HepLorentzVector(newP.X(), newP.Y() , newP.Z() , 
                                           newP.T()));
  
  theSignal -> parent_event() -> add_vertex( newVertex ) ;
  newVertex -> add_particle_in( theSignal ) ;

  HepMC::GenVertex * sVertex = theSignalAtRest -> end_vertex() ;
  
  HepMC::GenVertex::particles_out_const_iterator child ;
  
  for ( child  = sVertex -> particles_out_const_begin( ) ; 
        child != sVertex -> particles_out_const_end( ) ; ++child ) {
    // Boost all daughter particles and create a new HepMC particle
    // for each daughter
    Gaudi::LorentzVector momentum( (*child) -> momentum() ) ;
    Gaudi::LorentzVector newMomentum = theBoost( momentum ) ;
    int id                           = (*child) -> pdg_id() ;
    int status                       = (*child) -> status() ;
    
    HepMC::GenParticle * newPart =
      new HepMC::GenParticle( HepLorentzVector( newMomentum.Px() , 
                                                newMomentum.Py() , 
                                                newMomentum.Pz() , 
                                                newMomentum.E()   ) , 
                              id , status ) ;
    
    newVertex -> add_particle_out( newPart ) ;
    
    HepMC::GenParticle * theNewSignal             = newPart ;
    const HepMC::GenParticle * theNewSignalAtRest = (*child) ;
    
    // Recursive call to boostTree for each daughter
    boostTree( theNewSignal , theNewSignalAtRest , theBoost ) ;
  }

  return StatusCode::SUCCESS ;
}
