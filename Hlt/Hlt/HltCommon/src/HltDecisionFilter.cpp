// $Id: HltDecisionFilter.cpp,v 1.1 2008-09-04 12:25:20 graven Exp $ // Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HltDecisionFilter.h"
#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDecisionFilter
//
// 2008-09-02 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDecisionFilter );

//=============================================================================
// parser code...
//=============================================================================


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDecisionFilter::HltDecisionFilter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  declareProperty("Accept", m_filterDescription = "" );
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
HltDecisionFilter::~HltDecisionFilter() {}; 
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltDecisionFilter::initialize() {
  debug() << "==> Initialize" << endmsg;
  m_factory = tool<IHltDecisionPredicateFactory>("HltDecisionPredicateFactory");
  m_filter = m_factory->create( m_filterDescription );
  info() << "using accept filter '" << m_filterDescription << "'" << endmsg;
  return StatusCode::SUCCESS;
};
//=============================================================================
// Restart
//=============================================================================
StatusCode HltDecisionFilter::restart() {
  debug() << "==> Restart" << endmsg;
  m_filter = m_factory->create( m_filterDescription );
  info() << "using accept filter '" << m_filterDescription << "'" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltDecisionFilter::execute() {
  LHCb::HltDecReports* decisions = get<LHCb::HltDecReports>( m_location );
  bool pass = (*m_filter)(*decisions);
  setFilterPassed(pass);
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltDecisionFilter::finalize() {
  //TODO: add statistics...
  return GaudiAlgorithm::finalize();  
}

//=============================================================================
