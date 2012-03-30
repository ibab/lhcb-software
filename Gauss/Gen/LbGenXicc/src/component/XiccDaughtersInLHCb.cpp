// Include files 

// local
#include "XiccDaughtersInLHCb.h"

#include "Generators/ExternalGenerator.h"

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
// Implementation file for class : XiccDaughtersInLHCb
//
// 2011-04-22 : F. Zhang     
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( XiccDaughtersInLHCb );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XiccDaughtersInLHCb::XiccDaughtersInLHCb( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decayTool( 0 ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "DecayTool" ,       m_decayToolName = "EvtGenDecay") ;
  declareProperty( "BaryonState"    , m_BaryonState="Xi_cc+"); // double heavy baryon to be looked for

}

//=============================================================================
// Destructor 
//=============================================================================
XiccDaughtersInLHCb::~XiccDaughtersInLHCb( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode XiccDaughtersInLHCb::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;


  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_sigXiccPID = m_mapBaryonPID[m_BaryonState];
  m_decayTool -> setSignal( m_sigXiccPID  ) ;

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode XiccDaughtersInLHCb::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );

  return GaudiTool::finalize(); 

}

//=============================================================================
// Acceptance function
//=============================================================================
bool XiccDaughtersInLHCb::applyCut( ParticleVector & theParticleVector ,
                                  const HepMC::GenEvent * theEvent  ,
                                  const LHCb::GenCollision */* theHardInfo */ )
  const {
  
  // To see whether the Xi_cc is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; it != theEvent -> particles_end() ; ++it )
    if ( abs( (*it) -> pdg_id() ) == m_sigXiccPID ) 
      // GG : particle must not have decayed (no intermediate states)
      if ( ( LHCb::HepMCEvent::StableInProdGen == (*it) -> status() ) && ( HepMCUtils::IsBAtProduction( *it ) ) ){
        theParticleVector.push_back( *it ) ;
        verbose()<< "found a signal particle "<< **it << endmsg;
  // Added by F. Zhang 27-04-2011 
  //always() << "               " << endmsg;   
  //always() << "@@@@ XICC4412 FOURMOM = :" << (*it)-> momentum().px() << " MeV/c " << endmsg;
  //always() << "@@@@ XICC4412 FOURMOM = :" << (*it)-> momentum().py() << " MeV/c " << endmsg;
  //always() << "@@@@ XICC4412 FOURMOM = :" << (*it)-> momentum().pz() << " MeV/c " << endmsg;
  //always() << "@@@@ XICC4412 FOURMOM = :" << (*it)-> momentum().e() << " MeV " << endmsg;
  //always() << "               " << endmsg;   
      }
  //std::sort( theParticleVector.begin() , theParticleVector.end() , HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) {
    warning() << "No double heavy baryon "<<m_BaryonState << " found among the GenXicc products!" << endmsg; 
    return false ;    
  }
  else if(theParticleVector.size() > 1) {
    warning() << "There are "<<theParticleVector.size() << " " <<m_BaryonState<<" in the GenXicc output!!!"<<endmsg;
  }
  // To decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;

  theSignal = theParticleVector.back() ; // in case of multiple signals (should not happen), let's decay only the last one 
  if ( 0 != theSignal -> end_vertex() ) {
    error()<<"MyXicc already decayed ??" <<endmsg;
    return false;
  }
  m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  
  return passCuts(theSignal);
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool XiccDaughtersInLHCb::passCuts( const HepMC::GenParticle * theSignal ) 
  const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) {
    warning()<<"SIGNAL HAS NO DECAY VERTEX!!!"<<endmsg;
    return true; 
  }
  
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
    bool isout=false;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( angle ) > fabs( m_neutralThetaMax ) ) 
        isout=true ;
      if ( fabs( angle ) < fabs( m_neutralThetaMin ) ) 
        isout=true ;
    } else {
      if ( fabs( angle ) > fabs( m_chargedThetaMax ) ) 
        isout=true ;
      if ( fabs( angle ) < fabs( m_chargedThetaMin ) ) 
        isout=true ;
    }
    if(isout) {
      debug() << "particle " << pid << " is out of LHCbacceptance (angle="<<
        angle/Gaudi::Units::mrad << " mrad)"<<endmsg;
      return false ;
    }
  }

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}
