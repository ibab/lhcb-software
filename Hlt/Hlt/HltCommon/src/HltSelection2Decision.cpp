// $Id: HltSelection2Decision.cpp,v 1.1 2008-09-04 12:20:01 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HltSelection2Decision.h"
#include "Event/HltDecReport.h"
#include "Event/HltDecReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelection2Decision
//
// 2008-08-22 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelection2Decision );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelection2Decision::HltSelection2Decision( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  declareProperty("InputSelection", m_inputName.property());
  declareProperty("DecisionName", m_decisionName = name );
  declareProperty("DecReportsLocation", m_location = LHCb::HltDecReportsLocation::Default);
}
//=============================================================================
// Destructor
//=============================================================================
HltSelection2Decision::~HltSelection2Decision() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelection2Decision::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if (!dataSvc().hasSelection(m_inputName)) return StatusCode::FAILURE;
  m_input = &dataSvc().selection(m_inputName,this);
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelection2Decision::execute() {
  
  LHCb::HltDecReports* decisions = getOrCreate<LHCb::HltDecReports,LHCb::HltDecReports>( m_location );
  if ( decisions->hasSelectionName( m_decisionName ) ) {
     debug() << "HltDecReport already contains decision for " << m_decisionName << endmsg;
     error() << "HltDecReport already contains 'my' decision" << endmsg;
     return StatusCode::FAILURE;
  }
  boost::optional<IANNSvc::minor_value_type> key = annSvc().value("Hlt1SelectionID",m_decisionName);
  if (!key) {
     debug() << "HltANNSvc does not know about " << m_decisionName << endmsg;
     error() << "'my' decision not known by HltANNSvc " << endmsg;
     return StatusCode::FAILURE;
  }
  debug() << "got input: " << m_decisionName << " and key " << key->first << " -> " << key->second << endmsg;
  //TODO: do we want to check 'isFilterPassed'  and/or 'isExecuted' of the algorithm 
  //      that produced m_input instead??? (or maybe in addition?)
  if (!m_input->decision() ) {
     warning() << "HltSelection2Decision: input did not pass..." << endmsg;
  }
  if (!m_input->processed() ) {
     warning() << "HltSelection2Decision: input not processed..." << endmsg;
  }
  int kError(0);
  kError |= ( !m_input->decision()  ? 0x01 : 0x00 );
  kError |= ( !m_input->processed() ? 0x02 : 0x00 );
  LHCb::HltDecReport decision(  m_input->decision(), 
                                kError, 
                                m_input->ncandidates(), 
                                key->second );
  if (decision.invalidIntSelectionID()) {
    warning() << " selectionName=" << key->first << " has invalid intSelectionID=" << key->second << " Skipped. " << endmsg;
  } else {
    decisions->insert( m_decisionName, decision );
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelection2Decision::finalize() {
  //TODO: add statistics...
  return GaudiAlgorithm::finalize();  
}

//=============================================================================
