// $Id: TestUpdateMgr.cpp,v 1.5 2006-01-19 18:32:11 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DataObject.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DetDesc/ValidDataObject.h"

// local
#include "TestUpdateMgr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestUpdateMgr
//
// 2005-04-27 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestUpdateMgr );



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestUpdateMgr::TestUpdateMgr( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_evtCount(0)
{
  m_dummyUMSentry = new DataObject();
}
//=============================================================================
// Destructor
//=============================================================================
TestUpdateMgr::~TestUpdateMgr() {
  delete m_dummyUMSentry;
}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestUpdateMgr::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    
    registerCondition("/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod1);
    registerCondition("/dd/SlowControl/LHCb/scLHCb",&TestUpdateMgr::i_updateMethod2);
    registerCondition("/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
    registerCondition("/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod3);
    registerCondition("/dd/SlowControl/Hcal/scHcal",&TestUpdateMgr::i_updateMethod2);
    
    registerCondition("/dd/Structure/LHCb/Dummy",&TestUpdateMgr::i_updateMethod3);
    
    updMgrSvc()->registerCondition(&m_intermediate,"/dd/SlowControl/Hcal/scHcal",&InternalClass::myTinyMethod);
    
    registerCondition(&m_intermediate,&TestUpdateMgr::i_updateMethod4);

    // I'm crazy... but that must work!
    updMgrSvc()->registerCondition(m_dummyUMSentry);
    registerCondition<TestUpdateMgr>(m_dummyUMSentry,NULL);

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
    updMgrSvc()->invalidate(&m_intermediate);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestUpdateMgr::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  updMgrSvc()->dump();
  
  updMgrSvc()->unregister(m_dummyUMSentry);
  updMgrSvc()->unregister(&m_intermediate);
  updMgrSvc()->unregister(this);

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
