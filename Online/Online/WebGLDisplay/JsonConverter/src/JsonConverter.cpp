// Include files 
#include <iostream>

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "JsonConverter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : JsonConverter
//
// 2014-12-03 : Ben Couturier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( JsonConverter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
JsonConverter::JsonConverter( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
JsonConverter::~JsonConverter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode JsonConverter::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode JsonConverter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  std::cout << "=====> IN JSON CONVERTER" << std::endl;
  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode JsonConverter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
