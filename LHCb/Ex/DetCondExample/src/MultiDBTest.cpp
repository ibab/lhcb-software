// $Id: MultiDBTest.cpp,v 1.8 2006-02-01 19:50:30 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DetDesc/Condition.h"
#include "DetCond/ICondDBAccessSvc.h"

// local
#include "MultiDBTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MultiDBTest
//
// 2005-05-12 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MultiDBTest );


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
    m_dbAccSvc1 = svc<ICondDBAccessSvc>("CondDBAccessSvc/DB1",true);

    // Locate the Database Access Service 2
    m_dbAccSvc2 = svc<ICondDBAccessSvc>("CondDBAccessSvc/DB2",true);

    info() << "*** prepare databases ***" << endmsg;

    m_dbAccSvc1->createFolder("/multiDBTest/Folder1","",ICondDBAccessSvc::XML);

    m_dbAccSvc2->createFolder("/multiDBTest/Folder1","",ICondDBAccessSvc::XML);
    m_dbAccSvc2->createFolder("/multiDBTest/Folder2","",ICondDBAccessSvc::XML);

    m_dbAccSvc1->storeXMLString("/multiDBTest/Folder1","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond1\"><param name=\"Database\" type=\"other\">DB1</param>"
                                "</condition></DDDB>",Gaudi::Time(0), Gaudi::Time(10));
    m_dbAccSvc2->storeXMLString("/multiDBTest/Folder1","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond1\"><param name=\"Database\" type=\"other\">DB2</param>"
                                "</condition></DDDB>",Gaudi::Time(0), Gaudi::Time(20));
    m_dbAccSvc2->storeXMLString("/multiDBTest/Folder2","<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                "<!DOCTYPE DDDB SYSTEM \"structure.dtd\">"
                                "<DDDB><condition name=\"Cond2\"><param name=\"Database\" type=\"other\">DB2</param>"
                                "</condition></DDDB>",Gaudi::Time(0), Gaudi::Time(20));

    info() << "*** register conditions ***" << endreq;
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond1",m_cond1);
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond2",m_cond2);

  }
  catch (GaudiException){
    return StatusCode::FAILURE;
  }
  return runUpdate();
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MultiDBTest::execute() {

  info() << "*************** execute(): process new event ***************" << endmsg;
  
  info() << "Cond1: " << m_cond1->validSince() << " -> " << m_cond1->validTill() << endmsg;
  info() << "       DB = " << m_cond1->paramAsString("Database") << endmsg;
  
  info() << "Cond2: " << m_cond2->validSince() << " -> " << m_cond2->validTill() << endmsg;
  info() << "       DB = " << m_cond2->paramAsString("Database") << endmsg;

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
