// $Id: FireUpdateManager.cpp,v 1.1 2005-05-03 12:46:19 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "DetDesc/IUpdateManagerSvc.h"

// local
#include "FireUpdateManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FireUpdateManager
//
// 2005-03-31 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FireUpdateManager>          s_factory ;
const        IAlgFactory& FireUpdateManagerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FireUpdateManager::FireUpdateManager( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_updateMgr(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
FireUpdateManager::~FireUpdateManager() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FireUpdateManager::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  sc = service("UpdateManagerSvc",m_updateMgr,true);
  if ( !sc.isSuccess() ){
    error() << "Unable to get a pointer to IUpdateManagerSvc" << endmsg;
    return sc;
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FireUpdateManager::execute() {

  debug() << "==> Execute" << endmsg;
  debug() << "  firing the update manager" << endmsg;
  return m_updateMgr->newEvent();
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FireUpdateManager::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_updateMgr->release();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
