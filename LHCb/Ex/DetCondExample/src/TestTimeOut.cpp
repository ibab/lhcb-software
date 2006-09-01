// $Id: TestTimeOut.cpp,v 1.2 2006-09-01 08:43:32 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "SealBase/TimeInfo.h"

// local
#include "TestTimeOut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestTimeOut
//
// 2006-08-31 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestTimeOut );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestTimeOut::TestTimeOut( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("SleepTime", m_sleepTime = 20);
}
//=============================================================================
// Destructor
//=============================================================================
TestTimeOut::~TestTimeOut() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestTimeOut::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestTimeOut::execute() {

  debug() << "==> Execute" << endmsg;
  info() << "Sleeping ..." << endmsg;
  seal::TimeInfo::sleep(m_sleepTime);
  info() << "... done." << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestTimeOut::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
