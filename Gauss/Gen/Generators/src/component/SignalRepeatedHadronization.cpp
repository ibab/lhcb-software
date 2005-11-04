// $Id: SignalRepeatedHadronization.cpp,v 1.1 2005-11-04 10:48:24 robbep Exp $
// Include files 

// local
#include "SignalRepeatedHadronization.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Kernel
#include "Kernel/ParticleID.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/IDecayTool.h"
#include "Generators/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SignalRepeatedHadronization
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<SignalRepeatedHadronization>          s_factory ;
const        IToolFactory& SignalRepeatedHadronizationFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SignalRepeatedHadronization::SignalRepeatedHadronization( 
  const std::string& type, const std::string& name, const IInterface* parent )
  : Signal( type, name , parent ) { 
    declareProperty( "MaxNumberOfRepetitions" , 
                     m_maxNumberOfRepetitions = 500 ) ; 
  }

//=============================================================================
// Destructor
//=============================================================================
SignalRepeatedHadronization::~SignalRepeatedHadronization( ) { ; }

//=============================================================================
// Initialize Method
//=============================================================================
StatusCode SignalRepeatedHadronization::initialize( ) {
  StatusCode sc = Signal::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  // Determine quark types
  ParticleID pid( *m_pids.begin() ) ;
  if ( pid.hasBottom() ) {
    m_pidQuarks.insert( -ParticleID::bottom ) ;
    m_pidQuarks.insert( ParticleID::bottom ) ;
  } else if ( pid.hasCharm() ) {
    m_pidQuarks.insert( -ParticleID::charm ) ;
    m_pidQuarks.insert( ParticleID::charm ) ;
  }

  return sc ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool SignalRepeatedHadronization::generate( const unsigned int nPileUp ,
                                            EventVector & theEventVector , 
                                            HardVector  & theHardVector ) {
  StatusCode sc ;
  bool gotSignalInteraction = false ;

  HardInfo * theHardInfo( 0 )  ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  HepMC::GenParticle * theSignal ;
  bool flip ;

  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    bool partonEventWithSignalQuarks = false ;
    ParticleVector theQuarkList ;

    prepareInteraction( theEventVector , theHardVector , theGenEvent , 
                        theHardInfo ) ;

    if ( ! gotSignalInteraction ) m_productionTool -> turnOffFragmentation( ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! gotSignalInteraction ) {
      if ( checkPresence( m_pidQuarks , theGenEvent , theQuarkList ) ) {
        partonEventWithSignalQuarks = true ;
        m_productionTool -> savePartonEvent( theGenEvent ) ;
      } 
      
      m_productionTool -> turnOnFragmentation( ) ;
      // Clear theGenEvent
      HepMCUtils::Clear( theGenEvent ) ;
      m_productionTool -> hadronize( theGenEvent , theHardInfo ) ;
      
      // Check if one particle of the requested list is present in event
      unsigned int nRepetitions = 0 ;
      
      while ( nRepetitions < m_maxNumberOfRepetitions ) {
        // Decay heavy particles
        decayHeavyParticles( theGenEvent , m_signalMass , m_signalPID ) ;

        // Check if one particle of the requested list is present in event
        ParticleVector theParticleList ;
        if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
          
          // establish correct multiplicity of signal
          if ( ensureMultiplicity( theParticleList.size() ) ) {

            m_nEventsBeforeCut++ ;
            if ( 0 != m_cutTool ) m_cutTool -> applyCut( theParticleList ) ;
            
            if ( ! theParticleList.empty() ) {
              m_nEventsAfterCut++ ;
              
              theSignal = chooseAndRevert( theParticleList , theGenEvent ) ;
              
              flip = false ;
              if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
              m_decayTool -> generateSignalDecay( theGenEvent , theSignal , 
                                                  flip ) ;
              if ( ! flip ) gotSignalInteraction = true ;
            }
            // if the interaction is not kept, we must re-hadronize it
            // once to have a fresh unbiased event
            if ( ! gotSignalInteraction ) {
              nRepetitions++ ;
              m_productionTool -> retrievePartonEvent( theGenEvent ) ;
              m_productionTool -> turnOnFragmentation( ) ;
              m_productionTool -> savePartonEvent( theGenEvent ) ;
              HepMCUtils::Clear( theGenEvent ) ;
              m_productionTool -> hadronize( theGenEvent , theHardInfo ) ;
            }
            // Then we exit and do not re-hadronize this event
            // not to bias things
            break ;
          }
        } 
        
        if ( ! partonEventWithSignalQuarks ) break ;

        nRepetitions++ ;
        m_productionTool -> retrievePartonEvent( theGenEvent ) ;
        m_productionTool -> turnOnFragmentation( ) ;
        m_productionTool -> savePartonEvent( theGenEvent ) ;
        // Clear HepMC event
        HepMCUtils::Clear( theGenEvent ) ;
        m_productionTool -> hadronize( theGenEvent , theHardInfo ) ;
      }

      if ( nRepetitions == m_maxNumberOfRepetitions ) 
        info() << "Number of repetitions of hadronization exceeds the limit" 
               << endmsg ;
    }
  }

  return gotSignalInteraction ;
}
