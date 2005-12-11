// $Id: SignalForcedFragmentation.cpp,v 1.3 2005-12-11 23:22:30 robbep Exp $
// Include files

// local
#include "SignalForcedFragmentation.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from HepMC
#include "HepMC/GenEvent.h"

// from Generators
#include "Generators/IDecayTool.h"
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalForcedFragmentation
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SignalForcedFragmentation>          s_factory ;
const        IToolFactory& SignalForcedFragmentationFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalForcedFragmentation::SignalForcedFragmentation(
  const std::string& type, const std::string& name, const IInterface* parent )
  : Signal( type, name , parent ) { 
  }

//=============================================================================
// Destructor
//=============================================================================
SignalForcedFragmentation::~SignalForcedFragmentation( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode SignalForcedFragmentation::initialize( ) {
  StatusCode sc = Signal::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  return sc ;
}

//=============================================================================
// Generate set of events with repeated hadronization
//=============================================================================
bool SignalForcedFragmentation::generate( const unsigned int nPileUp ,
                                          EventVector & theEventVector ,
                                          HardVector & theHardVector ) {
  StatusCode sc ;

  // first decay signal particle
  HepMC::GenEvent * theSignalHepMCEvent = new HepMC::GenEvent( ) ;
  HepMC::GenParticle * theSignalAtRest = new HepMC::GenParticle( ) ;
  theSignalAtRest -> 
    set_momentum( HepLorentzVector( 0., 0., 0., m_signalMass ) ) ;

  // Create an origin vertex at (0,0,0,0) for the signal particle at rest
  HepLorentzVector theOrigin( 0., 0., 0., 0. ) ;
  HepMC::GenVertex * theVertex =  new HepMC::GenVertex( theOrigin ) ;
  theSignalHepMCEvent -> add_vertex( theVertex ) ;
  theVertex -> add_particle_out( theSignalAtRest ) ;
  
  bool flip ;
  int theSignalPID = *m_pids.begin() ;

  if ( m_cpMixture ) {
    // decide which flavour to generate : 
    // if flavour < 0.5, b flavour
    // if flavour >= 0.5, bbar flavour
    double flavour = m_flatGenerator() ;

    m_decayTool -> enableFlip() ;
    
    if ( flavour < 0.5 ) 
      theSignalAtRest -> set_pdg_id( +abs( theSignalPID ) ) ;
    else
      theSignalAtRest -> set_pdg_id( -abs( theSignalPID ) ) ;
  } else {
    // generate only one flavour
    m_decayTool -> disableFlip() ;
    theSignalAtRest -> set_pdg_id( theSignalPID ) ;
  }

  m_decayTool -> generateSignalDecay( theSignalAtRest , flip ) ;

  bool result = false ;  

  PIDs signalPid ;
  signalPid.insert( theSignalAtRest -> pdg_id() ) ;

  sc = m_productionTool -> 
    setupForcedFragmentation( theSignalAtRest -> pdg_id() ) ;
  if ( sc.isFailure() ) error() << "Could not force fragmentation" << endmsg ;

  HardInfo * theHardInfo( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  // Then generate set of pile-up events    
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEventVector , theHardVector , theGenEvent ,
                        theHardInfo ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      ParticleVector theParticleList ;
      if ( checkPresence( signalPid , theGenEvent , theParticleList ) ) {
        m_nEventsBeforeCut++ ;

        updateCounters( theParticleList , m_nParticlesBeforeCut , 
                        m_nAntiParticlesBeforeCut , false ) ;

        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                           theHardInfo ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {
          m_nEventsAfterCut++ ;

          updateCounters( theParticleList , m_nParticlesAfterCut , 
                          m_nAntiParticlesAfterCut , true ) ;

          HepMC::GenParticle * theSignal = chooseAndRevert( theParticleList ) ;

          // Give signal status
          theSignal -> set_status( 889 ) ;

          // Now boost signal at rest to frame of signal produced by 
          // production generator
          const Hep3Vector theVector = theSignal -> momentum().boostVector() ;
          sc = boostTree( theSignal , theSignalAtRest , theGenEvent , 
                          theVector ) ;
          if ( ! sc.isSuccess() ) Exception( "Cannot boost signal tree" ) ;

          if ( m_cleanEvents ) { 
            sc = isolateSignal( theSignal ) ;
            if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
          }

          theGenEvent -> 
            set_signal_process_vertex( theSignal -> production_vertex() ) ;
          
          result = true ;
        }
      }   
    }
  }

  delete theSignalHepMCEvent ;
  return result ;
}

//=============================================================================
// Boost the Tree theSignal to theVector frame and attach it to theMother
//=============================================================================
StatusCode SignalForcedFragmentation::boostTree( HepMC::GenParticle * 
                                                 theSignal ,
                                                 const HepMC::GenParticle * 
                                                 theSignalAtRest ,
                                                 HepMC::GenEvent * theEvent ,
                                                 const Hep3Vector & theVector )
  const {
  if ( 0 == theSignalAtRest -> end_vertex() ) return StatusCode::SUCCESS ;
    
  if ( 0 != theSignal -> end_vertex() ) 
    return Error( "The particle has already a decay vertex !" ) ;

  if ( 0 == theSignalAtRest -> production_vertex() )
    return Error( "The particle has no production vertex !" ) ;
  
  // Displacement in original frame
  HepLorentzVector position = theSignalAtRest -> end_vertex() -> position() -
    theSignalAtRest -> production_vertex() -> position() ;
  // Displacement in new frame after boost.
  HepLorentzVector newPosition = position.boost( theVector ) ;
    
  // Create new HepMC vertex after boost and add it to the current event    
  HepMC::GenVertex * newVertex = 
    new HepMC::GenVertex( newPosition + 
                          theSignal -> production_vertex() -> position() ) ;
  theEvent  -> add_vertex( newVertex ) ;
  newVertex -> add_particle_in( theSignal ) ;

  HepMC::GenVertex * sVertex = theSignalAtRest -> end_vertex() ;
  
  HepMC::GenVertex::particles_out_const_iterator child ;
  
  for ( child  = sVertex -> particles_out_const_begin( ) ; 
        child != sVertex -> particles_out_const_end( ) ; ++child ) {
    // Boost all daughter particles and create a new HepMC particle
    // for each daughter
    HepLorentzVector momentum    = (*child) -> momentum() ;
    HepLorentzVector newMomentum = momentum.boost( theVector ) ;
    int id                       = (*child) -> pdg_id() ;
    int status                   = (*child) -> status() ;
    
    HepMC::GenParticle * newPart =
      new HepMC::GenParticle( newMomentum , id , status ) ;
    
    newVertex -> add_particle_out( newPart ) ;
    
    HepMC::GenParticle * theNewSignal             = newPart ;
    const HepMC::GenParticle * theNewSignalAtRest = (*child) ;
    
    // Recursive call to boostTree for each daughter
    boostTree( theNewSignal , theNewSignalAtRest , theEvent , 
               theVector ) ;
  }

  return StatusCode::SUCCESS ;
}
