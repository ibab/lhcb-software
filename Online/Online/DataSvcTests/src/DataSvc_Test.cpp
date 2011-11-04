//====================================================================
//	Test the DataSvc functionality
//--------------------------------------------------------------------
//
//	Package    : System ( The LHCb Offline System)
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
// Transient data service
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "DataSvcTests/PersistencySvcTest.h"
#include "Factories.h"
#include "Hit.h"

extern const Tests::ICnvFactory& TmpHitConverterFactory;

namespace Tests {

  /** Definition of a small class to test the Gaudi data service.
   *
   *  @author M.Frank
   *  @date   11/2/99
   *
   */
  class DataSvc_Test1   : public DataSvcTest   {
  public:
    std::vector<std::string> m_entries;
    /// Standard constructor
    DataSvc_Test1(bool dbg) : DataSvcTest("DataSvc_Test1", dbg) {}
    /// Standard destructor
    virtual ~DataSvc_Test1()    {}
    /// action methid
    virtual void run(const std::string& rootName, bool useRoot);
    virtual void large(bool use_rnd, 
		       int maxObj, 
		       int mxLvl, 
		       const std::string prefix, 
		       const std::string& rootName, 
		       bool useRoot);
    void makeTree( bool use_rnd,
		   int maxObj, 
		   int mxLvl, 
		   const std::string prefix, 
		   int level, 
		   const std::string& path);
  };
}

using namespace Tests;
using namespace std;
#include <ctime>

void DataSvc_Test1::makeTree(bool use_rnd, 
                             int maxObj, 
                             int mxLvl, 
                             const string prefix, 
                             int level, 
                             const string& path)   {
  float rnd = (use_rnd) ? float(rand()) : float(RAND_MAX);
  float mx = RAND_MAX;
  rnd /= mx;
  rnd *= float(maxObj);
  long numObj = (level==0) ? maxObj : long (rnd);
  char txt[32];
  for ( int i = 0; i < numObj; i++ )    {
    Hit *hit = new Hit();
    string thePath = path;
    sprintf(txt,"%d", i);
    thePath += "/";
    thePath += prefix;
    thePath += txt;
    StatusCode status = testRegisterObject(StatusCode::SUCCESS, thePath, hit);
    m_entries.push_back(thePath);
    if ( level+1 < mxLvl && status.isSuccess() )    {
      IRegistry* pDir = hit->registry();
      makeTree(use_rnd, maxObj, mxLvl, prefix, level+1, pDir->identifier());
    }
  }
}

//======================================================================
// run the standard test
//======================================================================
void DataSvc_Test1::large( bool use_rnd, 
                           int maxObj, 
                           int mxLvl, 
                           const string prefix, 
                           const string& rootName, 
                           bool useRoot)    {
  // Try to add something to the store
  int i;
  DataObject* pObject = 0;
  MsgStream log(msgSvc(), "Test1::"+rootName);
  string theRoot = (useRoot) ? rootName+"/" : "";
  StatusCode status = testSetRoot(StatusCode::SUCCESS, rootName, new Hit());
  log << MSG::DEBUG << "Filling tree...." << endreq;

  float fill_time = float(currentTime());
  makeTree( use_rnd, maxObj, mxLvl, prefix, 0, (useRoot) ? rootName : "");
  fill_time = currentTime() - fill_time;

  float get_time = float(currentTime());
  for( i = 0; i < 100000; i++ )     {
    float rnd = float(rand());
    float mx = RAND_MAX;
    rnd /= mx;
    rnd *= (m_entries.size()-1);
    status = m_pIDP->retrieveObject(m_entries[long(rnd)], pObject);
    if ( !status.isSuccess() )    {
      log << MSG::ERROR << "Cannot retrieve " << m_entries[long(rnd)] << endreq;
    }
  }
  get_time = currentTime() - get_time;
  long nEnt = m_entries.size();
  log << MSG::ALWAYS << "(" << maxObj << " Obj. x " << mxLvl << "+1 Levels) <" << prefix << "> " 
      << endreq
      << "Filled " << m_entries.size() << " entries: "
      << fill_time/1000.0 << " msec (" 
      << fill_time/nEnt << " musec/Ent) "
      << i << " Loops over " 
      << get_time/1000.0 << " msec (" 
      << get_time/i << " musec/Ent)" << endreq;
  status = testClearTree   (StatusCode::SUCCESS, "");
  if ( Hit::refCount() != 0 )  {
    log << MSG::ERROR << "Detected Memory leak in DataSvc." << endreq;
  }
  shutdown();
  log << MSG::DEBUG << "... Test Finished " << System::typeinfoName(typeid(*this)) 
      << " Root=<" << rootName << ">, <" << theRoot << ">" << endreq;
};


//======================================================================
// run the standard test
//======================================================================
void DataSvc_Test1::run(const string& rootName, bool useRoot)    {
  // Try to add something to the store
  string theRoot = (useRoot) ? rootName+"/" : "";
  Hit *hit, *hit1, *hit2, *hit3, *hit4, *hit5, *hit11, *hit21, *hit211;
  StatusCode status = testSetRoot(StatusCode::SUCCESS, rootName, hit=new Hit());
  MsgStream log(msgSvc(), "Test1::"+rootName);
  string rootEx = theRoot.substr(0,theRoot.length()-1);
  log << MSG::DEBUG << "... Test DataProviderSvc Interface Root=<" << rootName << ">, <" << theRoot << ">" << endreq;
  status = testRegisterObject(IDataProviderSvc::INVALID_OBJ_PATH, rootName, hit);
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1",    hit1   = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/1",  new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/1/1",  new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/1/1/1",  new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/1/1/1/1",  new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"2",    hit2   = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"3",    hit3   = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit,4,        hit4   = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit,5,        hit5   = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, rootEx, "6", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, rootEx, "/7", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/11", hit11  = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"2/21", hit21  = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit21, "211", hit211 = new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"2116", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"211", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"211/211", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"211/211/211", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"2117", new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, hit211,"2118", new Hit());
  Hit* hitabc = new Hit();
  status = testRegisterObject(IDataProviderSvc::INVALID_PARENT, theRoot+"a/b/c/d/e", hitabc);

  m_pPrp->setProperty("ForceLeaves", "1");
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"a/b/c", hitabc);
  status = testRegisterObject(StatusCode::SUCCESS, hitabc, "d/e", new Hit());
  m_pPrp->setProperty("ForceLeaves", "0");

  // Fake unloaded entries to hit 211
  DataSvcHelpers::RegistryEntry* hit211_entry = 
    dynamic_cast<DataSvcHelpers::RegistryEntry*>(hit211->registry());
  hit211_entry->add( new DataSvcHelpers::RegistryEntry("2111",0) );
  hit211_entry->add( new DataSvcHelpers::RegistryEntry("2112",0) );
  hit211_entry->add( new DataSvcHelpers::RegistryEntry("/2/21/211/2113",0) );
  hit211_entry->add( new DataSvcHelpers::RegistryEntry("2114",0) );
  hit211_entry->add( new DataSvcHelpers::RegistryEntry("2115",0) );

  status = testFindObject       (StatusCode::SUCCESS,                "");
  status = testFindObject       (StatusCode::SUCCESS,                rootEx);
  status = testFindObject       (StatusCode::SUCCESS,                rootEx, "6");
  status = testFindObject       (StatusCode::SUCCESS,                rootEx, "/7");
  status = testFindObject       (StatusCode::SUCCESS,                rootEx, 7);
  status = testFindObject       (StatusCode::SUCCESS,                theRoot+"1");
  status = testFindObject       (StatusCode::SUCCESS,                theRoot+"1/1");
  status = testFindObject       (StatusCode::SUCCESS,                theRoot+"1/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                theRoot+"1/1/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                theRoot+"1/1/1/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                hit,   1);
  status = testFindObject       (StatusCode::SUCCESS,                hit,  "/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                hit,  "/1/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                hit,  "/1/1/1/1");
  status = testFindObject       (StatusCode::SUCCESS,                hit,  "/1/11");
  status = testFindObject       (StatusCode::SUCCESS,                hit1, "/11");
  status = testFindObject       (StatusCode::SUCCESS,                hit2, "/21/211");
  status = testFindObject       (StatusCode::SUCCESS,                hit211,"/211");
  status = testFindObject       (StatusCode::SUCCESS,                hit211,"/211/211");
  status = testFindObject       (StatusCode::SUCCESS,                hit211,"/211/211/211");
  status = testFindObject       (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"1/plonck");
  status = testUnregisterObject (IDataProviderSvc::DIR_NOT_EMPTY,    theRoot+"2");
  status = testUnregisterObject (StatusCode::SUCCESS,                theRoot+"4");
  status = testUnregisterObject (StatusCode::SUCCESS,                hit3);
  status = testUnregisterObject (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"1/plonck");
  status = testRetrieveObject   (StatusCode::SUCCESS,                "");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot.substr(0,theRoot.length()-1));
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"2/21/211");
  status = testRetrieveObject   (StatusCode::SUCCESS,                hit2, "/21/211");
  status = testRetrieveObject   (StatusCode::SUCCESS,                hit211, "211");
  status = testRetrieveObject   (StatusCode::SUCCESS,                hit211, "211/211");
  status = testRetrieveObject   (StatusCode::SUCCESS,                hit211, "211/211/211");
  status = testRetrieveObject   (IDataProviderSvc::NO_DATA_LOADER,   theRoot+"2/21/211/2111");
  status = testRetrieveObject   (IDataProviderSvc::NO_DATA_LOADER,   theRoot+"2/21/211/2112");
  status = testRetrieveObject   (IDataProviderSvc::NO_DATA_LOADER,   theRoot+"2/21/211/2113");
  status = testRetrieveObject   (IDataProviderSvc::NO_DATA_LOADER,   theRoot+"2/21/211/2114");
  status = testRetrieveObject   (IDataProviderSvc::NO_DATA_LOADER,   theRoot+"2/21/211/2115");
  m_pPrp->setProperty("ForceLeaves", "1");
  m_pPrp->setProperty("EnableFaultHandler","1");
  m_pPrp->setProperty("EnableAccessHandler","1");
  m_pPrp->setProperty("DataFaultName", "TestDataFault");
  m_pPrp->setProperty("DataAccessName","TestDataAccess");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"2/21/211/2115/21x15/21xx15/21xxx15");
  m_pPrp->setProperty("ForceLeaves", "0");
  m_pPrp->setProperty("EnableFaultHandler","0");
  m_pPrp->setProperty("EnableAccessHandler","0");
  status = testRetrieveObject   (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"2/21/211/2200");
  status = testRetrieveObject   (IDataProviderSvc::INVALID_ROOT,hit2,"xx/211");

  status = testLinkObject       (StatusCode::SUCCESS,                "", theRoot+"alias_3");
  status = testLinkObject       (StatusCode::SUCCESS,                theRoot+"1", theRoot+"alias_1");
  status = testLinkObject       (StatusCode::SUCCESS,                theRoot+"2/21", theRoot+"5/alias_21");
  status = testLinkObject       (StatusCode::SUCCESS,                hit2, theRoot+"2/alias_2");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"alias_3");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"alias_1");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"alias_1/1/1/1");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"5/alias_21");
  status = testRetrieveObject   (StatusCode::SUCCESS,                theRoot+"5/alias_21/211");

  status = testTraverseTree(StatusCode::SUCCESS, "", m_debug);

  status = testUnlinkObject     (StatusCode::SUCCESS,                "alias_3");
  status = testUnlinkObject     (StatusCode::SUCCESS,                theRoot+"2/alias_2");
  status = testUnlinkObject     (StatusCode::SUCCESS,                theRoot+"alias_1");
  status = testRetrieveObject   (IDataProviderSvc::INVALID_ROOT,hit2,"alias_1/21/211");

  status = testLinkObject       (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"yy", theRoot+"xx");
  status = testUnlinkObject     (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"yy");

  log << MSG::DEBUG << "... Test DataDirectory Interface Root=<" << rootName << ">, <" << theRoot << ">" << endreq;
  status = testObjDirectory(hit211);
  log << MSG::DEBUG << "... Test Timing" << endreq;
#ifdef _DEBUG
  int num_loop = 50000;
#else
  int num_loop = 100000;
#endif
  testFindTiming(theRoot+"2/21/211/2118", num_loop);
  testFindTiming(hit2, "/21/211/2118",  num_loop);
  testFindTiming(hit21, "/211/2118",    num_loop);
  testFindTiming(hit211,"/2118",        num_loop);
  log << MSG::DEBUG << "... Test DataManagerSvc Interface " << endreq;
  status = testTraverseTree(StatusCode::SUCCESS, "", m_debug);
  status = testTraverseTree(StatusCode::SUCCESS, theRoot+"2", m_debug);
  status = testTraverseTree(StatusCode::SUCCESS, hit2, m_debug);
  status = testClearTree   (IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"1/plonck");
  status = testClearTree   (StatusCode::SUCCESS,                theRoot+"1");
  status = testClearTree   (StatusCode::SUCCESS,                hit2);
  status = testClearTree   (StatusCode::SUCCESS,                "");
  if ( Hit::refCount() != 0 )  {
    log << MSG::ERROR << "Detected Memory leak in DataSvc." << endreq;
  }
  log << MSG::ALWAYS << "... Test Finished " << System::typeinfoName(typeid(*this))
      << " Root=<" << rootName << ">, <" << theRoot << ">" << endreq;
  shutdown();
}

/** Definition of a small class to test the persistency service.

  @author M.Frank
  @date   11/2/99
    
*/
class PersistencySvc_Test1   : public PersistencySvcTest   {
public:
  /// Standard constructor
  PersistencySvc_Test1(bool dbg) 
    : PersistencySvcTest("PersistencySvc_Test1", dbg) {}
  /// Standard destructor
  virtual ~PersistencySvc_Test1()    {}
  /// action methid
  virtual void run(const string& rootName, bool useRoot);
};


// Test generic Persistency service
void PersistencySvc_Test1::run(const string& rootName, bool useRoot)    {
  MsgStream log(msgSvc(), "Test1::"+rootName);
  StatusCode status = StatusCode::SUCCESS;
  string theRoot = (useRoot) ? rootName+"/" : "";
  status = testDataLoader(m_pICV);
  status = testSetRoot(StatusCode::SUCCESS, rootName, new Hit());
  status = testSetRoot(StatusCode::SUCCESS, rootName, 
                       new GenericAddress(TEST_StorageType, CLID_Hit, "","",0,0));

  cout << "... Test Persistency Interface Root=<" << rootName << ">, <" << theRoot << ">" << endl;
  status = addConverter(TmpHitConverterFactory);
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1",       new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"1/11",    new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"2",       new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"3",       new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"3/31",    new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"3/31/311",new Hit());
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"4",       new Hit());

  // Fake an unloaded entry to hit 211
  registerAddress(StatusCode::SUCCESS, theRoot+"2",        "21",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"2",        "22",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"2",        "23",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"3",        "32",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"4",        "41",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"4",        "42",     TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"4",        "43",     TEST_StorageType, CLID_Hit);

  // Test bug reported by Vanya: Intermediate leaves are not created on Registeraddress
  m_pPrp->setProperty("ForceLeaves", "1");
  registerAddress(StatusCode::SUCCESS, theRoot+"4", "44/441/4441",   TEST_StorageType, CLID_Hit);
  m_pPrp->setProperty("ForceLeaves", "0");
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/44/441/4441");
  status = testTraverseTree(StatusCode::SUCCESS, "", m_debug);

  // Test bug reported by O.Callot: Case CT0000000063718
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"3/32",       new Hit());
  status = testRetrieveObject(StatusCode::SUCCESS, theRoot+"3/32");
  status = testRegisterObject(StatusCode::SUCCESS, theRoot+"4/41",       new Hit());
  status = testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/41");

  //======================================================================
  // Retrieve objects from the store: 
  //======================================================================
  status = addConverter(TmpHitConverterFactory);
  status = testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/43");
  registerAddress(StatusCode::SUCCESS, theRoot+"4",        "43/431", TEST_StorageType, CLID_Hit);
  registerAddress(StatusCode::SUCCESS, theRoot+"4/43/433",           TEST_StorageType, CLID_Hit);
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/43/433");
  unregisterAddress(StatusCode::SUCCESS, theRoot+"4",      "43/431");
  unregisterAddress(StatusCode::SUCCESS, theRoot+"4/43/433");
  
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"2/21");
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"2/22");
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/41");
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/42");
  testRetrieveObject(StatusCode::SUCCESS, theRoot+"4/43");
  testRetrieveObject(IDataProviderSvc::INVALID_OBJ_PATH, theRoot+"2/2x");
  status = testClearTree   (StatusCode::SUCCESS, "");
  if ( Hit::refCount() != 0 )  {
    cout << "Detected Memory leak in DataSvc." << endl;
  }
  shutdown();
  log << MSG::ALWAYS << "... Test Finished " << System::typeinfoName(typeid(*this))
      << " Root=<" << rootName << ">, <" << theRoot << ">" 
      << " HitCount:" << Hit::refCount()
      << endreq;
}

static int datasvc_test(int argc, char** /* argv */ )    {
  bool dbg = argc > 1;
//  makeTree( true, 1000, 1, "Object_Number_asfdkjghfjkldhgjklfhgkfgdjkfghdkhfjklgjkleyregfrhlfjgh__", 0, (useRoot) ? rootName : "");
//  makeTree( true, 15, 6, "Object_Number_", 0, (useRoot) ? rootName : "");
//  makeTree( true, 15, 6, "", 0, (useRoot) ? rootName : "");
//  makeTree( false, 1000, 1, "", 0, (useRoot) ? rootName : "");
//  makeTree( false, 5, 5, "Object_Number_Object_Number_Object_Number_Object_Number_", 0, (useRoot) ? rootName : "");
  string prefix = "JustForFunSearch";
  prefix = "a";

  { DataSvc_Test1 tst(dbg);     // 1
    srand(123);
    tst.large(false, 3, 3, prefix, "/Event", true);
  }

  { DataSvc_Test1 tst(dbg);     // 2
    srand(123);
    tst.large(false, 4, 4, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 3
    srand(123);
    tst.large(false, 5, 5, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 4
    srand(123);
    tst.large(false, 6, 6, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 5
    srand(123);
    tst.large(false, 5, 4, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 6
    srand(123);
    tst.large(false,10, 4, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 7
    srand(123);
    tst.large(false,20, 4, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 8
    srand(123);
    tst.large(false, 3, 3, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 9
    srand(123);
    tst.large(false, 3, 6, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 10
    srand(123);
    tst.large(false, 3, 9, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 11
    srand(123);
    tst.large(false, 10, 1, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 12
    srand(123);
    tst.large(false, 20, 1, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 13
    srand(123);
    tst.large(false, 40, 1, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 14
    srand(123);
    tst.large(false, 80, 1, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 15
    srand(123);
    tst.large(false, 160, 1, prefix, "/Event", true);
  }
  { DataSvc_Test1 tst(dbg);     // 15
    srand(123);
    tst.large(false, 320, 1, prefix, "/Event", true);
  }
  /*  */
  /* */
  { 
    DataSvc_Test1 tst(dbg);
    tst.run("/Event", true);
  }
  {
    DataSvc_Test1 tst(dbg);
    tst.run("/Event", false);
  }  
  {
    PersistencySvc_Test1 tst(dbg);
    tst.run("/Event", true);
  }
  {
    PersistencySvc_Test1 tst(dbg);
    tst.run("/Event", false);
  }
  /* */
  return 0x0;
}

//static int datasvc_test(int argc, char** /* argv */ )    {

//extern "C" int rndmsvc_entry(int argc, char* argv[]);
//extern "C" int gaudi_datasvc_test(int argc, char** /* argv */ )    {

extern "C" int gaudi_datasvc_test(int argc, char* argv[])    {
  try   {
    long res = datasvc_test(argc, argv);
    //long res = rndmsvc_entry(argc, argv);
    return res;
  }
  catch(const exception& e)    {
    cout << "Exception caught in main program:" << e.what() << endl;
  }
  catch(...)    {
    cout << "Exception caught in main program." << endl;
  }
  return 0;
}
