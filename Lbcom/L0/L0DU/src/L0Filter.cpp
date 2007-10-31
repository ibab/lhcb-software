// $Id: L0Filter.cpp,v 1.2 2007-10-31 15:04:46 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/L0DUReport.h"

// local
#include "L0Filter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0Filter
//
// 2007-07-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Filter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Filter::L0Filter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_l0channels.clear();
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );
  declareProperty("OrChannels", m_l0channels );

}
//=============================================================================
// Destructor
//=============================================================================
L0Filter::~L0Filter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Filter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  info() << "Will require that any " ;
  if ( !m_l0channels.empty()) {
    info() << "of " ;
    for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
          c != m_l0channels.end() ; ++c ) {
      info() << *c << " " ;
    }
  }
  info() << "channel(s) is passed" << endmsg ;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Filter::execute() {

  debug() << "==> Execute" << endmsg;

  const  LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(m_l0Location);

  setFilterPassed(false); // switch off by default

  if (l0->decision()){ 
    if ( !m_l0channels.empty()) {
      for ( std::vector<std::string>::const_iterator c = m_l0channels.begin() ; 
            c != m_l0channels.end() ; ++c ) {
        if (l0->channelDecisionByName(*c)){
          setFilterPassed(true);
          if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by " 
                                                 << *c << endmsg ;
        } else if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is not accepted by " 
                                                      << *c << endmsg ; 
      }
    } else { // any decision will do
      if ( msgLevel(MSG::VERBOSE)) verbose() << "Event is accepted by L0" << endmsg ;
      setFilterPassed(true);
    }
  } else {
    setFilterPassed(false);
    verbose() << "Event is rejected by L0" << endmsg ;
  }
  if ( msgLevel(MSG::DEBUG)){
    if ( filterPassed()) debug() << "Event is accepted" << endmsg ;
    else debug() << "Event is rejected" << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Filter::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
