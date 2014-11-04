// Include files 
// from Gaudi
#include "Event/ODIN.h" // event & run number
#include "GaudiKernel/AlgFactory.h" 
// local
#include "TurboMIAB.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TurboMIAB
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TurboMIAB )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TurboMIAB::TurboMIAB( const std::string& name,
                                    ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator )
{
}

//=============================================================================
// Destructor
//=============================================================================
TurboMIAB::~TurboMIAB() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TurboMIAB::initialize()
{

  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TurboMIAB::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "Passing the message along that Turbo level reports are to be made for this line" << endmsg;
  return StatusCode::SUCCESS;
}
