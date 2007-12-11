// $Id: CopyODIN.cpp,v 1.5 2007-12-11 17:37:12 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/ODIN.h"
// local
#include "CopyODIN.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyODIN
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyODIN );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyODIN::CopyODIN( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyODIN::~CopyODIN() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyODIN::initialize() {
  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::ODINLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyODIN::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);
  
  verbose() << "Going to store ODIN bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return (0!=copyAndStoreObject<LHCb::ODIN,ODINCopy>(inputTESLocation(),
                                                     fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyODIN::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
