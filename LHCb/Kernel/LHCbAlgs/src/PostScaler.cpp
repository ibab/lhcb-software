// Include files 

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"

// local
#include "PostScaler.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PostScaler
//
// 2003-03-16 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( PostScaler )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PostScaler::PostScaler( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_nEvents(0)
  , m_nEventsAll(0)
  , m_eventCounter(0)
{
  
  declareProperty("PercentPass", m_percentPass=100.0 );
  declareProperty("ForcedReduction", m_forcedReduction=1. );
  declareProperty("SafetyMargin", m_margin=0.5 );
  m_percentPass.verifier().setBounds( 0.0, 100.0 );
}

//=============================================================================
// Destructor
//=============================================================================
PostScaler::~PostScaler() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PostScaler::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Initialize" << endmsg;

  if (m_forcedReduction > 1.)
    m_eventCounter = tool<IEventCounter>( "EvtCounter" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PostScaler::execute() {

  bool accepted = true ;
  m_nEventsAll++ ;
  
  // random number method
  if (m_percentPass < 100.){    
    Rndm::Numbers random(randSvc(), Rndm::Flat(0.0,100.));
    if ( random ){
      double r = random() ;
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << "Random number thrown: " << r << endmsg;
      if ( r > m_percentPass ) {
        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
          verbose() << "Random filter failed" << endmsg;
        accepted = false ;
      } else {
        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
          verbose() << "Random filter passed" << endmsg;
      }
    }
  }
  
  // Event number fraction method
  if (m_forcedReduction > 1.){
    long m_event = m_eventCounter->getEventCounter() ;
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose() << "event number is now " << m_event << endmsg;
  
    double max_evts = (double)m_event/m_forcedReduction + m_margin ;
    //  max_evts = max_evts + 3*sqrt(max_evts);  /// 3 sigma security
    if ( m_nEvents > max_evts ) {
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << "Filter not passed as " << m_nEvents
                  << " > " << max_evts << " (reduction 1/" << m_forcedReduction  
                  << " + " << m_margin 
                  << ")" << endmsg;    
      accepted = false ;
    } else {
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << "Filter passed." << endmsg;
    }
  }
  
  // event passed
  if ( accepted ) m_nEvents++;
  
  setFilterPassed( accepted ) ;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PostScaler::finalize() {

  info() << "Event Filtered " << m_nEventsAll << endmsg;
  info() << "Event Accepted " << m_nEvents << endmsg;

  return GaudiAlgorithm::finalize() ;
}

//=============================================================================
