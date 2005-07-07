// $Id: TestCacheInjection.cpp,v 1.2 2005-07-07 12:32:29 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "DetDesc/Condition.h"
#include "DetDesc/ICondDBAccessSvc.h"
#include "DetDesc/IUpdateManagerSvc.h"

// local
#include "TestCacheInjection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestCacheInjection
//
// 2005-06-20 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TestCacheInjection>          s_factory ;
const        IAlgFactory& TestCacheInjectionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestCacheInjection::TestCacheInjection( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_dbAccSvc(NULL), m_evtCount(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
TestCacheInjection::~TestCacheInjection() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestCacheInjection::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    // Locate the Database Access Service
    m_dbAccSvc = svc<ICondDBAccessSvc>("CondDBAccessSvc");

    // Store sample data if the database is empty
    info() << "Inject data into the cache" << endreq;
    sc = i_injectData();
    if ( !sc.isSuccess() ) return sc;
    
    // Locate the Update Manager Service
    m_ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);

    info() << "*** register conditions ***" << endreq;
    m_ums->registerCondition(this,"/dd/CacheTest/Object1",NULL);
    m_ums->registerCondition(this,"/dd/CacheTest/Object2",NULL);
    m_ums->update(this);
  }
  catch (GaudiException) {
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestCacheInjection::execute() {

  //  debug() << "==> Execute" << endmsg;
  info() << "*************** execute(): process new event ***************" << endmsg;
  info() << "Event " << ++m_evtCount << endmsg;

  if ( m_evtCount == 2 ) { // second event
    // add a new object to folder 2 to be ready for next event
    
    TimePoint
      new_since(20),
      new_until(time_absolutefuture);
    
    // **************************************************
    info() << "Try to add a new condition to the cache" << endmsg;
    Condition testCond;
    testCond.addParam<std::string>("data","object 2 new");
    if (!m_dbAccSvc->cacheAddXMLObject("/test/cache/folder2",new_since,new_until,testCond.toXml("Object2"))) {
      return StatusCode::FAILURE;
    }

    // **************************************************
    info() << "Tell the update manager of the change I made" << endmsg;
    TimePoint s,u;
    if (m_ums->getValidity("/test/cache/folder2",s,u,true)){ // UMS knows the object
      info() << "old validity = " << s << " - " << u << endmsg;
      bool has_to_change = false;
      if ( u > new_since ) {
        u = new_since;
        has_to_change = true;
      }
      info() << "new validity = " << s << " - " << u << endmsg;
      if ( has_to_change ) m_ums->setValidity("/test/cache/folder2",s,u,true);
    } else {
      error() << "I cannot find the CondDB folder in UMS" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  SmartDataPtr<Condition> cond1( detSvc(), "/dd/CacheTest/Object1" );
  SmartDataPtr<Condition> cond2( detSvc(), "/dd/CacheTest/Object2" );

  info() << "Object1: " << cond1->validSince() << " -> " << cond1->validTill() << endmsg;
  info() << "         data = " << cond1->paramAsString("data") << endmsg;
  
  info() << "Object2: " << cond2->validSince() << " -> " << cond2->validTill() << endmsg;
  info() << "         data = " << cond2->paramAsString("data") << endmsg;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestCacheInjection::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Inject test data into the cache
//=========================================================================
StatusCode TestCacheInjection::i_injectData() {

  // add a couple of folders
  if (!m_dbAccSvc->cacheAddXMLFolder("/test/cache/folder1")) return StatusCode::FAILURE;
  if (!m_dbAccSvc->cacheAddXMLFolder("/test/cache/folder2")) return StatusCode::FAILURE;

  
  // then the objects
  Condition testCond;
  testCond.addParam<std::string>("data","object 1.a");

  if (!m_dbAccSvc->cacheAddXMLObject("/test/cache/folder1",0,10,testCond.toXml("Object1"))) {
    return StatusCode::FAILURE;
  }
  
  testCond.param<std::string>("data") = "object 1.b";
  if (!m_dbAccSvc->cacheAddXMLObject("/test/cache/folder1",10,time_absolutefuture,testCond.toXml("Object1"))) {
    return StatusCode::FAILURE;
  }
  
  testCond.param<std::string>("data") = "object 2";
  if (!m_dbAccSvc->cacheAddXMLObject("/test/cache/folder2",0,time_absolutefuture,testCond.toXml("Object2"))) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
