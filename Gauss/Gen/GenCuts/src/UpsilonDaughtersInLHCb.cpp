// $Id: UpsilonDaughtersInLHCb.cpp,v 1.4 2010-09-19 09:03:47 robbep Exp $
// Include files 

// local
#include "UpsilonDaughtersInLHCb.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"
#include "MCInterfaces/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UpsilonDaughtersInLHCb
//
// 2009-02-26 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( UpsilonDaughtersInLHCb );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UpsilonDaughtersInLHCb::UpsilonDaughtersInLHCb( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decayTool( 0 ) , m_nUpsilonBeforeCut( 0 ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "DecayTool" ,       m_decayToolName = "EvtGenDecay") ;
  declareProperty( "SignalPID" ,       m_sigUpsilonPID = 553 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
UpsilonDaughtersInLHCb::~UpsilonDaughtersInLHCb( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode UpsilonDaughtersInLHCb::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;

  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_decayTool -> setSignal( m_sigUpsilonPID ) ;

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode UpsilonDaughtersInLHCb::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );

  info() << "*****************************************" << std::endl 
         << "************** Upsilon Counters *********" << std::endl 
         << "Number of generated signal Upsilon = " 
         << m_nUpsilonBeforeCut << endmsg ;

  return GaudiTool::finalize();

}

//=============================================================================
// Acceptance function
//=============================================================================
bool UpsilonDaughtersInLHCb::applyCut( ParticleVector & theParticleVector ,
                                       const HepMC::GenEvent * theEvent  ,
                                       const LHCb::GenCollision */* theHardInfo */ ) const {
  
  // First decay all particles heavier than the Upsilon
  m_decayTool -> disableFlip() ;
  HepMCUtils::ParticleSet particleSet ;  
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it )
    if ( LHCb::ParticleID( (*it) -> pdg_id() ).hasQuark( LHCb::ParticleID::bottom ) ) 
      particleSet.insert( *it ) ;
  
  for ( HepMCUtils::ParticleSet::iterator itHeavy = particleSet.begin() ; 
        itHeavy != particleSet.end() ; ++itHeavy ) 
    
    if ( ( LHCb::HepMCEvent::StableInProdGen == (*itHeavy) -> status() ) && 
         ( m_sigUpsilonPID != abs( (*itHeavy) -> pdg_id() ) ) ) {
      
      if ( m_decayTool -> isKnownToDecayTool( (*itHeavy) -> pdg_id() ) ) 
        m_decayTool -> generateDecayWithLimit( *itHeavy , m_sigUpsilonPID ) ;
    }
  
  // To see whether the Upsilon is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it )
    if ( abs( (*it) -> pdg_id() ) == m_sigUpsilonPID ) 
      if ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() ) 
        theParticleVector.push_back( *it ) ;
  std::sort( theParticleVector.begin() , theParticleVector.end() , 
	     HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) return false ;

  m_nUpsilonBeforeCut++ ;

  // To decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;
  theSignal = theParticleVector.front() ;  
  StatusCode sc = m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  if ( ! sc.isSuccess() ) {
    Error( "Skip event" ) ;
    return false ;
  }

  // To do the cut
  //--------------------------------------------------------------------  
  ParticleVector::iterator itp ;
  for ( itp = theParticleVector.begin() ; 
        itp != theParticleVector.end() ; ) {    
    if ( ! passCuts( *itp ) ) {
      itp = theParticleVector.erase( itp ) ;
    } else ++itp ;
  }
  
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool UpsilonDaughtersInLHCb::passCuts( const HepMC::GenParticle * theSignal ) 
  const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return false ;
  
  // check if pz of the Bc is positive
  if ( theSignal -> momentum().pz() < 0. ) return false ;

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
            << (*it) -> momentum().theta() / Gaudi::Units::mrad << " mrad." << endmsg ;
    
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
      if ( fabs( angle ) > fabs( m_neutralThetaMax ) ) 
        return false ;
      if ( fabs( angle ) < fabs( m_neutralThetaMin ) ) 
        return false ;
    } else {
      if ( fabs( angle ) > fabs( m_chargedThetaMax ) ) 
        return false ;
      if ( fabs( angle ) < fabs( m_chargedThetaMin ) ) 
        return false ;
    }
  }

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}
