// $Id: MultiDBTest.cpp,v 1.2 2005-06-23 13:44:01 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartDataPtr.h"

#include "DetDesc/Condition.h"
#include "DetDesc/IUpdateManagerSvc.h"

#include "DetCond/ICondDBAccessSvc.h"

// local
#include "MultiDBTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MultiDBTest
//
// 2005-05-12 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MultiDBTest>          s_factory ;
const        IAlgFactory& MultiDBTestFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultiDBTest::MultiDBTest( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{}
//=============================================================================
// Destructor
//=============================================================================
MultiDBTest::~MultiDBTest() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MultiDBTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  try {

    // Locate the Database Access Service 1
    m_dbAccSvc1 = svc<ICondDBAccessSvc>("DB1");

    // Locate the Database Access Service 2
    m_dbAccSvc2 = svc<ICondDBAccessSvc>("DB2");

    info() << "*** prepare databases ***" << endmsg;

    m_dbAccSvc1->createFolder("/multiDBTest/Folder1","",ICondDBAccessSvc::XML);

    m_dbAccSvc2->createFolder("/multiDBTest/Folder1","",ICondDBAccessSvc::XML);
    m_dbAccSvc2->createFolder("/multiDBTest/Folder2","",ICondDBAccessSvc::XML);

    m_dbAccSvc1->storeXMLString("/multiDBTest/Folder1","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond1\"><param name=\"Database\" type=\"other\">DB1</param>"
                                "</condition></DDDB>",TimePoint(0), TimePoint(10));
    m_dbAccSvc2->storeXMLString("/multiDBTest/Folder1","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond1\"><param name=\"Database\" type=\"other\">DB2</param>"
                                "</condition></DDDB>",TimePoint(0), TimePoint(20));
    m_dbAccSvc2->storeXMLString("/multiDBTest/Folder2","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond2\"><param name=\"Database\" type=\"other\">DB2</param>"
                                "</condition></DDDB>",TimePoint(0), TimePoint(20));

    info() << "*** register conditions ***" << endreq;
    IUpdateManagerSvc *ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);
    ums->registerCondition(this,"/dd/multiDBTest/Cond1",NULL);
    ums->registerCondition(this,"/dd/multiDBTest/Cond2",NULL);
    ums->update(this);

  }
  catch (GaudiException){
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MultiDBTest::execute() {

  info() << "*************** execute(): process new event ***************" << endmsg;

  SmartDataPtr<Condition> cond1( detSvc(), "/dd/multiDBTest/Cond1" );
  SmartDataPtr<Condition> cond2( detSvc(), "/dd/multiDBTest/Cond2" );
  
  info() << "Cond1: " << cond1->validSince() << " -> " << cond1->validTill() << endmsg;
  info() << "       DB = " << cond1->paramAsString("Database") << endmsg;
  
  info() << "Cond2: " << cond2->validSince() << " -> " << cond2->validTill() << endmsg;
  info() << "       DB = " << cond2->paramAsString("Database") << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MultiDBTest::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
