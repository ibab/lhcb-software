#include "GaudiKernel/System.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IEventDataSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "DataSvcTests/UnitTest.h"
#include "DataSvcTests/EventDataSvcSetup.h"

// C/C++ header files
#include <stdexcept>

using namespace Gaudi;
using namespace std;


namespace Tests {

  class EvtDataSvcAlg : public GaudiAlgorithm, public EventDataSvcSetup 
  {
    /// Pointer to data provider interface
    typedef IDataProviderSvc T;
    T*     pIDP;
    string svcName;
    string cmdFile;
    size_t num_actions, num_events;
    Timer tim;
  public:
    EvtDataSvcAlg(const std::string& name,ISvcLocator* svc)  
      : GaudiAlgorithm(name, svc), EventDataSvcSetup()
    {
      declareProperty("SvcName",svcName);
      declareProperty("Commands",cmdFile);
    }
    virtual ~EvtDataSvcAlg() {
    }

    StatusCode initialize() {
      StatusCode status = service(svcName,m_pIS);
      if ( !status.isSuccess() )    {
	cout << "Service:" << svcName << " not found. " << endl;    
	throw runtime_error("Fatal exception. cannot continue to run.");
      }
      // Data provider interface
      status = m_pIS->queryInterface(T::interfaceID(), (void**)&pIDP);
      if ( !status.isSuccess() )    {
	cout << "Service:" << m_pIS->name() << " has NO DataProviderSvc. " << endl;    
	throw runtime_error("Fatal exception. NO DataProviderSvc. cannot continue to run.");
      }
      // IID_IDataManager
      status = m_pIS->queryInterface(IDataManagerSvc::interfaceID(), (void**)&m_pIDM);
      if ( !status.isSuccess() )    {
	cout << "Service:" << m_pIS->name() << " has NO IID_IDataManagerSvc. " << endl;
	throw runtime_error("Fatal exception. NO DataManagerSvc. cannot continue to run.");
      }
      setup(pIDP);
      load_commands(cmdFile);
      create_objects();
      num_actions = 0;
      num_events = 0;
      return StatusCode::SUCCESS;
    }

    StatusCode start()   {
      tim.start();
      return StatusCode::SUCCESS;
    }

    StatusCode stop()   {
      tim.stop();
      delete_objects();
      cout << "Processed " << num_events << " events [ " << num_actions << " actions] from " 
	   << cmdFile << " in:" << tim.seconds() << " seconds. " 
	   << tim.seconds()/num_events << " sec/event" << endl;
      cout << "With a nominal Brunel processing time of ~ 600 msec, this corresponds to " 
	   << 100.0*tim.seconds()/0.6/num_events << " %" << endl;
      return StatusCode::SUCCESS;
    }

    StatusCode execute()   {
      ++num_events;
#if 0
      for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
	DataObject* pObj = 0;
	Command cmd = commands[i];
	const EventTag_t& tag = tags[i];
	StatusCode sc = StatusCode::SUCCESS;
	switch(cmd) {
	case RETRIEVE:
	  pObj = pIDP->retrieve<DataObject>(tag);
	  break;
	case REGOBJ:
	  sc = pIDP->put(tag,objects[i]);
	  break;
	case SETROOT:
	default:
	  --num_actions;
	  break;
	}
	if ( !sc.isSuccess() ) {
	  throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
	}
      }
#endif
      for(size_t i=0; i<tags.size(); ++i, ++num_actions) {
	DataObject* pObj = 0;
	Command cmd = commands[i];
	const string& tag = leaves[i];
	StatusCode sc = StatusCode::SUCCESS;
	switch(cmd) {
	case RETRIEVE:
	  try {
	    pObj = get<DataObject>(pIDP,tag);
	  }
	  catch(...) {
	  }
	  break;
	case REGOBJ:
	  put(pIDP,objects[i],tag);
	  break;
	case SETROOT:
	default:
	  --num_actions;
	  break;
	}
	if ( !sc.isSuccess() ) {
	  throw runtime_error("Fatal exception. cannot populate EventDataSvc.");
	}
      }

      m_pIDM->clearStore();
      return StatusCode::SUCCESS;
    }

  };

  DECLARE_ALGORITHM_FACTORY(EvtDataSvcAlg)

  //typedef EvtDataSvcAlg<IDataProviderSvc> DataProviderSvcAlg;
  //typedef EvtDataSvcAlg<IEventDataSvc>    EventDataSvcAlg;

  //  DECLARE_ALGORITHM_FACTORY(DataProviderSvcAlg)
  //DECLARE_ALGORITHM_FACTORY(EventDataSvcAlg)
}
