// Include files

#include "DetDesc/Condition.h"
#include "DetCond/ICondDBAccessSvc.h"

// local
#include "TestCacheInjection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestCacheInjection
//
// 2005-06-20 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestCacheInjection )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestCacheInjection::TestCacheInjection( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_dbAccSvc(NULL), m_evtCount(0),
    m_cond1(NULL),
    m_cond2(NULL),
    m_cond3(NULL),
    m_cond4(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
TestCacheInjection::~TestCacheInjection() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestCacheInjection::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    // Locate the Database Access Service
    m_dbAccSvc = svc<ICondDBAccessSvc>("CondDBAccessSvc",true);

    // Store sample data if the database is empty
    info() << "Inject data into the cache" << endmsg;
    sc = i_injectData();
    if ( !sc.isSuccess() ) return sc;

    info() << "*** register conditions ***" << endmsg;
    registerCondition<TestCacheInjection>("CacheTest/Object1",m_cond1);
    registerCondition<TestCacheInjection>("CacheTest/Object2",m_cond2);
    registerCondition<TestCacheInjection>("CacheTest/Object3",m_cond3);
    registerCondition<TestCacheInjection>("CacheTest/Object4",m_cond4);
  }
  catch (GaudiException) {
    return StatusCode::FAILURE;
  }

  return runUpdate();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestCacheInjection::execute() {

  //  debug() << "==> Execute" << endmsg;
  info() << "*************** execute(): process new event ***************" << endmsg;
  info() << "Event " << ++m_evtCount << endmsg;

  if ( m_evtCount == 2 ) { // second event
    // add a new object to folder 2 to be ready for next event

    Gaudi::Time
      new_since(20),
      new_until(Gaudi::Time::max());

    // **************************************************
    info() << "Try to add a new condition to the cache" << endmsg;
    Condition testCond;
    testCond.addParam<std::string>("data","object 2 new");
    if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder2",new_since,new_until,testCond.toXml("Object2"))) {
      return StatusCode::FAILURE;
    }

    // **************************************************
    info() << "Tell the update manager of the change I made" << endmsg;
    Gaudi::Time s,u;
    if (updMgrSvc()->getValidity("/test/cache/folder2",s,u,true)){ // UMS knows the object
      info() << "old validity = " << s << " - " << u << endmsg;
      bool has_to_change = false;
      if ( u > new_since ) {
        u = new_since;
        has_to_change = true;
      }
      info() << "new validity = " << s << " - " << u << endmsg;
      if ( has_to_change ) updMgrSvc()->setValidity("/test/cache/folder2",s,u,true);
    } else {
      error() << "I cannot find the CondDB folder in UMS" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  info() << "Object1: " << m_cond1->validSince() << " -> " << m_cond1->validTill() << endmsg;
  info() << "         data = " << m_cond1->paramAsString("data") << endmsg;

  info() << "Object2: " << m_cond2->validSince() << " -> " << m_cond2->validTill() << endmsg;
  info() << "         data = " << m_cond2->paramAsString("data") << endmsg;

  info() << "Object3: " << m_cond3->validSince() << " -> " << m_cond3->validTill() << endmsg;
  info() << "         data = " << m_cond3->paramAsString("data") << endmsg;

  info() << "Object4: " << m_cond4->validSince() << " -> " << m_cond4->validTill() << endmsg;
  info() << "         data = " << m_cond4->paramAsString("data") << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Inject test data into the cache
//=========================================================================
StatusCode TestCacheInjection::i_injectData() {

  // add the main catalog entry
  if (!m_dbAccSvc->cacheAddXMLFolder("/lhcb.xml")) return StatusCode::FAILURE;
  if (!m_dbAccSvc->
      cacheAddXMLData("/lhcb.xml",
                      Gaudi::Time::epoch(),Gaudi::Time::max(),
                      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">\n"
                      "<DDDB> \n"
                      "  <catalog name=\"dd\">\n"
                      "    <catalog name=\"CacheTest\">\n"
                      "      <conditionref href=\"test/cache/folder1#Object1\"/>\n"
                      "      <conditionref href=\"test/cache/folder2#Object2\"/>\n"
                      "      <conditionref href=\"test/cache/folder3#Object3\"/>\n"
                      "      <conditionref href=\"test/cache/folder3:1#Object4\"/>\n"
                      "    </catalog>\n"
                      "  </catalog>\n"
                      "</DDDB>\n")) return StatusCode::FAILURE;

  // add a few of folders
  if (!m_dbAccSvc->cacheAddXMLFolder("/test/cache/folder1")) return StatusCode::FAILURE;
  if (!m_dbAccSvc->cacheAddXMLFolder("/test/cache/folder2")) return StatusCode::FAILURE;
  if (!m_dbAccSvc->cacheAddXMLFolder("/test/cache/folder3")) return StatusCode::FAILURE;

  // then the objects
  Condition testCond;
  testCond.addParam<std::string>("data","object 1.a");

  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder1",0,10,testCond.toXml("Object1"))) {
    return StatusCode::FAILURE;
  }

  testCond.param<std::string>("data") = "object 1.b";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder1",10,Gaudi::Time::max(),testCond.toXml("Object1"))) {
    return StatusCode::FAILURE;
  }

  testCond.param<std::string>("data") = "object 2";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder2",0,Gaudi::Time::max(),testCond.toXml("Object2"))) {
    return StatusCode::FAILURE;
  }

  testCond.param<std::string>("data") = "object 3.1";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder3",20,Gaudi::Time::max(),testCond.toXml("Object3"))) {
    return StatusCode::FAILURE;
  }
  testCond.param<std::string>("data") = "object 3.0";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder3",0,20,testCond.toXml("Object3"))) {
    return StatusCode::FAILURE;
  }
  testCond.param<std::string>("data") = "object 3.2";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder3",30,Gaudi::Time::max(),testCond.toXml("Object3"))) {
    return StatusCode::FAILURE;
  }

  testCond.param<std::string>("data") = "object 4";
  if (!m_dbAccSvc->cacheAddXMLData("/test/cache/folder3",0,Gaudi::Time::max(),testCond.toXml("Object4"),1)) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
