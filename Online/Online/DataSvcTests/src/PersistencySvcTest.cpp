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

// Class include file
#include "DataSvcTests/PersistencySvcTest.h"
#include "Factories.h"

//======================================================================
// Constructor
//======================================================================
PersistencySvcTest::PersistencySvcTest(const std::string& n, bool dbg) 
: DataSvcTest(n, dbg)   
{
  std::string nam;
  IInterface* pIF;
  m_pICV = 0;
  m_pISvcCnv = 0;

  StatusCode status = m_svcLoc->getService("TmpConversionSvc", m_pISvcCnv, true);
  if ( !status.isSuccess() )   {
    std::cout << "Service: has NO TmpConversionSvc. -- Giving up" << std::endl;    
    m_nerr++;
    return;
  }
  // ask for interfaces
  // IID_IInterface
  status = m_pISvcCnv->queryInterface(IInterface::interfaceID(), (void**)&pIF);
  if ( status.isSuccess() )   {
    if ( m_debug ) std::cout << "Service:" << nam << " has IID_IInterface." << std::endl;    
  }
  else    {
    std::cout << "Service:" << nam << " has NO IInterface." << std::endl;
    m_nerr++;
  }
  // IID_IConversionSvc
  status = pIF->queryInterface(IConversionSvc::interfaceID(), (void**)&m_pICV);
  if ( status.isSuccess() )   {
    if ( m_debug ) std::cout << "Service:" << nam << " has IID_IConversionSvc." << std::endl;
  }
  else    {
    std::cout << "Service:" << nam << " has NO IID_IConversionSvc." << std::endl;
    m_nerr++;
  }
  m_pICV->setDataProvider ( m_pIDP );
  pIF->release();
  if ( m_debug )    {
    std::cout << "+===============================================================+" << std::endl;
    std::cout << "|          PersistencySvcTest initialized                       |" << std::endl;
    std::cout << "+===============================================================+" << std::endl;
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
    //std::cout << "ERROR: Memory leak in releasing interface!!!" << std::endl;
  }
  if ( m_debug )    {
    std::cout << "Release:" << count << std::flush << std::endl;    // release ConversionsSvc
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
    std::cout << " m_pICV->addConverter( <ConcreteConverter> ) >>";
    PrintCode(std::cout, status, 0);
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
    std::cout << " m_pIDM->setDataLoader( loader )" << std::flush;
    PrintCode(std::cout, status, 0);
  }
  status = loader->setDataProvider( m_pIDP );
  if ( check(status, StatusCode::SUCCESS) )      {
    std::cout << " loader->setStore( m_pIDP ) >>";
    PrintCode(std::cout, status, 0);
  }
  return status;
}
  
