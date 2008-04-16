// $Id: CopyL0DUReport.cpp,v 1.1 2008-04-16 13:19:53 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/L0DUReport.h"
// local
#include "CopyL0DUReport.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyL0DUReport
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyL0DUReport );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyL0DUReport::CopyL0DUReport( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyL0DUReport::~CopyL0DUReport() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyL0DUReport::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::L0DUReportLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyL0DUReport::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store L0DUReport bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return (0!=copyAndStoreObject<LHCb::L0DUReport,L0DUReportCopy>( inputTESLocation(),
                                                                  fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyL0DUReport::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
