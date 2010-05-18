// $Id: EventCountAlg.cpp,v 1.3 2010-05-18 21:33:39 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/AlgFactory.h"

// local
#include "EventCountAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventCountAlg
//
// 2007-01-08 : Robert Lambert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( EventCountAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventCountAlg::EventCountAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_nHandled    ( 0 )
  , m_nExecuted   ( 0 )
  , m_counterName ( "" )
  , m_incSvc      (0)
{

  //declare the filename to write. If the string is empty, no file will be written
  declareProperty("CounterName",m_counterName="efficiency");
  //I know this is a stat entity, make sure it's written as such
  declareProperty("StatEntityList",m_statEntityList=std::vector<std::string>(1,".*"));

}
//=============================================================================
// Destructor
//=============================================================================
EventCountAlg::~EventCountAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode EventCountAlg::initialize() {
  
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;
  
  //prepare the incident service
  sc=service("IncidentSvc", m_incSvc, false);
  if(!sc.isSuccess() || m_incSvc== NULL) return StatusCode::FAILURE;
  m_incSvc->addListener( this, IncidentType::BeginEvent);
  
  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventCountAlg::execute() {

  //debug() << "==> Execute" << endmsg;

  // code goes here  

  m_nExecuted++;

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventCountAlg::finalize() {

  debug() << "==> Finalize" << endmsg;
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
