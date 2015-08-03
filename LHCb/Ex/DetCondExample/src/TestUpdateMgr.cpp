// $Id: TestUpdateMgr.cpp,v 1.7 2007-07-31 17:39:10 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DataObject.h" 

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "DetDesc/ValidDataObject.h"
//#include "DetDesc/IUMSPurge.h"
#include "DetDesc/Condition.h"

// local
#include "TestUpdateMgr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestUpdateMgr
//
// 2005-04-27 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestUpdateMgr )



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
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestUpdateMgr::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    
    registerCondition("Conditions/Environment/LHCb/Temperature",&TestUpdateMgr::i_updateMethod1);
    registerCondition("Conditions/Environment/LHCb/Temperature",&TestUpdateMgr::i_updateMethod2);
    registerCondition("Conditions/Environment/Hcal/Temperature",&TestUpdateMgr::i_updateMethod3);
    registerCondition("Conditions/Environment/Hcal/Temperature",&TestUpdateMgr::i_updateMethod3);
    registerCondition("Conditions/Environment/Hcal/Temperature",&TestUpdateMgr::i_updateMethod2);
    
    registerCondition("Conditions/Environment/DummyDE/Temperature",&TestUpdateMgr::i_updateMethod3);
    
    updMgrSvc()->registerCondition(&m_intermediate,"Conditions/Environment/Hcal/Temperature",&InternalClass::myTinyMethod);
    
    registerCondition(&m_intermediate,&TestUpdateMgr::i_updateMethod4);

    // I'm crazy... but that must work!
    updMgrSvc()->registerCondition(m_dummyUMSentry);
    registerCondition<TestUpdateMgr>(m_dummyUMSentry,NULL);

  } catch (GaudiException) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestUpdateMgr::execute() {
  
  ++m_evtCount;
  debug() << "==> Execute (" << m_evtCount << ")" << endmsg;
  
  if ( m_evtCount == 3 ) {
    debug() << " invalidating m_intermediate" << endmsg;
    updMgrSvc()->invalidate(&m_intermediate);
    updMgrSvc()->invalidate(getDet<Condition>("Conditions/Environment/Hcal/Temperature"));
  }
  /*
  if ( m_evtCount == 3 ) {
    SmartIF<IDataManagerSvc> dm(detSvc());
    dm->clearStore();
    //updMgrSvc()->purge();
    SmartIF<IUMSPurge> p(updMgrSvc());
    p->purge();
    updMgrSvc()->dump();
  }
  */
  
  return StatusCode::SUCCESS;
}

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
