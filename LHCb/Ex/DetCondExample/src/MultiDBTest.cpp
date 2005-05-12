// $Id: MultiDBTest.cpp,v 1.1 2005-05-12 16:34:22 marcocle Exp $
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
  
  // Locate the Database Access Service 1
  sc = serviceLocator()->service("DB1",m_dbAccSvc1);
  if (  !sc.isSuccess() ) {
    error() << "Could not locate DB1" << endreq;
    return sc;
  }
  // Locate the Database Access Service 2
  sc = serviceLocator()->service("DB2",m_dbAccSvc2);
  if (  !sc.isSuccess() ) {
    error() << "Could not locate DB2" << endreq;
    return sc;
  }

  // Get a pointer to the DBs to speed up a bit
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
  IUpdateManagerSvc *ums;
  sc = serviceLocator()->service("UpdateManagerSvc",ums,true);
  if (!sc.isSuccess()) {
    error() << "Unable to find UpdateManagerSvc" <<endmsg;
    return sc;
  }
  ums->registerCondition(this,"/dd/multiDBTest/Cond1",NULL);
  ums->registerCondition(this,"/dd/multiDBTest/Cond2",NULL);
  ums->update(this);
  ums->release();

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

  m_dbAccSvc1->release();
  m_dbAccSvc2->release();

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
