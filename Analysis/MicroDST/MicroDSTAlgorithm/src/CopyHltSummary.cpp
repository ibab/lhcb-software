// $Id: CopyHltSummary.cpp,v 1.1 2008-04-16 13:19:19 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/HltSummary.h"
// local
#include "CopyHltSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyHltSummary
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyHltSummary );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyHltSummary::CopyHltSummary( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyHltSummary::~CopyHltSummary() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyHltSummary::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::HltSummaryLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyHltSummary::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store HltSummary bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return (0!=copyAndStoreObject<LHCb::HltSummary,HltSummaryCopy>( inputTESLocation(),
                                                                  fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyHltSummary::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
