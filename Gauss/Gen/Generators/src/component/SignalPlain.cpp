// $Id: SignalPlain.cpp,v 1.1 2005-10-03 10:30:25 robbep Exp $
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
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    HepMCEvent * theHepMCEvent = new HepMCEvent( m_productionTool -> name() ,
                                                 1 , 1 ) ;
    HardInfo * theHardInfo = new HardInfo( ) ;

    HepMC::GenEvent * theGenEvent = theHepMCEvent -> pGenEvt() ;
    theHardInfo -> setEvent( theHepMCEvent ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    theEventVector.push_back( theHepMCEvent ) ;
    theHardVector .push_back( theHardInfo   ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_signalMass , m_signalPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {

        if ( ( m_cpMixture ) && ( 1 == theParticleList.size() ) ) 
          if ( m_flatGenerator() < ( ( 1. - m_signalBr ) / 
                                     ( 2. - m_signalBr ) ) ) continue ;
        
        m_nEventsBeforeCut++ ;
        if ( 0 != m_cutTool ) m_cutTool -> applyCut( theParticleList ) ;
        
        if ( ! theParticleList.empty() ) {
          m_nEventsAfterCut++ ;

          HepMC::GenParticle * theSignal = 0 ;
          unsigned int nPart = theParticleList.size() ;
          if ( nPart > 1 ) {
            unsigned iPart = 
              (unsigned int) floor( nPart * m_flatGenerator() ) ;
            theSignal = theParticleList[ iPart ] ;
          }
          else theSignal = theParticleList.front() ;

          if ( theSignal -> momentum() . pz() < 0 ) {
            revertEvent( theGenEvent ) ;
            m_nInvertedEvents++ ;
          }

          bool flip ;
          if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
          m_decayTool -> generateSignalDecay( theGenEvent , theSignal , 
                                              flip ) ;
          if ( flip ) continue ;
          result = true ;
        }
      }
    }
  }  
  
  return result ;
}
