// $Id: PostScaler.cpp,v 1.3 2007-11-28 13:43:20 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// local
#include "PostScaler.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PostScaler
//
// 2003-03-16 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( PostScaler );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PostScaler::PostScaler( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_nEvents(0)
  , m_nEventsAll(0)
{
  
  declareProperty("PercentPass", m_percentPass=100.0 );
  declareProperty("ForcedReduction", m_forcedReduction=1. );
  declareProperty("SafetyMargin", m_margin=0.5 );
  m_percentPass.verifier().setBounds( 0.0, 100.0 );
}

//=============================================================================
// Destructor
//=============================================================================
PostScaler::~PostScaler() {}; 

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
      verbose() << "Random number thrown: " << r << endreq;
      if ( r > m_percentPass ) {
        verbose() << "Random filter failed" << endreq;
        accepted = false ;
      } else verbose() << "Random filter passed" << endreq;
    }
  }
  
  // Event number fraction method
  if (m_forcedReduction > 1.){
    m_eventNumber = tool<IEventCounter>( "EvtCounter" );
    long m_event = m_eventNumber->getEventCounter() ;
    verbose() << "event number is now " << m_event << endmsg;
  
    double max_evts = (double)m_event/m_forcedReduction + m_margin ;
    //  max_evts = max_evts + 3*sqrt(max_evts);  /// 3 sigma security
    if ( m_nEvents > max_evts ) {
      verbose() << "Filter not passed as " << m_nEvents
              << " > " << max_evts << " (reduction 1/" << m_forcedReduction  
              << " + " << m_margin 
              << ")" << endreq;    
      accepted = false ;
    } else verbose() << "Filter passed." << endreq;
  }
  
  // event passed
  if ( accepted ) m_nEvents++;
  
  setFilterPassed( accepted ) ;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PostScaler::finalize() {

  info() << "Event Filtered " << m_nEventsAll << endreq;
  info() << "Event Accepted " << m_nEvents << endreq;

  return GaudiAlgorithm::finalize() ;
}

//=============================================================================
