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
// Framework include files
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartIF.h"

#include "GaudiKernel/AlgFactory.h"
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

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/DataIncident.h"

#include "Hit.h"

using namespace Tests;

static IDataProviderSvc* s_pDataSvc = 0;

// External declarations
extern const ISvcFactory& DataServiceFactory;

class IncidentAlgorithm : public Algorithm  {
public:
  IncidentAlgorithm(const std::string& name, ISvcLocator *svcloc) 
    : Algorithm(name, svcloc) {
  }
  virtual ~IncidentAlgorithm() {
  }
  virtual StatusCode execute()  {
    MsgStream log(msgSvc(), name());
    log << MSG::ALWAYS << "Data-On-Demand trigger running for:"
        << "/Event/2/21/211/2115/21x15/21xx15/21xxx15" << endmsg;
    StatusCode sc = s_pDataSvc->registerObject("/Event/2/21/211/2115/21x15/21xx15/21xxx15",new Hit());
    return sc;
  }
};

DECLARE_ALGORITHM_FACTORY(IncidentAlgorithm)

//======================================================================
// Constructor
//======================================================================
DataSvcTest::DataSvcTest(const std::string& nam, bool dbg)
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
    std::cout << "Service: has NO DataSvc. -- Giving up" << std::endl;    
    m_nerr++;
    throw std::runtime_error("Fatal exception. cannot continue to run.");
  }
  // IID_IInterface
  status = m_pIS->queryInterface(IInterface::interfaceID(), (void**)&pIF);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service:" << m_pIS->name() << " has NO IID_IInterface. " << std::endl;
  }
  else if ( m_debug )   {
    std::cout << "Service:" << m_pIS->name() << " has IID_IInterface." << std::endl;    
  }

  // IID_IDataManager
  status = m_pIS->queryInterface(IDataManagerSvc::interfaceID(), (void**)&m_pIDM);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service:" << m_pIS->name() << " has NO IID_IDataManagerSvc. " << std::endl;
  }
  else if ( m_debug )   {
    std::cout << "Service:" << m_pIS->name() << " has IID_IDataManagerSvc." << std::endl;    
  }

  // IID_IDataProviderSvc
  status = m_pIS->queryInterface(IDataProviderSvc::interfaceID(), (void**)&m_pIDP);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service:" << m_pIS->name() << " has NO IID_IDataProviderSvc. " << std::endl;    
  }
  else if ( m_debug )   {
    std::cout << "Service:" << m_pIS->name() << " has IID_IDataProviderSvc." << std::endl;
  }
  // IID_IProperty
  status = m_pIS->queryInterface(IProperty::interfaceID(), (void**)&m_pPrp);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service:" << m_pIS->name() << " has NO IID_IProperty. " << std::endl;    
  }
  else if ( m_debug )   {
    std::cout << "Service:" << m_pIS->name() << " has IID_IProperty." << std::endl;
  }
  s_pDataSvc = m_pIDP;
  IProperty* pSvc = 0;
  status = m_svcLoc->service("DataOnDemandSvc", pSvc, true);
  if ( status.isSuccess() )    {
    SmartIF<IProperty> iprp(pSvc);
    std::vector<std::string> handlers;
    handlers.push_back("DATA='/Event/2/21/211/2115/21x15/21xx15/21xxx15' NAME='HitHandler' TYPE='IncidentAlgorithm'");
    iprp->setProperty("DataSvc","DataSvc");
    iprp->setProperty("Trap",   "TestDataFault");
    iprp->setProperty(StringArrayProperty("Algorithms",handlers));
    SmartIF<IService> isvc(pSvc);
    if ( isvc.isValid() )  {
      status = isvc->reinitialize();
    }
  }
#if 0
  status = m_svcLoc->service("IncidentSvc", m_pIncidentSvc, true);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service: IncidentSvc not found. " << std::endl;    
  }
  else if ( m_debug )   {
    std::cout << "Service: IncidentSvc found." << std::endl;
  }
  if ( status.isSuccess() )    {
    IncidentAlgorithm* listener = new IncidentAlgorithm("IncidentListener", m_svcLoc);
    listener->setDataSvc(m_pIDP);
    m_pIncidentSvc->addListener(listener,"TestDataFault");
    m_pIncidentSvc->addListener(listener,"TestDataAccess");
  }
#endif
  if ( m_debug )    {
    std::cout << "+===============================================================+" << std::endl;
    std::cout << "|             DataSvcTest initialized                           |" << std::endl;
    std::cout << "+===============================================================+" << std::endl;
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
    //std::cout << "Memory leak in releasing interfaces!" << std::endl;
  }
  UnitTest::shutdown();
}


//======================================================================
// Interprete and print return codes
//======================================================================
std::ostream& DataSvcTest::PrintCode(std::ostream& s, const StatusCode& status, DataObject* pObject)   {
  s << std::endl << "      => Result:";
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
  s << "  -> Object:" << objectName(pObject) << " ObjPath:" << fullpath(pObject) << std::endl;
  return s;
}
//======================================================================
// Analyse directory entry
//======================================================================
StatusCode DataSvcTest::testObjDirectory(DataObject* pObject)    {
  if ( 0 != pObject )   {
    typedef std::vector<IRegistry*> Leafs;
    Leafs leafs;
    StatusCode status=m_pIDM->objectLeaves(pObject, leafs);
    if ( m_debug )    {
      std::cout << "Object:" << objectName(pObject) << std::endl;
    }
    for ( Leafs::iterator dit = leafs.begin(); dit != leafs.end(); dit++ )    {
      IRegistry* dd = *dit;
      const std::string& nam = dd->name();
      if ( m_debug )    {
        std::cout << "       -> Directory entry ";
        std::cout << (0==dd->object() ? "(UNLOADED)  " : "(LOADED)    ");
        std::cout << nam << std::endl;
      }
    }
    return status;
  }
  std::cout << "Object is NIL!" << std::endl;
  return StatusCode::FAILURE;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           DataObject* pNode, 
                                           const std::string& name, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      std::cout << " registerObject (by node/name:" << fullpath(pNode) << ", " << name << ") >>";
    }
    else  {
      std::cout << " registerObject (by node/name: ..., " << name << ") >>";
    }
    PrintCode(std::cout, status, pObject);
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
      std::cout << " registerObject (by node/id:" << fullpath(pNode) << ", " << id << ") >>";
    }
    else  {
      std::cout << " registerObject (by node/id: ..., " << id << ") >>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           const std::string& name, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(name, pObject);
  if ( check(status, expected) )      {
    std::cout << " registerObject (by path:" << name << ") >>";
    if ( 0 != pObject )   {
      std::cout << " ( regPath=" << fullpath(pObject) << ")>>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test RegisterObject
//======================================================================
StatusCode DataSvcTest::testRegisterObject(long expected, 
                                           const std::string& parPath, 
                                           const std::string& objPath, 
                                           DataObject* pObject)    {
  StatusCode status = m_pIDP->registerObject(parPath, objPath, pObject);
  if ( check(status, expected) )      {
    std::cout << " registerObject (by parPath, objPath:" 
              << parPath << ", " << objPath << ") >>";
    if ( 0 != pObject )   {
      std::cout << " ( regPath=" << fullpath(pObject) << ")>>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test findObject
//======================================================================
StatusCode DataSvcTest::testFindObject(long expected, const std::string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(name, pObject);
  if ( check(status, expected) )      {
    std::cout << " findObject (by path:" << name << ") >>";
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test findObject
//======================================================================
StatusCode DataSvcTest::testFindObject(long expected, DataObject* pNode, const std::string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      std::cout << " findObject (by node/name:" << fullpath(pNode) << ", " << name << ") >>";
    }
    else  {
      std::cout << " findObject (by node/name: ..., " << name << ") >>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test FindObj by parent path, object path
//======================================================================
StatusCode DataSvcTest::testFindObject(   long expected, 
                                          const std::string& parPath,
                                          const std::string& objPath)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(parPath, objPath, pObject);
  if ( check(status, expected) )      {
    std::cout << " findObject (by parPath/objPath:" << parPath << ", " << objPath << ") >>";
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test FindObj by parent path, object id
//======================================================================
StatusCode DataSvcTest::testFindObject(   long expected, 
                                          const std::string& parPath,
                                          long objID)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(parPath, objID, pObject);
  if ( check(status, expected) )      {
    std::cout << " findObject (by parPath/id:" << parPath << ", " << objID << ") >>";
    PrintCode(std::cout, status, pObject);
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
      std::cout << " findObject (by node/id:" << fullpath(pNode) << ", " << id << ") >>";
    }
    else  {
      std::cout << " findObject (by node/id: ..., " << id << ") >>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test unregisterObject
//======================================================================
StatusCode DataSvcTest::testUnregisterObject(long expected, const std::string& name)    {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(name, pObject);
  status = m_pIDP->unregisterObject(name);
  if ( status.isSuccess() )   {
    delete pObject;
  }
  if ( check(status, expected) )      {
    std::cout << " unregisterObject (by string:" << name << ") ";
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Test unregisterObject
//======================================================================
StatusCode DataSvcTest::testUnregisterObject(long expected, DataObject* pObj)    {
  std::string full = fullpath(pObj);
  StatusCode status = m_pIDP->unregisterObject(pObj);
  if ( status.isSuccess() )   {
    delete pObj;
  }
  if ( check(status, expected) )      {
    if ( 0 != pObj && 0 != pObj->registry() )    {
      std::cout << " unregisterObject (by object:" << full << ") >>";
    }
    else  {
      std::cout << " unregisterObject (by object:" << full << ") >>";
    }
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Test retrieveObject by name
//======================================================================
StatusCode DataSvcTest::testRetrieveObject(long expected, const std::string& name)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(name, pObject);
  if ( check(status, expected) )      {
    std::cout << " retrieveObject (by path  :" << name << ") >>";
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test retrieveObject by name/object
//======================================================================
StatusCode DataSvcTest::testRetrieveObject(long expected, DataObject* pNode, const std::string& name)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->retrieveObject(pNode, name, pObject);
  if ( check(status, expected) )      {
    if ( pNode != 0 && pNode->registry() != 0 )   { // lazy evaluation....
      std::cout << " retrieveObject (by node/name:" << pNode->name() << ", " << name << ") >>";
    }
    else  {
      std::cout << " retrieveObject (by node/name  :..., " << name << ") >>";
    }
    PrintCode(std::cout, status, pObject);
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
      std::cout << " retrieveObject (by node/id:" << pNode->name() << ", " << id << ") >>";
    }
    else  {
      std::cout << " retrieveObject (by node/id:..., " << id << ") >>";
    }
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

// Test 
StatusCode DataSvcTest::testLinkObject(long expected, const std::string& from, const std::string& to)   {
  DataObject* pObject = 0, *pO = 0;
  StatusCode status = m_pIDP->retrieveObject(from, pObject);
  if ( status.isSuccess() )   {
    status = m_pIDP->linkObject(to, pObject);
    if ( status.isSuccess() )   {
    }
  }
  if ( check(status, expected) )      {
    std::cout << " testLinkObject (from, to:" << from << ", " << to << ") >>";
    m_pIDP->retrieveObject(to, pO);
    if ( pObject )
      std::cout << " " << fullpath(pObject);
    else
      std::cout << " INVALID SOURCE ";
    if ( pO )
      std::cout << " " << fullpath(pO) << std::endl;
    else
      std::cout << " INVALID TARGET " << std::endl;
    PrintCode(std::cout, status, pObject);
  }
  return status;
}
// Test 
StatusCode DataSvcTest::testLinkObject(long expected, DataObject* from, const std::string& to)   {
  DataObject* pObject = 0, *pO = 0;
  StatusCode status = m_pIDP->linkObject(to, from);
  if ( check(status, expected) )      {
    std::cout << " testLinkObject (from ( by Node), to:" << fullpath(from) << ", " << to << ") >>";
    m_pIDP->retrieveObject(to, pO);
    if ( pObject )
      std::cout << " " << fullpath(pObject);
    else
      std::cout << " INVALID SOURCE ";
    if ( pO )
      std::cout << " " << fullpath(pO) << std::endl;
    else
      std::cout << " INVALID TARGET " << std::endl;
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

// Test 
StatusCode DataSvcTest::testUnlinkObject(long expected, const std::string& to)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->unlinkObject(to);
  if ( check(status, expected) )      {
    std::cout << " testUnlinkObject (name:" << to << ") >>";
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Test TraverseTree
//======================================================================
StatusCode DataSvcTest::testTraverseTree(long expected, 
                                         const std::string& dir, 
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
      std::cout << " traverseTree (pAgent) >>";
    }
    else    {
      std::cout << " traverseTree (" << dir << ", pAgent) >>";
    }
    PrintCode(std::cout, status, 0);
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
      std::cout << " traverseTree (" << pNode->name() << ", pAgent) >>";
    }
    else  {
      std::cout << " traverseTree (..., pAgent) >>";
    }
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Test ClearTree
//======================================================================
StatusCode DataSvcTest::testClearTree(long expected, const std::string& dir)    {
  StatusCode status = StatusCode::FAILURE;
  if ( dir == "" )    {
    status = m_pIDM->clearStore();
  }
  else    {
    status = m_pIDM->clearSubTree(dir);
  }
  if ( check(status, expected) )      {
    if ( dir == "" )    {
      std::cout << " clearStore () >>" << std::endl;
    }
    else    {
      std::cout << " clearSubTree (" << dir << ") >>" << std::endl;
    }
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Test ClearTree
//======================================================================
StatusCode DataSvcTest::testClearTree(long expected, DataObject* pNode)    {
  std::string path = "<Unknown>";
  if ( 0 != pNode ) path = pNode->name();
  StatusCode status = m_pIDM->clearSubTree(pNode);
  if ( check(status, expected) )      {
    std::cout << " clearSubTree (" << path << ") >>" << std::endl;
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Set Rot object
//======================================================================
StatusCode DataSvcTest::testSetRoot(long expected, const std::string& root, DataObject* pObject)    {
  StatusCode status = m_pIDM->setRoot(root, pObject);
  if ( check(status, expected) )      {
    std::cout << "setRoot(" << root << ", pObject)" << std::endl;
    PrintCode(std::cout, status, pObject);
  }
  return status;
}

//======================================================================
// Set Rot object by opaque address
//======================================================================
StatusCode DataSvcTest::testSetRoot(long expected, const std::string& root, IOpaqueAddress* pAddr)    {
  StatusCode status = m_pIDM->setRoot(root, pAddr);
  if ( check(status, expected) )      {
    std::cout << "setRoot(" << root << ", pAddr)" << std::endl;
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Add fake address to tree
//======================================================================
StatusCode DataSvcTest::registerAddress(long expected, const std::string& path, unsigned char typ, const CLID& clid)   {
  GenericAddress* pAddr = new GenericAddress(typ, clid);
  StatusCode status = m_pIDM->registerAddress(path, pAddr);
  if ( check(status, expected) )      {
    std::cout << "registerAddress(" << path << ")" << std::endl;
    PrintCode(std::cout, status, 0);
    //pAddr->release();
  }
  return status;
}

//======================================================================
// Add fake address to tree
//======================================================================
StatusCode DataSvcTest::registerAddress(long expected, const std::string& path, const std::string& sub_path, unsigned char typ, const CLID& clid)   {
  DataObject* pObject = 0;
  GenericAddress* pAddr = new GenericAddress(typ, clid);
  StatusCode status = m_pIDP->findObject(path, pObject);
  if ( status.isSuccess() )   {
    status = m_pIDM->registerAddress(pObject, sub_path, pAddr);
  }
  if ( check(status, expected) )      {
    std::cout << "registerAddress(" << path << ", " << sub_path << ")" << std::endl;
    PrintCode(std::cout, status, 0);
    //pAddr->release();
  }
  return status;
}

//======================================================================
// registerAddress fake address from tree
//======================================================================
StatusCode DataSvcTest::unregisterAddress(long expected, const std::string& path)   {
  StatusCode status = m_pIDM->unregisterAddress(path);
  if ( check(status, expected) )      {
    std::cout << "registerAddress(" << path << ")" << std::endl;
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// registerAddress fake address from tree
//======================================================================
StatusCode DataSvcTest::unregisterAddress(long expected, const std::string& path, const std::string& sub_path)   {
  DataObject* pObject = 0;
  StatusCode status = m_pIDP->findObject(path,pObject);
  if ( status.isSuccess() )   {
    status = m_pIDM->unregisterAddress(pObject, sub_path);
  }
  if ( check(status, expected) )      {
    std::cout << "unregisterAddress(" << path << ", " << sub_path << ")" << std::endl;
    PrintCode(std::cout, status, 0);
  }
  return status;
}

//======================================================================
// Test Timing: object find
//======================================================================
StatusCode DataSvcTest::testFindTiming(const std::string& path, int num_loop)   {
  longlong end, start = currentTime();
  DataObject *pObject;
  StatusCode status;
  int i = 0;
  for ( i = 0; i < num_loop; i++ )    {
    status = m_pIDP->findObject(path,pObject);
    if ( !status.isSuccess() )    {
      std::cout << "Error!!!!!" << std::endl;
      m_nerr++;
      return StatusCode::FAILURE;
    }
  }
  end = currentTime();
  if ( m_debug )    {
    std::cout << i << " Loops of m_pIDP->findObject(\"" << path << "\",pObject) took "
              << int((end-start)/1000) << " msec=" 
              << float(end-start)/float(i) << " muSec each" << std::endl;
  }
  return StatusCode::SUCCESS;
}
//======================================================================
// Test Timing: object find
//======================================================================
StatusCode DataSvcTest::testFindTiming(DataObject* pNode, const std::string& path, int num_loop)   {
  longlong end, start = currentTime();
  DataObject *pObject;
  StatusCode status;
  int i = 0;
  for ( i = 0; i < num_loop; i++ )    {
    status = m_pIDP->findObject(pNode,path,pObject);
    if ( !status.isSuccess() )    {
      std::cout << "Error!!!!!" << std::endl;
      m_nerr++;
      return StatusCode::FAILURE;
    }
  }
  end = currentTime();
  if ( m_debug )    {
    std::cout << i << " Loops of m_pIDP->findObject(\""
              << pNode->registry()->name()
              << "\", \""
              << path
              << "\", pObject) took "
              << int((end-start)/1000) << " msec=" 
              << float(end-start)/float(i) << " muSec each" << std::endl;
  }
  return StatusCode::SUCCESS;
}
