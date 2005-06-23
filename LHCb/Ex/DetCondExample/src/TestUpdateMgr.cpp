// $Id: TestUpdateMgr.cpp,v 1.2 2005-06-23 13:57:14 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "DetDesc/IUpdateManagerSvc.h"
#include "DetDesc/ValidDataObject.h"

// local
#include "TestUpdateMgr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestUpdateMgr
//
// 2005-04-27 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TestUpdateMgr>          s_factory ;
const        IAlgFactory& TestUpdateMgrFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestUpdateMgr::TestUpdateMgr( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_evtCount(0)
{
}
//=============================================================================
// Destructor
//=============================================================================
TestUpdateMgr::~TestUpdateMgr() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestUpdateMgr::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    m_ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);
    
    m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod1);
    m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod2);
    m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
    m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
    m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod2);
    
    m_ums->registerCondition(this,"/dd/Structure/LHCb/Dummy",&TestUpdateMgr::i_updateMethod3);
    
    m_ums->registerCondition(&m_intermediate,"/dd/SlowControl/Hcal/scHcal",&InternalClass::myTinyMethod);
    
    m_ums->registerCondition(this,&m_intermediate,&TestUpdateMgr::i_updateMethod4);
  } catch (GaudiException) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestUpdateMgr::execute() {

  debug() << "==> Execute" << endmsg;
  if ( ++m_evtCount == 3 ) {
    debug() << " invalidating m_intermediate" << endmsg;
    m_ums->invalidate(&m_intermediate);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestUpdateMgr::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  m_ums->dump();
  
  m_ums->unregister(&m_intermediate);
  m_ums->unregister(this);

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
