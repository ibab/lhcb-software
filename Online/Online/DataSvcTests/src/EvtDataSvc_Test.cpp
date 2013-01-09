#include "GaudiKernel/System.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IEventDataSvc.h"
#include "DataSvcTests/UnitTest.h"
#include "DataSvcTests/EventDataSvcSetup.h"

// C/C++ header files
#include <stdexcept>

using namespace Tests;
using namespace Gaudi;
using namespace std;


namespace Tests {

  template <class T> class EvtDataSvcTest 
    : public UnitTest, 
      public EventDataSvcSetup 
  {
    /// Pointer to data provider interface
    T* m_pIDP;

  public:
    EvtDataSvcTest(const std::string& name)  
      : UnitTest("EvtDataSvcTest",true), EventDataSvcSetup()
    {
      StatusCode status = getService(name,IService::interfaceID(), (void**)&m_pIS);
      if ( !status.isSuccess() )    {
	m_nerr++;
	cout << "Service:" << name << " not found. " << endl;    
	throw runtime_error("Fatal exception. cannot continue to run.");
      }
      // Data provider interface
      status = m_pIS->queryInterface(T::interfaceID(), (void**)&m_pIDP);
      if ( !status.isSuccess() )    {
	m_nerr++;
	cout << "Service:" << m_pIS->name() << " has NO DataProviderSvc. " << endl;    
	throw runtime_error("Fatal exception. NO DataProviderSvc. cannot continue to run.");
      }
      // IID_IDataManager
      status = m_pIS->queryInterface(IDataManagerSvc::interfaceID(), (void**)&m_pIDM);
      if ( !status.isSuccess() )    {
	m_nerr++;
	cout << "Service:" << m_pIS->name() << " has NO IID_IDataManagerSvc. " << endl;
	throw runtime_error("Fatal exception. NO DataManagerSvc. cannot continue to run.");
      }
      setup(m_pIDP);
    }

    virtual ~EvtDataSvcTest() {
      shutdown();
    }

    void populate(const std::string& file_name) {
      size_t num_events = 10000, num_actions=0;
      Timer tim;
      load_tags(file_name);
      create_objects();
      tim.start();
      for(size_t loop=0; loop<num_events;++loop)   {
	num_actions += insert_leaves(m_pIDP);
      }
      tim.stop();
      delete_objects();
      cout << "Inserted " << tags.size() << " tags [" << num_actions 
	   << " actions] in tes:" << tim.seconds() << endl;
    }

    void execute(const std::string& file_name) {
      Timer tim;
      size_t num_events = 10000, num_actions=0;
      load_commands(file_name);
      create_objects();
      tim.start();
      for(size_t loop=0; loop<num_events;++loop) {
	num_actions += excute_commands(m_pIDP);
      }
      tim.stop();
      delete_objects();
      cout << "Processed " << num_events << " events [ " << num_actions << " actions] from " 
	   << file_name << " in:" << tim.seconds() << " seconds. " 
	   << tim.seconds()/num_events << " sec/event" << endl;
      cout << "With a nominal Brunel processing time of ~ 600 msec, this corresponds to " 
	   << 100.0*tim.seconds()/0.6/num_events << " %" << endl;
    }

  };
}

extern "C" int gaudi_evtdatasvc_test(int, char**) {
  {
    EvtDataSvcTest<IDataProviderSvc> test("EvtDataSvc/DataSvc");
    test.populate("REG.data");
    test.execute("CMDS.data");
  }
  {
    EvtDataSvcTest<IEventDataSvc> test("Gaudi::EventDataSvc/DataSvc");
    test.populate("REG.data");
    test.execute("CMDS.data");
  }
  return 1;
}
