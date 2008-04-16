// $Id: CopyFlavourTag.cpp,v 1.1 2008-04-16 10:25:13 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/FlavourTag.h"
// local
#include "CopyFlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyFlavourTag
//
// 2008-04-16 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyFlavourTag );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyFlavourTag::CopyFlavourTag( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyFlavourTag::~CopyFlavourTag() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyFlavourTag::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::FlavourTagLocation::Default);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyFlavourTag::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(true);

  verbose() << "Going to store FlavourTag bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;
  return 
    (0!=copyKeyedContainer<LHCb::FlavourTags,FlavourTagKeyedItemCopy>( inputTESLocation(),
                                                                       fullOutputTESLocation() ) ) ?
    StatusCode::SUCCESS : StatusCode::FAILURE;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyFlavourTag::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
