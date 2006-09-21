// $Id: TestTimeOut.cpp,v 1.3 2006-09-21 13:15:47 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "DetDesc/Condition.h"
#include "DetDesc/DetectorElement.h"

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
  info() << "Try to access the db..." << endmsg;
  DetectorElement *lhcb = getDet<DetectorElement>( "/dd/Structure/LHCb" );
  
  Condition* alLHCb = lhcb->condition("SlowControl");
  if (alLHCb) {
    info() << "Succesfully retrieved condition '" << lhcb->condition("SlowControl").path() << "'" << endmsg;
  }
  else {
    error() << "Cannot retrieve condition '" << lhcb->condition("SlowControl").path() << "'" << endmsg;
    return StatusCode::FAILURE;
  }
  
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
