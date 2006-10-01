// $Id: SignalPlain.cpp,v 1.11 2006-10-01 22:43:39 robbep Exp $
// Include files 

// local
#include "SignalPlain.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/IDecayTool.h"
#include "Generators/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalPlain
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SignalPlain>          s_factory ;
const        IToolFactory& SignalPlainFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalPlain::SignalPlain( const std::string& type, const std::string& name,
                          const IInterface* parent )
  : Signal( type, name , parent ) { ; }

//=============================================================================
// Destructor
//=============================================================================
SignalPlain::~SignalPlain( ) { ; }

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool SignalPlain::generate( const unsigned int nPileUp , 
                            LHCb::HepMCEvents * theEvents , 
                            LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  bool result = false ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;
    
    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_signalQuark , m_signalPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {

        // establish correct multiplicity of signal
        if ( ensureMultiplicity( theParticleList.size() ) ) {
          
          m_nEventsBeforeCut++ ;

          updateCounters( theParticleList , m_nParticlesBeforeCut , 
                          m_nAntiParticlesBeforeCut , false ) ;          
          
          bool passCut = true ;
          if ( 0 != m_cutTool ) 
            passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                             theGenCollision , m_decayTool , 
                                             m_cpMixture , 0 ) ;
          
          if ( passCut && ( ! theParticleList.empty() ) ) {
            m_nEventsAfterCut++ ;
            
            updateCounters( theParticleList , m_nParticlesAfterCut , 
                            m_nAntiParticlesAfterCut , true ) ;
            
            HepMC::GenParticle * theSignal =
              chooseAndRevert( theParticleList ) ;
            
            bool flip ;
            if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
            m_decayTool -> generateSignalDecay( theSignal , flip ) ;
            
            if ( flip ) { 
              // Remove all daughter particles from signal and ask to
              // re-generate decay
              HepMCUtils::RemoveDaughters( theSignal ) ;
              continue ;
            }
            
            if ( m_cleanEvents ) {
              sc = isolateSignal( theSignal ) ;
              if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
            }
            theGenEvent -> 
              set_signal_process_vertex( theSignal -> end_vertex() ) ;
            
            theGenCollision -> setIsSignal( true ) ;
            
            // Count signal B and signal Bbar
            if ( theSignal -> pdg_id() > 0 ) ++m_nSig ;
            else ++m_nSigBar ;
            
            // Update counters
            GenCounters::updateHadronCounters( theGenEvent , m_bHadC , 
                                               m_antibHadC , m_cHadC , 
                                               m_anticHadC , m_bbCounter ,
                                               m_ccCounter ) ;
            GenCounters::updateExcitedStatesCounters( theGenEvent , 
                                                      m_bExcitedC , 
                                                      m_cExcitedC ) ;
            
            result = true ;
          }
        }
      }
    }
  }  
  
  return result ;
}

