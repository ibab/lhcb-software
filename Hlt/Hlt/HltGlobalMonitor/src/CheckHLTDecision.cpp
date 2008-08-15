// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "CheckHLTDecision.h"
#include "Event/HltSummary.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : CheckHLTDecision
//
// 2007-05-16 : Bruno Souza de Paula
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CheckHLTDecision );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckHLTDecision::CheckHLTDecision( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  

}
//=============================================================================
// Destructor
//=============================================================================
CheckHLTDecision::~CheckHLTDecision() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckHLTDecision::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  info() << " Doing CheckHLTDecision::initialize() " << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckHLTDecision::execute() {  
    if (!exist<LHCb::HltSummary>(LHCb::HltSummaryLocation::Default)){
      info() << "No HLT Summary" << endreq; 
      setFilterPassed( false );
      return StatusCode::SUCCESS;
    }
   LHCb::HltSummary* summary = get<LHCb::HltSummary>(LHCb::HltSummaryLocation::Default);
//  if (summary->decision()) info() << "HLT decision yes" << endreq; 
  // IN THE EFF you can use the hltData() the FULL summary!
//   stringKey keyphys("HltPhysics");
//   stringKey keyrand("HltRandom");
//   if (hltData().selection(keyrand).decision() || hltData().selection(keyphys).decision()){ 
   LHCb::HltSelectionSummary& physsummary = summary->selectionSummary("PhysicsTrigger");
   LHCb::HltSelectionSummary& randsummary = summary->selectionSummary("RandomTrigger");
//if (thissummary.decision()) return true;
  if (physsummary.decision() || randsummary.decision()){ 
    debug() << "Event Accepted " << endreq; 
    setFilterPassed( true );
    return StatusCode::SUCCESS;
  }
  else {
    debug() << "Event Rejected" << endreq; 
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckHLTDecision::finalize() {

  info() << " Doing CheckHLTDecision::finalize() " << endreq;

  return HltBaseAlg::finalize();  
  // must be called after all other actions

}

//=============================================================================
