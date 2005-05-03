// $Id: TestUpdateMgr.cpp,v 1.1 2005-05-03 12:46:19 marcocle Exp $
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
  : GaudiAlgorithm ( name , pSvcLocator )
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

  sc = serviceLocator()->service("UpdateManagerSvc",m_ums,true);
  if (!sc.isSuccess()) {
    error() << "Unable to find UpdateManagerSvc" <<endmsg;
    return sc;
  }
  m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod1);
  m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod2);
  m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
  m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
  m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod2);

  m_ums->registerCondition(this,"/dd/Structure/LHCb/Dummy",&TestUpdateMgr::i_updateMethod3);

  m_ums->registerCondition(&m_intermediate,"/dd/SlowControl/Hcal/scHcal",&InternalClass::myTinyMethod);

  m_ums->registerCondition(this,&m_intermediate,&TestUpdateMgr::i_updateMethod4);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestUpdateMgr::execute() {

  debug() << "==> Execute" << endmsg;
  static int count = 0;
  if ( ++count == 3 ) {
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
