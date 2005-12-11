// $Id: SignalPlain.cpp,v 1.5 2005-12-11 23:22:30 robbep Exp $
// Include files 

// local
#include "SignalPlain.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/IDecayTool.h"

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
SignalPlain::SignalPlain( const std::string& type,
                          const std::string& name,
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
                            EventVector & theEventVector , 
                            HardVector  & theHardVector ) {
  StatusCode sc ;
  bool result = false ;
  HardInfo * theHardInfo( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEventVector, theHardVector, theGenEvent, 
                        theHardInfo ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_signalMass , m_signalPID ) ;
      
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
                                             theHardInfo ) ;
          
          if ( passCut && ( ! theParticleList.empty() ) ) {
            m_nEventsAfterCut++ ;
            
            updateCounters( theParticleList , m_nParticlesAfterCut , 
                            m_nAntiParticlesAfterCut , true ) ;
            
            HepMC::GenParticle * theSignal = chooseAndRevert( theParticleList ) ;
            
            bool flip ;
            if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
            m_decayTool -> generateSignalDecay( theSignal , flip ) ;

            if ( flip ) continue ;
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
  }  
  
  return result ;
}
