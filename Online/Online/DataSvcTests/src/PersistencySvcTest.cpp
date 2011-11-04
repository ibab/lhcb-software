//====================================================================
//	PersistencySvcTest test class implementation
//--------------------------------------------------------------------
//
//	Package    : PersistencySvc ( The LHCb Offline System)
//
//  Description: Test the PersistencySvc functionality
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
#define PERSISTENCYSVC_PERSISTENCYSVCTEST_CPP

// C/C++ include files
#include <iostream>
#include <stdexcept>

// Class include file
#include "DataSvcTests/PersistencySvcTest.h"
#include "Factories.h"

using namespace Tests;
using namespace std;

//======================================================================
// Constructor
//======================================================================
PersistencySvcTest::PersistencySvcTest(const string& n, bool dbg) 
: DataSvcTest(n, dbg)   
{
  string nam;
  IInterface* pIF;
  m_pICV = 0;
  m_pISvcCnv = 0;

  StatusCode status = m_svcLoc->getService("Tests::TmpConversionSvc", m_pISvcCnv, true);
  if ( !status.isSuccess() )   {
    m_nerr++;
    cout << "Service: has NO Tests::TmpConversionSvc. -- Giving up" << endl;    
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  // ask for interfaces
  // IID_IInterface
  status = m_pISvcCnv->queryInterface(IInterface::interfaceID(), (void**)&pIF);
  if ( status.isSuccess() )   {
    if ( m_debug ) cout << "Service:" << nam << " has IID_IInterface." << endl;    
  }
  else    {
    m_nerr++;
    cout << "Service:" << nam << " has NO IInterface." << endl;
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  // IID_IConversionSvc
  status = pIF->queryInterface(IConversionSvc::interfaceID(), (void**)&m_pICV);
  if ( status.isSuccess() )   {
    if ( m_debug ) cout << "Service:" << nam << " has IID_IConversionSvc." << endl;
  }
  else    {
    m_nerr++;
    cout << "Service:" << nam << " has NO IID_IConversionSvc." << endl;
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  m_pICV->setDataProvider ( m_pIDP );
  pIF->release();
  if ( m_debug )    {
    cout << "+===============================================================+" << endl;
    cout << "|          PersistencySvcTest initialized                       |" << endl;
    cout << "+===============================================================+" << endl;
  }
}

//======================================================================
// Destructor
//======================================================================
PersistencySvcTest::~PersistencySvcTest()   {
}

//======================================================================
// Shutdown
//======================================================================
void PersistencySvcTest::shutdown()   {
  DataSvcTest::finalize();
  unsigned long count = m_pICV->release();
  if ( 0 != count )   {
    //cout << "ERROR: Memory leak in releasing interface!!!" << endl;
  }
  if ( m_debug )    {
    cout << "Release:" << count << flush << endl;    // release ConversionsSvc
  }
  DataSvcTest::shutdown();
}

//======================================================================
// Action method
//======================================================================
void PersistencySvcTest::run()   
{
  DataSvcTest::run();
}

//======================================================================
// Add converter to a conversion service
//======================================================================
StatusCode PersistencySvcTest::addConverter(const ICnvFactory& fac)   {
  IConverter* cnv = fac.create(0);
  StatusCode status = m_pICV->addConverter( cnv );
  if ( check(status, StatusCode::SUCCESS) )      {
    cout << " m_pICV->addConverter( <ConcreteConverter> ) >>";
    PrintCode(cout, status, 0);
    if ( !status.isSuccess() ) m_nerr++;
  }
  return status;
}

//======================================================================
// Set Data loader object
//======================================================================
StatusCode PersistencySvcTest::testDataLoader(IConversionSvc* loader)    {
  StatusCode status = m_pIDM->setDataLoader( loader );
  if ( check(status, StatusCode::SUCCESS) )      {
    cout << " m_pIDM->setDataLoader( loader )" << flush;
    PrintCode(cout, status, 0);
  }
  status = loader->setDataProvider( m_pIDP );
  if ( check(status, StatusCode::SUCCESS) )      {
    cout << " loader->setStore( m_pIDP ) >>";
    PrintCode(cout, status, 0);
  }
  return status;
}
  
