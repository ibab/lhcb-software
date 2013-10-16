// $Id: BcDaughtersInLHCbAndMassCut.cpp,v 1.4 2009-10-02 13:08:08 jhe Exp $
// Include files 

// local
#include "BcDaughtersInLHCbAndMassCut.h"

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
// Implementation file for class : BcDaughtersInLHCbAndMassCut
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BcDaughtersInLHCbAndMassCut );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BcDaughtersInLHCbAndMassCut::BcDaughtersInLHCbAndMassCut( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decayTool( 0 ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad );
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad );
  declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad );
  declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad );
  declareProperty( "DecayTool"       , m_decayToolName = "EvtGenDecay");

  declareProperty( "LeptonOneID"     , m_LeptonOneID = -13 );
  declareProperty( "LeptonTwoID"     , m_LeptonTwoID =  13 );

  declareProperty( "PreselDausPT"    , m_PreselDausPT = false );
  declareProperty( "DausPTMin"       , m_DausPTMin = 0.2 * Gaudi::Units::GeV );

  declareProperty( "PreselMinMass"   , m_PreselMinMass = false );
  declareProperty( "mmMinMass"       , m_mmMinMass = 4.5 * Gaudi::Units::GeV );

  declareProperty( "PreselMaxMass"   , m_PreselMaxMass = false );
  declareProperty( "mmMaxMass"       , m_mmMaxMass = 6. * Gaudi::Units::GeV );
  m_sigBcPID = 541 ;
}

//=============================================================================
// Destructor 
//=============================================================================
BcDaughtersInLHCbAndMassCut::~BcDaughtersInLHCbAndMassCut( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode BcDaughtersInLHCbAndMassCut::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;

  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_decayTool -> setSignal( m_sigBcPID ) ;

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BcDaughtersInLHCbAndMassCut::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );

  return GaudiTool::finalize(); 

}

//=============================================================================
// Acceptance function
//=============================================================================
bool BcDaughtersInLHCbAndMassCut::applyCut( ParticleVector & theParticleVector ,
                                  const HepMC::GenEvent * theEvent  ,
                                  const LHCb::GenCollision */* theHardInfo */ )
  const {
  
  // To see whether the B_c is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it )
    if ( abs( (*it) -> pdg_id() ) == m_sigBcPID ) 
      if ( ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() ) 
           && ( HepMCUtils::IsBAtProduction( *it ) ) )
        theParticleVector.push_back( *it ) ;
  std::sort( theParticleVector.begin() , theParticleVector.end() , 
             HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) {
    msg() << "No particles" << endmsg ;
    return false ;
  }
  

  // To decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;
  theSignal = theParticleVector.front() ;  
  msg() << "  SIZE = " << theParticleVector.size() << endmsg ;
  m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  
  // To do the cut
  //--------------------------------------------------------------------  
  ParticleVector::iterator itp ;
  for ( itp = theParticleVector.begin() ; 
        itp != theParticleVector.end() ; ) {    
    if ( ! passCuts( *itp ) ) {
      itp = theParticleVector.erase( itp ) ;
    } else ++itp ;
  }
  debug() << "+++++++++ out with : " << ! theParticleVector.empty() << endmsg ;
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool BcDaughtersInLHCbAndMassCut::passCuts( const HepMC::GenParticle * theSignal ) 
  const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) {
    // this seems counter intuitive, but it is correct.
    // In case there are more than one Bc in the event, only one has been decayed at the moment.
    // The others will be decayed freely later: they dont have an endvertex (nor daughters).
    return true ;
  }

  // check if pz of the Bc is positive
  if ( theSignal -> momentum().pz() < 0. ) return false ;  
  
  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;

  Particles muPlusList ;
  Particles muMinusList ;

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
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;

    if ( m_LeptonOneID == (*it) -> pdg_id() ) muPlusList.push_back( *it ) ;
    if ( m_LeptonTwoID == (*it) -> pdg_id() ) muMinusList.push_back( *it ) ;

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

  debug() << "Event passed geometry cut!!" << endmsg ;

  if ( muPlusList.empty() || muMinusList.empty() ){
    debug() << "No lepton1, lepton2 found in this event" << endmsg ;
    return false;
  } else {
    debug() << "lepton1: " << muPlusList.size() << endmsg;
    debug() << "lepton2: " << muMinusList.size() << endmsg;
  }
  
  //====================================================================
  // Check mass 
  //====================================================================
  HepMC::FourVector mmVect;
  
  for ( Particles::const_iterator itePlus = muPlusList.begin() ; itePlus != muPlusList.end() ;
        ++itePlus ) {

    // PT Cut 
    if(m_PreselDausPT) {
      if( (*itePlus)->momentum().perp() < m_DausPTMin ) {
          debug() << "Event not pass minimal PT daughter cut "<< m_DausPTMin << ", with PT: "
                  << (*itePlus)->momentum().perp() << endmsg ;
          continue;
        }
    }
    
    for ( Particles::const_iterator iteMinus = muMinusList.begin() ; iteMinus != muMinusList.end() ;
          ++iteMinus ) {
      
      // PT Cut 
      if(m_PreselDausPT) {
        if( (*iteMinus)->momentum().perp() < m_DausPTMin ) {
          debug() << "Event not pass minimal PT daughter cut " << m_DausPTMin << ", with PT: "
                  << (*iteMinus)->momentum().perp() << endmsg ;
          continue;
        }
      }

      if( m_PreselMinMass || m_PreselMaxMass ) {
        
        mmVect.setPx( (*itePlus)->momentum().px() + (*iteMinus)->momentum().px() );
        mmVect.setPy( (*itePlus)->momentum().py() + (*iteMinus)->momentum().py() );
        mmVect.setPz( (*itePlus)->momentum().pz() + (*iteMinus)->momentum().pz() );
        mmVect.setE(  (*itePlus)->momentum().e()  + (*iteMinus)->momentum().e() );
      
        // mmMass > mmMaxMass 
        if( m_PreselMaxMass && mmVect.m() > m_mmMaxMass ) {
          debug() << "Event not pass max Mass cut " << m_mmMaxMass << ", with mass:" << mmVect.m() << endmsg ;
          continue;
        }

        // mmMass < mmMinMass 
        if( m_PreselMinMass && mmVect.m() < m_mmMinMass ) {
          debug() << "Event not pass min Mass cut " << m_mmMinMass << ", with mass:" << mmVect.m() << endmsg ;
          continue;
        }
        debug() << "!!!!!!!!!!!!!!!!!!!!!!Event pass: mass:" << mmVect.m() << endmsg ;
        return true ;
      }
      debug() << "=====================Event pass: mass:" << mmVect.m() << endmsg ;
      return true ;
    }
  }
  debug() << "Final false" << endmsg ;
  return false ;
}
