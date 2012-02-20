// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 


// local
#include "TutorialSpDetElemAlg.h"
#include "DeTutorialMaster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialSpDetElemAlg
//
// 2012-02-13 : Sajan Easo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TutorialSpDetElemAlg )



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialSpDetElemAlg::TutorialSpDetElemAlg( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
TutorialSpDetElemAlg::~TutorialSpDetElemAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialSpDetElemAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize  TutorialSpDetElemAlg " << endmsg;

  DeTutorialMaster* deMas = getDet<DeTutorialMaster> ( "/dd/Structure/LHCb/AfterMagnetRegion/TutorialMasterDet");
  if(deMas) {
    
    deMas->FindGlobalPoint(0.0,0.0, 25.0);
    
     
  }
  
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialSpDetElemAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialSpDetElemAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
