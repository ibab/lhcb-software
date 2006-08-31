// $Id: TestUMSError.cpp,v 1.2 2006-08-31 13:53:44 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "DetDesc/Condition.h"
#include "DetCond/ICondDBAccessSvc.h"

// local
#include "TestUMSError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestUMSError
//
// 2005-06-20 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestUMSError );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestUMSError::TestUMSError( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_dbAccSvc(NULL), m_evtCount(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
TestUMSError::~TestUMSError() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestUMSError::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    // Locate the Database Access Service
    m_dbAccSvc = svc<ICondDBAccessSvc>("CondDBAccessSvc",true);

    // Store sample data if the database is empty
    info() << "Inject data into the cache" << endreq;
    sc = i_injectData();
    if ( !sc.isSuccess() ) return sc;
    
    // Locate the Update Manager Service
    info() << "*** register conditions ***" << endreq;
    registerCondition<TestUMSError>("/dd/TestObject");

  }
  catch (GaudiException) {
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestUMSError::execute() {

  //  debug() << "==> Execute" << endmsg;
  info() << "*************** execute(): process new event ***************" << endmsg;
  info() << "Event " << ++m_evtCount << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestUMSError::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Inject test data into the cache
//=========================================================================
StatusCode TestUMSError::i_injectData() {

  // add a folder
  if (!m_dbAccSvc->cacheAddFolderSet("/dd","")) return StatusCode::FAILURE;
  if (!m_dbAccSvc->cacheAddXMLFolder("/dd/TestObject")) return StatusCode::FAILURE;
  Condition testCond;
  testCond.addParam<std::string>("data","object 1.a");
  if (!m_dbAccSvc->cacheAddXMLData("/dd/TestObject",0,10,testCond.toXml("TestObject"))) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
//=============================================================================
