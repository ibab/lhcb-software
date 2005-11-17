// $Id: SignalPlain.cpp,v 1.2 2005-11-17 15:57:31 robbep Exp $
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
          bool passCut = true ;
          if ( 0 != m_cutTool ) 
            passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                             theHardInfo ) ;
          
          if ( passCut && ( ! theParticleList.empty() ) ) {
            m_nEventsAfterCut++ ;

            HepMC::GenParticle * theSignal = 
              chooseAndRevert( theParticleList , theGenEvent ) ;
            
            bool flip ;
            if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
            m_decayTool -> generateSignalDecay( theGenEvent , theSignal , 
                                                flip ) ;
            if ( flip ) continue ;
            if ( m_cleanEvents ) {
              sc = isolateSignal( theSignal ) ;
              if ( ! sc.isSuccess() ) Exception( "Cannot isolate signal" ) ;
            }
            
            result = true ;
          }
        }
      }
    }
  }  
  
  return result ;
}
