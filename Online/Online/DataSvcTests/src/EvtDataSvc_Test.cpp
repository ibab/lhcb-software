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
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IEventDataSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

// C/C++ header files
#include <iostream>
#include <stdexcept>
#include <fstream>

using namespace Gaudi;
using namespace std;

template <class T> class EvtDataSvcTest {

  enum Command {
    SETROOT=1,
    RETRIEVE,
    REGOBJ,
    NO_COMMAND
  };

  /// Pointer to Service locator object
  ISvcLocator*      m_svcLoc;
  /// Pointer to message service instance
  IMessageSvc*      m_msgSvc;
  /// Keep reference to the apoplication managers user interface
  IAppMgrUI*        m_appMgr;

  /// Pointer to generic data service
  IService*         m_pIS;
  /// Pointer to data provider interface
  T*                m_pIDP;
  /// Pointer to data manager interface
  IDataManagerSvc*  m_pIDM;
  /// Pointer to property manager
  IProperty*        m_pPrp;
  /// Pointer to created instance
  IInterface*       m_pDataSvc;
  /// Pointer to incident handler
  IIncidentSvc*     m_pIncidentSvc;

  bool              m_debug;
  int               m_nerr;
  vector<EventTag_t> tags;
  std::vector<string> leaves;
  std::vector<Command> commands;
  std::vector<DataObject*> objects;

public:
  EvtDataSvcTest(const std::string& name);
  void populate(const std::string& file_name);
  void execute(const std::string& file_name);
  void load_tags(const std::string& file_name);
  void load_commands(const std::string& file_name);
  void create_objects();
  void delete_objects();
  void setup() {}
};

#include <sys/time.h>
struct Timer {
  struct timeval m_start, m_stop;
  Timer() {}
  void start() {
    struct timezone tz;
    ::gettimeofday(&m_start,&tz);
  }
  void stop() {
    struct timezone tz;
    ::gettimeofday(&m_stop,&tz);
  }
  double seconds() const {
    long long int usec = 1000000*(m_stop.tv_sec-m_start.tv_sec)+(m_stop.tv_usec-m_start.tv_usec);
    return double(usec)/1e6;
  }
};

#include <ctime>

template <class T> void EvtDataSvcTest<T>::create_objects() {
  for(size_t n=0; n<tags.size();++n) {
    DataObject* pObj = new DataObject();
    objects.push_back(pObj);
    pObj->addRef();
  }
}

template <class T> void EvtDataSvcTest<T>::delete_objects() {
  for(size_t n=0; n<objects.size();++n) {
    delete objects[n];
  }
  objects.clear();
}

template <class T> void EvtDataSvcTest<T>::load_tags(const std::string& file_name) {
  string item;
  ifstream input(file_name.c_str());
  while(input.good() ) {
    getline(input,item,'\n');
    if ( item.find('/') != string::npos ) {
      leaves.push_back(item);
      //cout << item << endl;
      tags.push_back(EventTag_t(item.c_str()));
    }
  }
  cout << "Read " << tags.size() << " tags." << endl;
}

template <class T> void EvtDataSvcTest<T>::load_commands(const std::string& file_name) {
  size_t idx;
  string item;
  ifstream input(file_name.c_str());
  tags.clear();
  leaves.clear();
  tags.clear();
  while(input.good() ) {
    getline(input,item,'\n');
    if ( (idx=item.find(' ')) != string::npos ) {
      string cmd = item.substr(0,idx);
      string leaf = item.substr(++idx);
      if      ( cmd == "RETRIEVE" ) commands.push_back(RETRIEVE);
      else if ( cmd == "REGOBJ"   ) commands.push_back(REGOBJ);
      else if ( cmd == "SETROOT"   ) commands.push_back(SETROOT);
      else commands.push_back(NO_COMMAND);
      leaves.push_back(leaf);
      string tag = leaf[0] == '/' ? leaf : "/Event/"+leaf;
      tags.push_back(EventTag_t(tag.c_str()));
    }
  }
  cout << "Read " << tags.size() << " tags." << endl;
}

template <> void EvtDataSvcTest<IEventDataSvc>::populate(const std::string& file_name) {
  size_t num_events = 10000, num_actions=0;
  Timer tim;
  load_tags(file_name);
  create_objects();
  IEventDataSvc* ds = m_pIDP;
  tim.start();
  for(size_t loop=0; loop<num_events;++loop) {
    for(size_t i=0; i<leaves.size(); ++i, ++num_actions) {
      const EventTag_t& tag = tags[i];
      DataObject* pObj = objects[i];
      StatusCode sc = ds->put(tag,pObj);
      if ( !sc.isSuccess() ) {
	throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
      }
    }
    m_pIDM->clearStore();
  }
  tim.stop();
  delete_objects();
  cout << "Inserted " << tags.size() << " tags [" << num_actions << " actions] in tes:" << tim.seconds() << endl;
}

template <> void EvtDataSvcTest<IDataProviderSvc>::populate(const std::string& file_name) {
  size_t num_events = 10000, num_actions=0;
  Timer tim;
  load_tags(file_name);
  create_objects();
  IDataProviderSvc* ds = m_pIDP;
  tim.start();
  for(size_t loop=0; loop<num_events;++loop) {
    StatusCode sc = m_pIDM->setRoot("/Event",new DataObject());
    for(size_t i=0; i<leaves.size(); ++i, ++num_actions) {
      const string& tag = leaves[i];
      DataObject* pObj = objects[i];
      StatusCode sc = ds->registerObject(tag,pObj);
      if ( !sc.isSuccess() ) {
	throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
      }
    }
    m_pIDM->clearStore();
  }
  tim.stop();
  delete_objects();
  cout << "Inserted " << tags.size() << " tags [" << num_actions << " actions] in tes:" << tim.seconds() << endl;
}

template <> void EvtDataSvcTest<IEventDataSvc>::execute(const std::string& file_name) {
  Timer tim;
  size_t num_events = 10000, num_actions=0;
  load_commands(file_name);
  create_objects();
  IEventDataSvc* ds = m_pIDP;
  tim.start();
  for(size_t loop=0; loop<num_events;++loop) {
    for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
      DataObject* pObj = 0;
      Command cmd = commands[i];
      const EventTag_t& tag = tags[i];
      StatusCode sc = StatusCode::SUCCESS;
      switch(cmd) {
      case RETRIEVE:
	pObj = ds->retrieve<DataObject>(tag);
	break;
      case REGOBJ:
	sc = ds->put(tag,objects[i]);
	break;
      case SETROOT:
	sc = ds->put(tag,objects[i]);
	break;
      default:
	--num_actions;
	break;
      }
      if ( !sc.isSuccess() ) {
	throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
      }
    }
    m_pIDM->clearStore();
  }
  tim.stop();
  delete_objects();
  cout << "Processed " << num_events << " events [ " << num_actions << " actions] from " 
       << file_name << " in:" << tim.seconds() << " seconds. " 
       << tim.seconds()/num_events << " sec/event" << endl;
  cout << "With a nominal Brunel processing time of ~ 600 msec, this corresponds to " 
       << 100.0*tim.seconds()/0.6/num_events << " %" << endl;
}


template <> void EvtDataSvcTest<IDataProviderSvc>::execute(const std::string& file_name) {
  Timer tim;
  size_t num_events = 10000, num_actions=0;
  load_commands(file_name);
  create_objects();
  IDataProviderSvc* ds = m_pIDP;
  tim.start();
  for(size_t loop=0; loop<num_events;++loop) {
    for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
      DataObject* pObj = 0;
      Command cmd = commands[i];
      const string& tag = leaves[i];
      StatusCode sc = StatusCode::SUCCESS;
      switch(cmd) {
      case RETRIEVE:
	sc = ds->retrieveObject(tag,pObj);
	if ( sc == IDataProviderSvc::INVALID_ROOT ) sc = StatusCode::SUCCESS;
	if ( sc == IDataProviderSvc::INVALID_OBJ_PATH ) sc = StatusCode::SUCCESS;
	break;
      case REGOBJ:
	sc = ds->registerObject(tag,objects[i]);
	break;
      case SETROOT:
	sc = m_pIDM->setRoot(tag,objects[i]);
	break;
      default:
	--num_actions;
	break;
      }
      if ( !sc.isSuccess() ) {
	throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
      }
    }
    m_pIDM->clearStore();
  }
  tim.stop();
  delete_objects();
  cout << "Processed " << num_events << " events [ " << num_actions << " actions] from " 
       << file_name << " in:" << tim.seconds() << " seconds. " 
       << tim.seconds()/num_events << " sec/event" << endl;
  cout << "With a nominal Brunel processing time of ~ 600 msec, this corresponds to " 
       << 100.0*tim.seconds()/0.6/num_events << " %" << endl;
}


template <> void EvtDataSvcTest<IEventDataSvc>::setup()  {
  m_pPrp->setProperty("EnableFaultHandler","true");
  m_pPrp->setProperty("ForceLeaves","true");
  m_pPrp->setProperty("RootName","/Event");
  m_pPrp->setProperty("RootCLID","1");
}

template <> void EvtDataSvcTest<IDataProviderSvc>::setup()  {
  m_pPrp->setProperty("EnableFaultHandler","true");
  m_pPrp->setProperty("ForceLeaves","true");
  m_pPrp->setProperty("RootName","/Event");
  m_pPrp->setProperty("RootCLID","1");
}

template <class T> EvtDataSvcTest<T>::EvtDataSvcTest(const std::string& name) {
  m_debug = true;
  m_nerr = 0;
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

  IInterface* pIF;
  StatusCode status = m_svcLoc->getService(name, m_pIS, true);
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
  status = m_pIS->queryInterface(T::interfaceID(), (void**)&m_pIDP);
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
  setup();
}

extern "C" int gaudi_evtdatasvc_test(int, char**) {
  cout << " ==================================================== " << endl;
  {
    EvtDataSvcTest<IDataProviderSvc> test("EvtDataSvc/DataSvc");
    test.populate("REG.data");
    test.execute("CMDS.data");
  }
  cout << " ==================================================== " << endl;
  {
    EvtDataSvcTest<IEventDataSvc> test("Gaudi::EventDataSvc/DataSvc");
    test.populate("REG.data");
    test.execute("CMDS.data");
  }
  cout << " ==================================================== " << endl;
  return 1;
}
