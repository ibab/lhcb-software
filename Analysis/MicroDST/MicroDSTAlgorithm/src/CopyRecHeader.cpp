// $Id: CopyRecHeader.cpp,v 1.3 2007-10-30 18:10:48 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecHeader.h"
// local
#include "CopyRecHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyRecHeader
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyRecHeader );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyRecHeader::CopyRecHeader( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : CopyAndStoreData ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyRecHeader::~CopyRecHeader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyRecHeader::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::RecHeaderLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyRecHeader::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store RecHeader bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return (0!=copyAndStoreObject<LHCb::RecHeader>( inputTESLocation(),
                                                  fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyRecHeader::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
