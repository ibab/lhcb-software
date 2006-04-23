// $Id: StandAloneDecayTool.cpp,v 1.1 2006-04-23 21:25:44 robbep Exp $
// Include files 
#include "StandAloneDecayTool.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from Generators
#include "Generators/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandAloneDecayTool
//
// 2006-04-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the tool factory
static const  ToolFactory< StandAloneDecayTool > s_factory ;
const        IToolFactory & StandAloneDecayToolFactory = s_factory ;

//=============================================================================
// const constructor, initializes variables
//=============================================================================
StandAloneDecayTool::StandAloneDecayTool( const std::string& type ,
                                          const std::string& name ,
                                          const IInterface * parent )
  : Signal ( type , name , parent ) , m_signalMass( 0. ) {} 
//=============================================================================
// Destructor
//=============================================================================
StandAloneDecayTool::~StandAloneDecayTool() {;} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StandAloneDecayTool::initialize() {
  StatusCode sc = Signal::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  IParticlePropertySvc * ppSvc = 
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  ParticleProperty * prop = ppSvc -> findByStdHepID( *m_pids.begin() ) ;
  m_signalMass = prop -> mass() ;

  release( ppSvc ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
bool StandAloneDecayTool::generate( const unsigned int nPileUp , 
                                    LHCb::HepMCEvents * theEvents ,
                                    LHCb::GenCollisions * theCollisions ) {
  // prepare event
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  // generate the requested number of "pile-up" events
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent , 
                        theGenCollision ) ;
    
    // Particle to decay
    HepMC::GenParticle * theParticle = new HepMC::GenParticle( ) ;
    theParticle -> 
      set_momentum( HepLorentzVector( 0. , 0. , 0., m_signalMass ) ) ;

    // Decay the particle at (0,0,0,0)
    HepMC::GenVertex * theVertex = 
      new HepMC::GenVertex( HepLorentzVector( 0., 0., 0., 0. ) ) ;
    theGenEvent -> add_vertex( theVertex ) ;
    theVertex -> add_particle_out( theParticle ) ;
    
    bool flip( false ) ;
    
    int thePID = *m_pids.begin() ;
    if ( m_cpMixture ) {
      // decide the PID to generate
      double flavour = m_flatGenerator() ;
      m_decayTool -> enableFlip() ;
      
      if ( flavour < 0.5 ) 
        theParticle -> set_pdg_id( +abs( thePID ) ) ;
      else
        theParticle -> set_pdg_id( -abs( thePID ) ) ;
    } else {
      m_decayTool -> disableFlip() ;
      theParticle -> set_pdg_id( thePID ) ;
    }
    
    m_decayTool -> generateSignalDecay( theParticle , flip ) ;
    
    theParticle -> set_status( LHCb::HepMCEvent::SignalInLabFrame ) ;
  
    theGenEvent -> 
      set_signal_process_vertex( theParticle -> end_vertex() ) ;
    theGenCollision -> setIsSignal( true ) ;
  }
  
  return true ;
}

//=============================================================================

