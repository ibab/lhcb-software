//====================================================================
//	DataSvc test class implementation
//--------------------------------------------------------------------
//
//	Package    : DataSvc ( The LHCb Offline System)
//
//  Description: Test the DataSvc functionality
//
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/10/98| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#define DATASVC_UnitTest_CPP

// Framework include files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IProperty.h"

// Class include file
#include "DataSvcTests/UnitTest.h"

// C/C++ include files
#include <ctime>
#include <iostream>

namespace Tests {  void init_root();  }

using namespace Tests;
using namespace std;

//======================================================================
// Constructor
//======================================================================
UnitTest::UnitTest(const string& nam, bool dbg)
: m_nerr(0), m_debug(dbg), m_name(nam)  {
  // Test generic Persistency service
  init_root();
  m_msgSvc = 0;
  m_appMgr = Gaudi::createApplicationMgrEx("GaudiSvc", "ApplicationMgr");
  SmartIF<ISvcLocator>   svcLoc (m_appMgr );
  SmartIF<IProperty>     propMgr(m_appMgr );
  Gaudi::setInstance(svcLoc);
  propMgr->setProperty("EventLoop",           "MinimalEventLoopMgr");
  propMgr->setProperty("HistogramPersistency","NONE");
  propMgr->setProperty("JobOptionsType",      "NONE");
  propMgr->setProperty("OutputLevel",         "4");
  propMgr->setProperty("AppName",             "");
  m_appMgr->configure();
  m_appMgr->initialize();
  m_svcLoc = svcLoc;
  cout << "+===============================================================+" << endl;
  cout << "|               " << name() << " TEST starting." << endl;
  cout << "+===============================================================+" << endl;
}

//======================================================================
// Destructor
//======================================================================
UnitTest::~UnitTest()    {
}


//======================================================================
// Finalize
//======================================================================
void UnitTest::finalize()   {
  m_appMgr->finalize();
}

//======================================================================
// Shutdown
//======================================================================
void UnitTest::shutdown()   {
  m_appMgr->finalize();
  m_appMgr->terminate();
  m_appMgr->release();
  if ( m_nerr == 0 )    {
    cout << m_name << ">>>>>>>>>>>>>>>>>>>> SUCCESS ... SUCCESS  <<<<<<<<<<<<<<<<<<<<<<<<" << endl;
  }
  else    {
    cout << m_name << ">>>>>>>>>>>>>>>>>>>> ERROR ....... ERROR  <<<<<<<<<<<<<<<<<<<<<<<<" << endl;
  }
  if ( m_debug )   {
    cout << "+===============================================================+" << endl;
    cout << "|               " << name() << " terminated" << endl;
    cout << "+===============================================================+" << endl;
  }
}

//======================================================================
// Action method
//======================================================================
void UnitTest::run()   {
}

//======================================================================
// Access to services
//======================================================================
StatusCode UnitTest::getService(const string& what, 
                                const InterfaceID& iid, 
                                void** ptr)    
{
  // Test generic Persistency service
  IInterface* pIF = 0;
  IService*   pIS = 0;
  IProperty*  pPrp = 0;
  StatusCode status = m_svcLoc->getService(what, pIS, true);
  if ( !status.isSuccess() )   {
    cout << "Service: has NO " << what << " service. -- Giving up" << endl;    
    m_nerr++;
    return StatusCode::FAILURE;
  }
  // IID_IInterface
  status = pIS->queryInterface(IInterface::interfaceID(), (void**)&pIF);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << pIS->name() << " has NO IID_IInterface. " << endl;
  }
  else if ( m_debug )   {
    cout << "Service:" << pIS->name() << " has IID_IInterface." << endl;    
  }
  // IID_IProperty
  status = pIS->queryInterface(IProperty::interfaceID(), (void**)&pPrp);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << pIS->name() << " has NO IID_IProperty. " << endl;    
  }
  else if ( m_debug )   {
    cout << "Service:" << pIS->name() << " has IID_IProperty." << endl;
  }
  // IID_IRndmGenSvc
  status = pIS->queryInterface(iid, ptr);
  pPrp->release();
  pIF->release();
  pIS->release();
  return status;
}

bool UnitTest::check(StatusCode& status, long expected)   {
  if ( expected != long(status.getCode()) )     {
    m_nerr++;
    cout << " FAILURE: ";
    return true;
  }
  else if ( m_debug )   {
    cout << " CORRECT: ";
  }
  return m_debug;
}

longlong UnitTest::currentTime()   {
  return System::cpuTime(System::microSec);
}

IMessageSvc* UnitTest::msgSvc() const    {
  return m_msgSvc;
}

string UnitTest::objectName(DataObject* pObject)    {
  if ( 0 != pObject )   {
    return pObject->name();
  }
  return " <<No Object>> ";
}

string UnitTest::fullpath(DataObject* pObject)   {
  if ( 0 != pObject )   {
    IRegistry* pReg = pObject->registry();
    if ( 0 != pReg )    {
      return pReg->identifier();
    }
  }
  return " <<No Directory>> ";
}
