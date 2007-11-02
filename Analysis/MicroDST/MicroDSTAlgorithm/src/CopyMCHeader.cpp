// $Id: CopyMCHeader.cpp,v 1.1 2007-11-02 16:24:00 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/MCHeader.h"
// local
#include "CopyMCHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyMCHeader
//
// 2007-11-02 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyMCHeader );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyMCHeader::CopyMCHeader( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : CopyAndStoreData ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyMCHeader::~CopyMCHeader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyMCHeader::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::MCHeaderLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyMCHeader::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store MCHeader bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return 
    (0!=copyAndStoreObject<LHCb::MCHeader,MCHeaderCopy>( inputTESLocation(),
                                                         fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyMCHeader::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
