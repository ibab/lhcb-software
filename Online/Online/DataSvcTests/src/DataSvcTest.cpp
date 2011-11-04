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
#define DATASVC_DATASVCTEST_CPP
// C/C++ include files
#include <ctime>
#include <iostream>
#include <stdexcept>

// Framework include files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/RegistryEntry.h"

#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartIF.h"

// Class include file
#include "DataSvcTests/DataSvcTest.h"
#include "Agent.h"

#include "GaudiKernel/DataIncident.h"

#include "Hit.h"

using namespace std;
using namespace Tests;

namespace Tests {
  IDataProviderSvc* s_pDataSvc = 0;
  IDataProviderSvc* dataSvc() {  return s_pDataSvc; }
}

//======================================================================
// Constructor
//======================================================================
DataSvcTest::DataSvcTest(const string& nam, bool dbg)
: UnitTest(nam, dbg)
{
  // Test generic Persistency service
  IInterface* pIF;
  m_pIS  = 0;
  m_pPrp = 0;
  m_pIDP = 0;
  m_pIDM = 0;

  // Create object instance
  StatusCode status = m_svcLoc->getService("DataSvc", m_pIS, true);
  if ( !status.isSuccess() )   {
    m_nerr++;
    cout << "Service: has NO DataSvc. -- Giving up" << endl;    
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  // IID_IInterface
  status = m_pIS->queryInterface(IInterface::interfaceID(), (void**)&pIF);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << m_pIS->name() << " has NO IID_IInterface. " << endl;
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  else if ( m_debug )   {
    cout << "Service:" << m_pIS->name() << " has IID_IInterface." << endl;    
  }

  // IID_IDataManager
  status = m_pIS->queryInterface(IDataManagerSvc::interfaceID(), (void**)&m_pIDM);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << m_pIS->name() << " has NO IID_IDataManagerSvc. " << endl;
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  else if ( m_debug )   {
    cout << "Service:" << m_pIS->name() << " has IID_IDataManagerSvc." << endl;    
  }

  // IID_IDataProviderSvc
  status = m_pIS->queryInterface(IDataProviderSvc::interfaceID(), (void**)&m_pIDP);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << m_pIS->name() << " has NO IID_IDataProviderSvc. " << endl;    
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  else if ( m_debug )   {
    cout << "Service:" << m_pIS->name() << " has IID_IDataProviderSvc." << endl;
  }
  // IID_IProperty
  status = m_pIS->queryInterface(IProperty::interfaceID(), (void**)&m_pPrp);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service:" << m_pIS->name() << " has NO IID_IProperty. " << endl;    
    throw runtime_error("Fatal exception. cannot continue to run.");
  }
  else if ( m_debug )   {
    cout << "Service:" << m_pIS->name() << " has IID_IProperty." << endl;
  }
  s_pDataSvc = m_pIDP;
  IProperty* pSvc = 0;
  status = m_svcLoc->service("DataOnDemandSvc", pSvc, true);
  if ( status.isSuccess() )    {
    SmartIF<IProperty> iprp(pSvc);
    iprp->setProperty("DataSvc","DataSvc");
    iprp->setProperty("IncidentName",   "TestDataFault");
    iprp->setProperty("AlgMap","['/Event/2/21/211/2115/21x15/21xx15/21xxx15' : 'Tests::IncidentAlgorithm']");
    SmartIF<IService> isvc(pSvc);
    if ( isvc.isValid() )  {
      status = isvc->reinitialize();
    }
  }
#if 0
  status = m_svcLoc->service("IncidentSvc", m_pIncidentSvc, true);
  if ( !status.isSuccess() )    {
    m_nerr++;
    cout << "Service: IncidentSvc not found. " << endl;    
  }
  else if ( m_debug )   {
    cout << "Service: IncidentSvc found." << endl;
  }
  if ( status.isSuccess() )    {
    IncidentAlgorithm* listener = new IncidentAlgorithm("IncidentListener", m_svcLoc);
    listener->setDataSvc(m_pIDP);
    m_pIncidentSvc->addListener(listener,"TestDataFault");
    m_pIncidentSvc->addListener(listener,"TestDataAccess");
  }
#endif
  if ( m_debug )    {
    cout << "+===============================================================+" << endl;
    cout << "|             DataSvcTest initialized                           |" << endl;
    cout << "+===============================================================+" << endl;
  }
  pIF->release();
}

//======================================================================
// Destructor
//======================================================================
DataSvcTest::~DataSvcTest()    {
}

//======================================================================
// Shutdown
//======================================================================
void DataSvcTest::shutdown()   {
  StatusCode status = testClearTree(IDataProviderSvc::INVALID_ROOT,"");
  UnitTest::finalize();
  unsigned long count = m_pIDP->release();
  count = m_pIDM->release();
  count = m_pPrp->release();
  if ( 0 != count )   {
    //cout << "Memory leak in releasing interfaces!" << endl;
  }
  UnitTest::shutdown();
}


//======================================================================
// Interprete and print return codes
//======================================================================
ostream& DataSvcTest::PrintCode(ostream& s, const StatusCode& status, DataObject* pObject)   {
  s << endl << "      => Result:";
  switch( status.getCode() )    {
  /// The path for this objects is already in use
  case IDataProviderSvc::DOUBL_OBJ_PATH:
    s << "DOUBL_OBJ_PATH  ";
    break;
  /// Invalid path from root to object request failed.
  case IDataProviderSvc::INVALID_OBJ_PATH:
    s << "INVALID_OBJ_PATH";
    break;
  /// Invalid root path object cannot be retrieved or stored.
  case IDataProviderSvc::INVALID_ROOT:
    s << "INVALID_ROOT    ";
    break;
  /// Object pointer is invalid
  case IDataProviderSvc::INVALID_OBJECT:
    s << "INVALID_OBJECT  ";
    break;
  /// Object pointer is invalid
  case IDataProviderSvc::INVALID_PARENT:
    s << "INVALID_PARENT  ";
    break;
  case IDataProviderSvc::OBJ_NOT_LOADED:
    s << "OBJ_NOT_LOADED  ";
    break;
  case IDataProviderSvc::NO_DATA_LOADER:
    s << "NO_DATA_LOADER  ";
    break;
  case IDataProviderSvc::INVALID_OBJ_ADDR:
    s << "INVALID_OBJ_ADDR";
    break;
  case IDataProviderSvc::DIR_NOT_EMPTY:
    s << "DIR_NOT_EMPTY   ";
    break;
  case StatusCode::SUCCESS:
    s << "SUCCESS         ";
    break;
  case StatusCode::FAILURE:
    s << "FAILURE         ";
    break;
  default:
    s << "???????         ";
    break;
  }
  s << "  -> Object:" << objectName(pObject) << " ObjPath:" << fullpath(pObject) << endl;
  return s;
}
//======================================================================
// Analyse directory entry
//======================================================================
StatusCode DataSvcTest::testObjDirectory(DataObject* pObject)    {
  if ( 0 != pObject )   {
    typedef vector<IRegistry*> Leafs;
    Leafs leafs;
    StatusCode status=m_pIDM->objectLeaves(pObject, leafs);
    if ( m_debug )    {
      cout << "Object:" << objectName(pObject) << endl;
    }
    for ( Leafs::iterator dit = leafs.begin(); dit != leafs.end(); dit++ )    {
      IRegistry* dd = *dit;
      const string& nam = dd->name();
      if ( m_debug )    {
        cout << "       -> Directory entry ";
        cout << (0==dd->object() ? "(UNLOADED)  " : "(LOADED)    ");
        cout << nam << endl;
      }
    }
    return status;
  }
  cout << "Object is NIL!" << endl;
  return StatusCode::FAILURE;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           DataObject* pNode, 
                                           const string& name, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " registerObject (by node/name:" << fullpath(pNode) << ", " << name << ") >>";
    }
    else  {
      cout << " registerObject (by node/name: ..., " << name << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           DataObject* pNode, 
                                           int id, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(pNode, id, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " registerObject (by node/id:" << fullpath(pNode) << ", " << id << ") >>";
    }
    else  {
      cout << " registerObject (by node/id: ..., " << id << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           const string& name, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(name, pObject);
  if ( check(status, expected) )      {
    cout << " registerObject (by path:" << name << ") >>";
    if ( 0 != pObject )   {
      cout << " ( regPath=" << fullpath(pObject) << ")>>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           const string& parPath, 
                                           const string& objPath, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(parPath, objPath, pObject);
  if ( check(status, expected) )      {
    cout << " registerObject (by parPath, objPath:" 
              << parPath << ", " << objPath << ") >>";
    if ( 0 != pObject )   {
      cout << " ( regPath=" << fullpath(pObject) << ")>>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test findObject
//======================================================================
StatusCode DataSvcTest::testFindObject(long expected, const string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(name, pObject);
  if ( check(status, expected) )      {
    cout << " findObject (by path:" << name << ") >>";
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test findObject
//======================================================================
StatusCode DataSvcTest::testFindObject(long expected, DataObject* pNode, const string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " findObject (by node/name:" << fullpath(pNode) << ", " << name << ") >>";
    }
    else  {
      cout << " findObject (by node/name: ..., " << name << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test FindObj by parent path, object path
//======================================================================
StatusCode DataSvcTest::testFindObject(   long expected, 
                                          const string& parPath,
                                          const string& objPath)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(parPath, objPath, pObject);
  if ( check(status, expected) )      {
    cout << " findObject (by parPath/objPath:" << parPath << ", " << objPath << ") >>";
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test FindObj by parent path, object id
//======================================================================
StatusCode DataSvcTest::testFindObject(   long expected, 
                                          const string& parPath,
                                          long objID)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(parPath, objID, pObject);
  if ( check(status, expected) )      {
    cout << " findObject (by parPath/id:" << parPath << ", " << objID << ") >>";
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test findObject
//======================================================================
StatusCode DataSvcTest::testFindObject(long expected, DataObject* pNode, int id)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(pNode, id, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " findObject (by node/id:" << fullpath(pNode) << ", " << id << ") >>";
    }
    else  {
      cout << " findObject (by node/id: ..., " << id << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test unregisterObject
//======================================================================
StatusCode DataSvcTest::testUnregisterObject(long expected, const string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(name, pObject);
  status = m_pIDP->unregisterObject(name);
  if ( status.isSuccess() )   {
    delete pObject;
  }
  if ( check(status, expected) )      {
    cout << " unregisterObject (by string:" << name << ") ";
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test unregisterObject
//======================================================================
StatusCode DataSvcTest::testUnregisterObject(long expected, DataObject* pObj)    {
  string full = fullpath(pObj);
  StatusCode status = m_pIDP->unregisterObject(pObj);
  if ( status.isSuccess() )   {
    delete pObj;
  }
  if ( check(status, expected) )      {
    if ( 0 != pObj && 0 != pObj->registry() )    {
      cout << " unregisterObject (by object:" << full << ") >>";
    }
    else  {
      cout << " unregisterObject (by object:" << full << ") >>";
    }
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test retrieveObject by name
//======================================================================
StatusCode DataSvcTest::testRetrieveObject(long expected, const string& name)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(name, pObject);
  if ( check(status, expected) )      {
    cout << " retrieveObject (by path  :" << name << ") >>";
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test retrieveObject by name/object
//======================================================================
StatusCode DataSvcTest::testRetrieveObject(long expected, DataObject* pNode, const string& name)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " retrieveObject (by node/name:" << pNode->name() << ", " << name << ") >>";
    }
    else  {
      cout << " retrieveObject (by node/name  :..., " << name << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test retrieveObject by name/object
//======================================================================
StatusCode DataSvcTest::testRetrieveObject(long expected, DataObject* pNode, int id)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(pNode, id, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " retrieveObject (by node/id:" << pNode->name() << ", " << id << ") >>";
    }
    else  {
      cout << " retrieveObject (by node/id:..., " << id << ") >>";
    }
    PrintCode(cout, status, pObject);
  }
  return status;
}

// Test 
StatusCode DataSvcTest::testLinkObject(long expected, const string& from, const string& to)   {
  DataObject* pObject = 0, *pO = 0;
  StatusCode status = m_pIDP->retrieveObject(from, pObject);
  if ( status.isSuccess() )   {
    status = m_pIDP->linkObject(to, pObject);
    if ( status.isSuccess() )   {
    }
  }
  if ( check(status, expected) )      {
    cout << " testLinkObject (from, to:" << from << ", " << to << ") >>";
    m_pIDP->retrieveObject(to, pO);
    if ( pObject )
      cout << " " << fullpath(pObject);
    else
      cout << " INVALID SOURCE ";
    if ( pO )
      cout << " " << fullpath(pO) << endl;
    else
      cout << " INVALID TARGET " << endl;
    PrintCode(cout, status, pObject);
  }
  return status;
}
// Test 
StatusCode DataSvcTest::testLinkObject(long expected, DataObject* from, const string& to)   {
  DataObject* pObject = 0, *pO = 0;
  StatusCode status = m_pIDP->linkObject(to, from);
  if ( check(status, expected) )      {
    cout << " testLinkObject (from ( by Node), to:" << fullpath(from) << ", " << to << ") >>";
    m_pIDP->retrieveObject(to, pO);
    if ( pObject )
      cout << " " << fullpath(pObject);
    else
      cout << " INVALID SOURCE ";
    if ( pO )
      cout << " " << fullpath(pO) << endl;
    else
      cout << " INVALID TARGET " << endl;
    PrintCode(cout, status, pObject);
  }
  return status;
}

// Test 
StatusCode DataSvcTest::testUnlinkObject(long expected, const string& to)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->unlinkObject(to);
  if ( check(status, expected) )      {
    cout << " testUnlinkObject (name:" << to << ") >>";
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test TraverseTree
//======================================================================
StatusCode DataSvcTest::testTraverseTree(long expected, 
                                         const string& dir, 
                                         bool dbg)
{
  Agent agnt(dbg);
  IDataStoreAgent* pAgent = (IDataStoreAgent*)&agnt;
  StatusCode status = StatusCode::FAILURE;
  if ( dir == "" )    {
    status = m_pIDM->traverseTree(pAgent);
  }
  else    {
    status = m_pIDM->traverseSubTree(dir, pAgent);
  }
  if ( check(status, expected) )      {
    if ( dir == "" )    {
      cout << " traverseTree (pAgent) >>";
    }
    else    {
      cout << " traverseTree (" << dir << ", pAgent) >>";
    }
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test TraverseTree
//======================================================================
StatusCode DataSvcTest::testTraverseTree(long expected, 
                                         DataObject* pNode,
                                         bool dbg)
{
  Agent agnt(dbg);
  IDataStoreAgent* pAgent = (IDataStoreAgent*)&agnt;
  StatusCode status = m_pIDM->traverseSubTree(pNode, pAgent);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      cout << " traverseTree (" << pNode->name() << ", pAgent) >>";
    }
    else  {
      cout << " traverseTree (..., pAgent) >>";
    }
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test ClearTree
//======================================================================
StatusCode DataSvcTest::testClearTree(long expected, const string& dir)    {
  StatusCode status = StatusCode::FAILURE;
  if ( dir == "" )    {
    status = m_pIDM->clearStore();
  }
  else    {
    status = m_pIDM->clearSubTree(dir);
  }
  if ( check(status, expected) )      {
    if ( dir == "" )    {
      cout << " clearStore () >>" << endl;
    }
    else    {
      cout << " clearSubTree (" << dir << ") >>" << endl;
    }
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test ClearTree
//======================================================================
StatusCode DataSvcTest::testClearTree(long expected, DataObject* pNode)    {
  string path = "<Unknown>";
  if ( 0 != pNode ) path = pNode->name();
  StatusCode status = m_pIDM->clearSubTree(pNode);
  if ( check(status, expected) )      {
    cout << " clearSubTree (" << path << ") >>" << endl;
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Set Rot object
//======================================================================
StatusCode DataSvcTest::testSetRoot(long expected, const string& root, DataObject* pObject)    {
  StatusCode status = m_pIDM->setRoot(root, pObject);
  if ( check(status, expected) )      {
    cout << "setRoot(" << root << ", pObject)" << endl;
    PrintCode(cout, status, pObject);
  }
  return status;
}

//======================================================================
// Set Rot object by opaque address
//======================================================================
StatusCode DataSvcTest::testSetRoot(long expected, const string& root, IOpaqueAddress* pAddr)    {
  StatusCode status = m_pIDM->setRoot(root, pAddr);
  if ( check(status, expected) )      {
    cout << "setRoot(" << root << ", pAddr)" << endl;
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Add fake address to tree
//======================================================================
StatusCode DataSvcTest::registerAddress(long expected, const string& path, unsigned char typ, const CLID& clid)   {
  GenericAddress* pAddr = new GenericAddress(typ, clid);
  StatusCode status = m_pIDM->registerAddress(path, pAddr);
  if ( check(status, expected) )      {
    cout << "registerAddress(" << path << ")" << endl;
    PrintCode(cout, status, 0);
    //pAddr->release();
  }
  return status;
}

//======================================================================
// Add fake address to tree
//======================================================================
StatusCode DataSvcTest::registerAddress(long expected, const string& path, const string& sub_path, unsigned char typ, const CLID& clid)   {
  DataObject* pObject = 0;
  GenericAddress* pAddr = new GenericAddress(typ, clid);
  StatusCode status = m_pIDP->findObject(path, pObject);
  if ( status.isSuccess() )   {
    status = m_pIDM->registerAddress(pObject, sub_path, pAddr);
  }
  if ( check(status, expected) )      {
    cout << "registerAddress(" << path << ", " << sub_path << ")" << endl;
    PrintCode(cout, status, 0);
    //pAddr->release();
  }
  return status;
}

//======================================================================
// registerAddress fake address from tree
//======================================================================
StatusCode DataSvcTest::unregisterAddress(long expected, const string& path)   {
  StatusCode status = m_pIDM->unregisterAddress(path);
  if ( check(status, expected) )      {
    cout << "registerAddress(" << path << ")" << endl;
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// registerAddress fake address from tree
//======================================================================
StatusCode DataSvcTest::unregisterAddress(long expected, const string& path, const string& sub_path)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(path,pObject);
  if ( status.isSuccess() )   {
    status = m_pIDM->unregisterAddress(pObject, sub_path);
  }
  if ( check(status, expected) )      {
    cout << "unregisterAddress(" << path << ", " << sub_path << ")" << endl;
    PrintCode(cout, status, 0);
  }
  return status;
}

//======================================================================
// Test Timing: object find
//======================================================================
StatusCode DataSvcTest::testFindTiming(const string& path, int num_loop)   {
  longlong end, start = currentTime();
  DataObject *pObject;
  StatusCode status;
  int i = 0;
  for ( i = 0; i < num_loop; i++ )    {
    status = m_pIDP->findObject(path,pObject);
    if ( !status.isSuccess() )    {
      cout << "Error!!!!!" << endl;
      m_nerr++;
      return StatusCode::FAILURE;
    }
  }
  end = currentTime();
  if ( m_debug )    {
    cout << i << " Loops of m_pIDP->findObject(\"" << path << "\",pObject) took "
              << int((end-start)/1000) << " msec=" 
              << float(end-start)/float(i) << " muSec each" << endl;
  }
  return StatusCode::SUCCESS;
}
//======================================================================
// Test Timing: object find
//======================================================================
StatusCode DataSvcTest::testFindTiming(DataObject* pNode, const string& path, int num_loop)   {
  longlong end, start = currentTime();
  DataObject *pObject;
  StatusCode status;
  int i = 0;
  for ( i = 0; i < num_loop; i++ )    {
    status = m_pIDP->findObject(pNode,path,pObject);
    if ( !status.isSuccess() )    {
      cout << "Error!!!!!" << endl;
      m_nerr++;
      return StatusCode::FAILURE;
    }
  }
  end = currentTime();
  if ( m_debug )    {
    cout << i << " Loops of m_pIDP->findObject(\""
              << pNode->registry()->name()
              << "\", \""
              << path
              << "\", pObject) took "
              << int((end-start)/1000) << " msec=" 
              << float(end-start)/float(i) << " muSec each" << endl;
  }
  return StatusCode::SUCCESS;
}
