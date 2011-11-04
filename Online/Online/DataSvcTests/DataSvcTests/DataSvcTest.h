//====================================================================
//	DataSvc test class functionality
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
#ifndef DATASVC_DATASVCTEST_H
#define DATASVC_DATASVCTEST_H

// Interface include files
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IService.h"

#include "DataSvcTests/UnitTest.h"

// C/C++ header files
#include <iostream>

/** Definition of a small class to test the persistency service.

  @author M.Frank
  @date   11/2/99
    
*/
class DataSvcTest : public UnitTest  {
public:
  /// Pointer to generic data service
  IService*         m_pIS;
  /// Pointer to data provider interface
  IDataProviderSvc* m_pIDP;
  /// Pointer to data manager interface
  IDataManagerSvc*  m_pIDM;
  /// Pointer to property manager
  IProperty*        m_pPrp;
  /// Pointer to created instance
  IInterface*       m_pDataSvc;
  /// Pointer to incident handler
  IIncidentSvc*     m_pIncidentSvc;
public:
  /// Standard constructor
  DataSvcTest(const std::string& nam, bool dbg);
  /// Standard destructor
  virtual ~DataSvcTest();
  /// Shutdown the test...
  virtual void shutdown();
  /// Analyse return code
  std::ostream& PrintCode(std::ostream& s, const StatusCode& status, DataObject* pObject);
  /// Analyse directory entry
  StatusCode testObjDirectory(DataObject* pObject);
  /// Test RegisterObject by full path
  StatusCode testRegisterObject(long expected, const std::string& path, DataObject*pObject);
  /// Test RegisterObject by parent path, object path
  StatusCode testRegisterObject(long expected, 
                                const std::string& parPath, 
                                const std::string& objPath, 
                                DataObject* pObject);
  /// Test RegisterObject by parent/name
  StatusCode testRegisterObject(long expected, DataObject* pNode, const std::string& name, DataObject*pObject);
  /// Test RegisterObject by parent/id
  StatusCode testRegisterObject(long expected, DataObject* pNode, int id, DataObject*pObject);
  /// Test FindObj by name
  StatusCode testFindObject(long expected, const std::string& path);
  /// Test FindObj by parent path, object path
  StatusCode testFindObject(    long expected, 
                                const std::string& parPath,
                                const std::string& objPath);
  /// Test FindObj by parent path, object id
  StatusCode testFindObject(    long expected, 
                                const std::string& parPath,
                                long objID);
  /// Test FindObj by object/name
  StatusCode testFindObject(long expected, DataObject* node, const std::string& name);
  /// Test FindObj by object/id
  StatusCode testFindObject(long expected, DataObject* node, int id);
  /// Test UnregisterObject by name
  StatusCode testUnregisterObject(long expected, const std::string& name);
  /// Test UnregisterObject by object
  StatusCode testUnregisterObject(long expected, DataObject* pObject);
  /// Test RetrieveObject by name
  StatusCode testRetrieveObject(long expected, const std::string& name);
  /// Test RetrieveObject by parent/object
  StatusCode testRetrieveObject(long expected, DataObject* node, const std::string& name);
  /// Test RetrieveObject by parent/id
  StatusCode testRetrieveObject(long expected, DataObject* node, int id);
  /// Test tree traversal by node name
  StatusCode testTraverseTree(long expected, const std::string& dir, bool dbg);
  /// Test tree traversal by node object
  StatusCode testTraverseTree(long expected, DataObject* node, bool dbg);
  /// Test tree clear by node name
  StatusCode testClearTree(long expected, const std::string& dir);
  /// Test tree clear by node object
  StatusCode testClearTree(long expected, DataObject* node);
  /// Set Rot object by object
  StatusCode testSetRoot(long expected, const std::string& root, DataObject* pObject);
  /// Set Rot object by opaque address
  StatusCode testSetRoot(long expected, const std::string& root, IOpaqueAddress* pAddr);
  /// Add fake address to tree
  StatusCode registerAddress(long expected, const std::string& path, const std::string& sub_path, unsigned char typ, const CLID& clid);
  /// Add fake address to tree
  StatusCode registerAddress(long expected, const std::string& path, unsigned char typ, const CLID& clid);
  /// Remove fake address from tree
  StatusCode unregisterAddress(long expected, const std::string& path, const std::string& sub_path);
  /// Remove fake address from tree
  StatusCode unregisterAddress(long expected, const std::string& path);

  /// Test object linking across the store
  StatusCode testLinkObject(long expected, const std::string& from, const std::string& to);
  /// Test object linking across the store
  StatusCode testLinkObject(long expected, DataObject* from, const std::string& to);
  /// Test unlink objects from the store
  StatusCode testUnlinkObject(long expected, const std::string& from);

  /// Test find timing
  StatusCode testFindTiming(const std::string& path, int num_loop);
  StatusCode testFindTiming(DataObject* pNode, const std::string& path, int num_loop);
};
#endif // DATASVC_DATASVCTEST_H
