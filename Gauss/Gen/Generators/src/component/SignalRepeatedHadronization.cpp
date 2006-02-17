// $Id: SignalRepeatedHadronization.cpp,v 1.7 2006-02-17 13:27:28 robbep Exp $
// Include files 

// local
#include "SignalRepeatedHadronization.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Kernel
#include "Kernel/ParticleID.h"

// From HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/IDecayTool.h"

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
  LHCb::ParticleID pid( *m_pids.begin() ) ;
  if ( pid.hasBottom() ) {
    m_pidQuarks.insert( -LHCb::ParticleID::bottom ) ;
    m_pidQuarks.insert( LHCb::ParticleID::bottom ) ;
  } else if ( pid.hasCharm() ) {
    m_pidQuarks.insert( -LHCb::ParticleID::charm ) ;
    m_pidQuarks.insert( LHCb::ParticleID::charm ) ;
  }

  return sc ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool SignalRepeatedHadronization::generate( const unsigned int nPileUp ,
                                            LHCb::HepMCEvents * theEvents , 
                                            LHCb::GenCollisions * 
                                            theCollisions ) {
  StatusCode sc ;
  bool gotSignalInteraction = false ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  HepMC::GenParticle * theSignal ;
  bool flip ;

  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    bool partonEventWithSignalQuarks = false ;
    ParticleVector theQuarkList ;

    prepareInteraction( theEvents , theCollisions , theGenEvent , 
                        theGenCollision ) ;

    if ( ! gotSignalInteraction ) m_productionTool -> turnOffFragmentation( ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! gotSignalInteraction ) {
      if ( checkPresence( m_pidQuarks , theGenEvent , theQuarkList ) ) {
        partonEventWithSignalQuarks = true ;
        m_productionTool -> savePartonEvent( theGenEvent ) ;
      } 
      
      m_productionTool -> turnOnFragmentation( ) ;
      // Clear theGenEvent
      Clear( theGenEvent ) ;
      m_productionTool -> hadronize( theGenEvent , theGenCollision ) ;
      
      // Check if one particle of the requested list is present in event
      unsigned int nRepetitions = 0 ;
      
      while ( nRepetitions < m_maxNumberOfRepetitions ) {
        // Decay heavy particles
        decayHeavyParticles( theGenEvent , m_signalQuark , m_signalPID ) ;

        // Check if one particle of the requested list is present in event
        ParticleVector theParticleList ;
        if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
          
          // establish correct multiplicity of signal
          if ( ensureMultiplicity( theParticleList.size() ) ) {

            m_nEventsBeforeCut++ ;

            // Count particles and anti-particles of Signal type before 
            // the cut in all directions
            updateCounters( theParticleList , m_nParticlesBeforeCut , 
                            m_nAntiParticlesBeforeCut , false ) ;

            bool passCut = true ;
            if ( 0 != m_cutTool ) 
              passCut = m_cutTool -> applyCut( theParticleList , theGenEvent ,
                                               theGenCollision ) ;
            
            if ( passCut && ( ! theParticleList.empty() ) ) {
              m_nEventsAfterCut++ ;

              // Count particles and anti-particles of Signal type with
              // pz>0, after generator level cut
              updateCounters( theParticleList , m_nParticlesAfterCut , 
                              m_nAntiParticlesAfterCut , true ) ;

              // If there are several particles passing the cuts, choose
              // one and revert the event if it has pz<0              
              theSignal = chooseAndRevert( theParticleList ) ;
              
              flip = false ;
              if ( m_cpMixture ) m_decayTool -> enableFlip( ) ;
              m_decayTool -> generateSignalDecay( theSignal , flip ) ;

              if ( ! flip ) {
                gotSignalInteraction = true ;
                if ( m_cleanEvents ) {
                  sc = isolateSignal( theSignal ) ;
                  if ( ! sc.isSuccess() ) 
                    Exception( "Cannot isolate signal" ) ;
                }
                theGenEvent -> 
                  set_signal_process_vertex( theSignal -> end_vertex() ) ;

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
              }
            }
            // if the interaction is not kept, we must re-hadronize it
            // once to have a fresh unbiased event
            if ( ! gotSignalInteraction ) {
              nRepetitions++ ;
              m_productionTool -> retrievePartonEvent( theGenEvent ) ;
              m_productionTool -> turnOnFragmentation( ) ;
              m_productionTool -> savePartonEvent( theGenEvent ) ;
              Clear( theGenEvent ) ;
              m_productionTool -> hadronize( theGenEvent , theGenCollision ) ;
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
        Clear( theGenEvent ) ;
        m_productionTool -> hadronize( theGenEvent , theGenCollision ) ;
      }

      if ( nRepetitions == m_maxNumberOfRepetitions ) 
        info() << "Number of repetitions of hadronization exceeds the limit" 
               << endmsg ;
    }
  }

  return gotSignalInteraction ;
}

//=============================================================================
// Clear a complete HepMC event
//=============================================================================
void SignalRepeatedHadronization::Clear( HepMC::GenEvent * theEvent ) const {
  if ( ! theEvent -> vertices_empty() ) {
    HepMC::GenEvent::vertex_iterator iter ;
    for ( iter = theEvent -> vertices_begin() ; 
          iter != theEvent -> vertices_end()  ; ++iter ) {
      theEvent -> remove_vertex( *iter ) ;
      delete (*iter) ;
    }
  }
}
