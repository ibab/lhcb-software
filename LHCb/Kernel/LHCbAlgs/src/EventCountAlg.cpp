// Include files

// local
#include "EventCountAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventCountAlg
//
// 2007-01-08 : Robert Lambert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventCountAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventCountAlg::EventCountAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  //declare the filename to write. If the string is empty, no file will be written
  declareProperty("CounterName",m_counterName="efficiency");
  //I know this is a stat entity, make sure it's written as such
  StatusCode sc = setProperty ( "StatEntityList" , "[ \".*\" ]" );
  // m_statEntityList = {".*"};
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventCountAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  //prepare the incident service
  m_incSvc = service("IncidentSvc", false);
  if(!m_incSvc) return StatusCode::FAILURE;
  m_incSvc->addListener( this, IncidentType::BeginEvent);
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventCountAlg::execute() {

  m_nExecuted++;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventCountAlg::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  //create the statEntity
  //                                                     entries    flag          flag2      min, max
  StatEntity anent(
                   m_nHandled, m_nExecuted,
                   m_nExecuted, int(m_nHandled>m_nExecuted),
                   int(m_nExecuted!=0));
  counter(m_counterName)= anent;

  return GaudiAlgorithm::finalize();
}

//=============================================================================
