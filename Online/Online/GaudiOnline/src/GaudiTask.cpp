#include "GaudiOnline/GaudiTask.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/SmartIF.h"
#include "CPP/IocSensor.h"
#include "WT/wt_facilities.h"
#include "RTL/rtl.h"
#include <signal.h>
#include <fstream>

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#include "Python.h"
#ifndef _WIN32
extern "C" void _PyGILState_Init(PyInterpreterState *, PyThreadState *);
extern "C" void _PyGILState_Fini();
#endif

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)

using namespace LHCb;
using namespace std;

static    lib_rtl_lock_t  s_lock = 0;

int gauditask_task_lock() {
  return ::lib_rtl_lock(s_lock);
}
int gauditask_task_trylock() {
#ifdef _WIN32
  return ::lib_rtl_lock(s_lock);
#else
  return ::lib_rtl_trylock(s_lock);
#endif
}
int gauditask_task_unlock() {
  return ::lib_rtl_unlock(s_lock);
}

static string loadScript(const string& fname) {
  ifstream file(fname.c_str());
  stringstream str;
  if( file ) {
    char ch;
    while( file.get(ch) ) str.put(ch);
    file.close();
    return str.str();
  }
  return "";
}

GaudiTask::PythonInterpreter::PythonInterpreter() {
  ::Py_Initialize();
}

GaudiTask::PythonInterpreter::~PythonInterpreter() {
  ::Py_Finalize();
}

void GaudiTask::PythonInterpreter::reinitializeGIL() {
  if ( ::Py_IsInitialized() ) {
    PyThreadState *tstate = 0;
    PyInterpreterState *interp = 0;
#ifndef _WIN32
    ::_PyGILState_Fini();
    ::PyOS_AfterFork();
    tstate = PyThreadState_GET();
    interp = tstate->interp;
    ::_PyGILState_Init(interp, tstate);
#endif
  }
}

/// Static thread routine to execute a Gaudi runable
int GaudiTask::execRunable(void* arg)  {
  pair<IRunable*,GaudiTask*>* p = (pair<IRunable*,GaudiTask*>*)arg;
  try {
    IRunable*  r = p->first;
    GaudiTask* t = p->second;
    delete p;
    PythonInterpreter::reinitializeGIL();
    int ret = r->run();
    t->setEventThread(false);
    //::Py_Finalize();
    return ret;
  }
  catch(const exception& e) {
    p->second->output(MSG::FATAL,"Exception while running main thread:"+string(e.what()));
  }
  catch(...) {
    p->second->output(MSG::FATAL,"UNKNWON Exception while running main thread.");
  }
  p->second->setEventThread(false);
  delete p;
  return 0;
}

GaudiTask::GaudiTask(IInterface*) 
: DimTaskFSM(0), m_handle(0), m_appMgr(0), m_subMgr(0), m_incidentSvc(0), m_msgSvc(0), m_nerr(0)
{
  propertyMgr().declareProperty("Runable",        m_runable     = "LHCb::OnlineRunable/Runable");
  propertyMgr().declareProperty("EventLoop",      m_evtloop     = "LHCb::OnlineRunable/EmptyEventLoop");
  propertyMgr().declareProperty("MessageSvcType", m_msgsvcType  = "MessageSvc");
  propertyMgr().declareProperty("JobOptionsPath", m_mainOptions = "");
  propertyMgr().declareProperty("OptionalOptions",m_optOptions  = "");
  propertyMgr().declareProperty("AutoStart",      m_autostart   = 0);
  ::lib_rtl_create_lock(0,&s_lock);
  m_eventThread = false;
  ::PyEval_InitThreads();
}

GaudiTask::~GaudiTask()  {
  if ( m_msgSvc ) m_msgSvc->release();
  m_msgSvc = 0;
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
  ::lib_rtl_delete_lock(s_lock);
}

void GaudiTask::lock() {
  gauditask_task_lock();
}

void GaudiTask::unlock() {
  gauditask_task_unlock();
}

/// Run the complete job (from intialize to terminate)
StatusCode GaudiTask::run()  {
  SmartIF<IAppMgrUI> ui(Gaudi::createInstance("ApplicationMgr","ApplicationMgr","GaudiSvc"));
  if ( ui )  {
    SmartIF<IProperty> ip(ui);
    if ( ip )  {
      ip->setProperty(IntegerProperty("OutputLevel",5));
      ip->setProperty(StringProperty("AppName",""));
      ip->setProperty(StringProperty("EvtSel","NONE"));
      ip->setProperty(StringProperty("HistogramPersistency","NONE"));
      //ip->setProperty(StringProperty("Runable",        m_runable));
      //ip->setProperty(StringProperty("EventLoop",      m_evtloop));
      ip->setProperty(StringProperty("MessageSvcType",   m_msgsvcType));
      if ( m_mainOptions.empty() )  {
        ip->setProperty(StringProperty("JobOptionsType", "NONE"));
      }
      else  {
        ip->setProperty(StringProperty("JobOptionsPath", m_mainOptions));
      }
      m_appMgr = ui;
      if ( m_autostart == 1 )  {
	cout << "Commencing checkpoint sequence..." << endl;
	IOCSENSOR.send(this,CONFIGURE);
      }
      else if ( m_autostart == 2 )  {
	cout << "Commencing autostart sequence..." << endl;
	IOCSENSOR.send(this,CONFIGURE);
	IOCSENSOR.send(this,INITIALIZE);
      }
      else if ( m_autostart == 3 )  {
	cout << "Commencing autostart sequence..." << endl;
	IOCSENSOR.send(this,CONFIGURE);
	IOCSENSOR.send(this,INITIALIZE);
	IOCSENSOR.send(this,START);
      }
      StatusCode sc = m_appMgr->run();
      if ( !sc.isSuccess() ) {
        declareState(ST_ERROR);
        return sc;
      }
    }
  }
  return DimTaskFSM::unload();
}

void GaudiTask::output(int level, const char* s)  {
  MsgStream log(msgSvc(), name());
  log << MSG::Level(level) << s << endmsg;
}

void GaudiTask::output(int level, const string& s)  {
  MsgStream log(msgSvc(), name());
  log << MSG::Level(level) << s << endmsg;
}

StatusCode GaudiTask::unload()  {
  m_python = auto_ptr<PythonInterpreter>(0);
  m_appMgr->stop();
  m_appMgr->finalize();
  m_appMgr->terminate();
  return DimTaskFSM::unload();
}

/// Access to message service object
IMessageSvc* GaudiTask::msgSvc()  {
  if ( !m_msgSvc )  {
    if ( m_appMgr )  {
      SmartIF<ISvcLocator> loc(m_appMgr);
      if ( loc )  {
        loc->service("MessageSvc",m_msgSvc).isSuccess();
      }
    }
  }
  return m_msgSvc;
}

/// Incident handler implemenentation: Inform that a new incident has occured
void GaudiTask::handle(const Incident& inc)    {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Got incident:" << inc.source()
      << " of type " << inc.type() << endmsg;
  if ( inc.type() == "DAQ_ERROR" )  {
    IOCSENSOR.send(this,ERROR);
  }
  if ( inc.type() == "DAQ_ERROR_CLEAR" )  {
    declareState(ST_RUNNING);
  }
  else if ( inc.type() == "DAQ_FATAL" )  {
    error();
  }
  else if ( inc.type() == "DAQ_PAUSE" )  {
    pauseProcessing();
  }
  else if ( inc.type() == "DAQ_CONTINUE" )  {
    continueProcessing();
  }
}

StatusCode GaudiTask::cancel()  {
  if ( m_incidentSvc )  {
    Incident incident(name(),"DAQ_CANCEL");
    m_incidentSvc->fireIncident(incident);
  }
  return StatusCode::SUCCESS;
}

/// Set properties of application manager instance
StatusCode GaudiTask::setInstanceProperties(IAppMgrUI* inst)  {
  SmartIF<IProperty> ip(inst);
  if ( ip )  {
    ip->setProperty(StringProperty("AppName",""));
    ip->setProperty(StringProperty("MessageSvcType", m_msgsvcType));
    if ( m_optOptions.empty() )  {
      ip->setProperty(StringProperty("JobOptionsType","NONE"));
    }
    else  {
      ip->setProperty(StringProperty("JobOptionsPath",m_optOptions));
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode GaudiTask::startRunable(IRunable* runable)   {
  MsgStream log(msgSvc(), name());
  pair<IRunable*,GaudiTask*>* p = new pair<IRunable*,GaudiTask*>(runable,this);
  m_eventThread = true;
  int sc = lib_rtl_start_thread(execRunable, p, &m_handle);
  if ( lib_rtl_is_success(sc) )  {
    return StatusCode::SUCCESS;
  }
  m_eventThread = false;
  log << MSG::ERROR << "Failed to spawn off execution thread." << endmsg;
  return StatusCode::FAILURE;
}

/// Wait for runable thread to stop. If it does not, send kill signal
void GaudiTask::stopRunable() {
  char txt[256];
  for(int i=0; i<999999; ++i) {
    if ( eventThread() ) ::lib_rtl_sleep(4000);
    cancel();
    if ( eventThread() )  {
      if ( m_handle && i>2 ) {
	::sprintf(txt,"Kill runable thread to get out of event loop.");
	output(MSG::WARNING,txt);
	::lib_rtl_kill_thread(m_handle,SIGINT);
      }
      ::lib_rtl_sleep(500);
    }
    if ( !eventThread() ) break;
    ::sprintf(txt,"Retry No. %d to cancel runable thread......",i);
    output(MSG::WARNING,txt);
  }
}

/// Configure second layer application manager for GAUDI Application
int GaudiTask::configApplication()  {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;
  m_eventThread = false;
  if (m_optOptions.find(".opts") != string::npos)
    sc = configSubManager();
  else
    sc = configPythonSubManager();
  if ( sc.isSuccess() )  {
    sc = m_subMgr->configure();
    if ( sc.isSuccess() )  {
      log << MSG::INFO << "2nd Level successfully configured." << endmsg;
      return sc;
    }
    // This means job options were not found - this is an error
    log << MSG::FATAL << "Failed to configure 2nd. level. Bad options ?" << endmsg;
  }
  if ( m_subMgr ) {
    m_subMgr->terminate();
    m_subMgr->release();
    m_subMgr = 0;
  }
  Gaudi::setInstance((IAppMgrUI*)0);
  Gaudi::setInstance((ISvcLocator*)0);
  if ( m_autostart != 0 ) {
    log << MSG::FATAL << "configApplication failed. --> Exit(-1)" << endmsg;
    ::exit(-1);
  }
  return 0;
}

/// Initialize second layer application manager for GAUDI Application
int GaudiTask::initApplication()  {
  MsgStream log(msgSvc(), name());
  if ( 0 != m_subMgr )  {
    SmartIF<ISvcLocator> loc(m_subMgr);
    if ( 0 == m_handle )  {
      StatusCode sc = m_subMgr->initialize();
      if ( sc.isSuccess() )   {
        if ( loc )  {
	  if ( loc->service("IncidentSvc",m_incidentSvc, true).isSuccess() )  {
	    Incident incident(name(),"APP_INITIALIZED");
	    m_incidentSvc->addListener(this,"DAQ_ERROR");
	    m_incidentSvc->addListener(this,"DAQ_ERROR_CLEAR");
	    m_incidentSvc->addListener(this,"DAQ_FATAL");
	    m_incidentSvc->addListener(this,"DAQ_PAUSE");
	    m_incidentSvc->addListener(this,"DAQ_CONTINUE");
	    m_incidentSvc->fireIncident(incident);
	    return 1;
	  }
	  log << MSG::ERROR << "Failed to access incident service." << endmsg;
	  goto Error;
        }
        log << MSG::ERROR << "Failed to access service locator object" << endmsg;
	goto Error;
      }
      log << MSG::ERROR << "Failed to initialize application manager" << endmsg;
      goto Error;
    }
    else  {
      log << MSG::INFO << "2nd. layer is already executing" << endmsg;
      return 3;
    }
  }
  else  {
    log << MSG::ERROR << "2nd. layer is not initialized...did you ever call configure?" << endmsg;
  }
 Error:
  if ( m_autostart != 0 ) {
    log << MSG::FATAL << "initApplication failed. --> Exit(-1)" << endmsg;
    ::exit(-1);
  }
  return 0;
}

/// Initialize second layer application manager for GAUDI Application
int GaudiTask::startApplication()  {
  MsgStream log(msgSvc(), name());
  if ( 0 != m_subMgr )  {
    string nam, runable_name;
    SmartIF<IProperty>   ip(m_subMgr);
    SmartIF<ISvcLocator> loc(m_subMgr);

    if ( ip && loc )  {
      if ( 0 == m_incidentSvc ) {  // In case we were removed during stop(): reconnect
	if ( !loc->service("IncidentSvc",m_incidentSvc, true).isSuccess() )  {
	  log << MSG::ERROR << "Failed to access incident service." << endmsg;
	  goto Error;
	}
	m_incidentSvc->addListener(this,"DAQ_ERROR");
	m_incidentSvc->addListener(this,"DAQ_ERROR_CLEAR");
	m_incidentSvc->addListener(this,"DAQ_FATAL");
	m_incidentSvc->addListener(this,"DAQ_PAUSE");
	m_incidentSvc->addListener(this,"DAQ_CONTINUE");
      }
      StatusCode sc = m_subMgr->start();
      if ( sc.isSuccess() )   {
	Incident incident(name(),"APP_RUNNING");
	m_incidentSvc->fireIncident(incident);
	if ( 0 == m_handle )  {
	  if ( ip->getProperty("Runable",nam).isSuccess() )  {
	    size_t id1 = nam.find_first_of("\"");
	    size_t id2 = nam.find_last_of("\"");
	    runable_name = nam.substr(id1+1,id2-id1-1);
	    IRunable* runable = 0;
	    if ( loc->service(runable_name,runable).isSuccess() )  {
	      sc = startRunable(runable);
	      if ( sc.isSuccess() )  {
		return 1;
	      }
	      log << MSG::ERROR << "Failed to start runable." << endmsg;
	      goto Error;
	    }
	    log << MSG::ERROR << "Failed to access Runable:" << nam << endmsg;
	    goto Error;
	  }
	}
	else  {
	  log << MSG::INFO << "2nd. layer is already executing." << endmsg;
	  return 1;
	}
      }
      log << MSG::ERROR << "Failed to start application manager" << endmsg;
      goto Error;
    }
    log << MSG::ERROR << "Failed to access service locator object" << endmsg;
    goto Error;
  }
  else  {
    log << MSG::ERROR << "2nd. layer is not initialized...did you ever call configure?" << endmsg;
  }
 Error:
  if ( m_autostart != 0 ) {
    log << MSG::FATAL << "startApplication failed. --> Exit(-1)" << endmsg;
    ::exit(-1);
  }
  return 0;
}

/// Finalize second layer application manager for GAUDI Application
int GaudiTask::stopApplication()  {
  if ( m_subMgr )  {
    // If the event thread finished, join it....
    if ( m_handle )   {  //&& m_eventThread == false )  {
      ::lib_rtl_join_thread(m_handle);
      m_handle = 0;
      PythonInterpreter::reinitializeGIL();
    }
    gauditask_task_lock();
    try {
#if 0
      if ( m_incidentSvc ) {
	m_incidentSvc->removeListener(this);
	m_incidentSvc->release();
      }
      m_incidentSvc= 0;
#endif
      StatusCode sc = m_subMgr ? m_subMgr->stop() : StatusCode::SUCCESS;
      if ( !sc.isSuccess() )   {
	MsgStream log(msgSvc(), name());
	log << MSG::ERROR << "Failed to stop the application" << endmsg;
	gauditask_task_unlock();
	return 0;
      }
      else if ( m_incidentSvc ) {
	Incident incident(name(),"APP_STOPPED");
	m_incidentSvc->fireIncident(incident);
      }
    }
    catch(const exception& e) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Crash while stopping 2nd.Layer:" << e.what() << endmsg;
    }
    catch(...) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Unknown crash while stopping 2nd.Layer." << endmsg;
    }
    gauditask_task_unlock();
  }
  return 1;
}

/// Finalize second layer application manager for GAUDI Application
int GaudiTask::finalizeApplication()  {
  if ( m_subMgr )  {
    try {
      StatusCode sc = StatusCode::SUCCESS;
      if ( m_handle )  {
        ::lib_rtl_join_thread(m_handle);
        m_handle = 0;
        PythonInterpreter::reinitializeGIL();
      }
      gauditask_task_lock();
      // First check if the process is still in state 'RUNNING'
      // If yes. call first 'stop'
      SmartIF<IService> isvc(m_subMgr);
      if ( isvc.get() ) {
	if ( isvc->FSMState() == Gaudi::StateMachine::RUNNING ) {
	  output(MSG::ALWAYS,"Attemp to call finalize before stop: will take corrective action....");
	  sc = isvc->stop();
	  if ( !sc.isSuccess() )   {
	    output(MSG::ERROR,"Failed to stop the application");
	    gauditask_task_unlock();
	    return 0;
	  }
	}
      }
      // If e.g.Class1 processes are reset() before start(), then the incident service 
      // is still connected, and a later cancel() would access violate.
      // Hence check again if the incident service is still present.
      if ( m_incidentSvc ) {
	m_incidentSvc->removeListener(this);
	m_incidentSvc->release();
      }
      m_incidentSvc= 0;
      sc = m_subMgr ? m_subMgr->finalize() : StatusCode::SUCCESS;
      if ( !sc.isSuccess() )   {
	output(MSG::ERROR,"Failed to finalize the application.");
	gauditask_task_unlock();
	return 0;
      }
    }
    catch(const exception& e) {
      output(MSG::ERROR,string("Crash while finalizing 2nd.Layer:")+e.what());
    }
    catch(...) {
      output(MSG::ERROR,"Unknown crash finalizing 2nd.Layer.");
    }
    gauditask_task_unlock();
  }
  return 1;
}

/// Terminate second layer application manager for GAUDI Application
int GaudiTask::terminateApplication()  {
  if ( m_subMgr )  {
    try {
      if ( m_incidentSvc ) {
	m_incidentSvc->removeListener(this);
	m_incidentSvc->release();
      }
      m_incidentSvc= 0;
      StatusCode sc = m_subMgr->terminate();
      if ( sc.isSuccess() )   {
	// Release is called by Gaudi::setInstance; danger of releasing twice?
	// m_subMgr->release();
	m_subMgr = 0;
	m_python = auto_ptr<PythonInterpreter>(0);
	Gaudi::setInstance(m_appMgr);
	return 1;
      }
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Failed to terminate the application" << endmsg;
      return 0;
    }
    catch(const exception& e) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Crash while terminating 2nd.Layer:" << e.what() << endmsg;
    }
    catch(...) {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Unknown crash terminating 2nd.Layer." << endmsg;
    }
  }
  return 1;
}

StatusCode GaudiTask::enable()  {
  if ( m_incidentSvc )  {
    Incident incident(name(),"DAQ_ENABLE");
    m_incidentSvc->fireIncident(incident);
  }
  MsgStream log(msgSvc(), name());
  if ( m_haveEventLoop )  {
    log << MSG::INFO << "Events should now be executed !" << endmsg;
  }
  else  {
    log << MSG::INFO << "No auto eventloop enabled !" << endmsg;
  }
  return DimTaskFSM::enable();
}

StatusCode GaudiTask::nextEvent(int /* num_event */)  {
  if ( m_haveEventLoop )  {
    StatusCode sc = m_subMgr->nextEvent(1);
    if ( sc.isSuccess() )  {
      m_nerr = 0;
      return DimTaskFSM::nextEvent(1);
    }
    /// Consecutive errors: go into error state
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Failed to process event." << endmsg;
    if ( m_nerr > 0 )  {
      return error();
    }
    m_nerr++;
  }
  return DimTaskFSM::nextEvent(1);
}

/// Configure standard C++ second level application manager
StatusCode GaudiTask::configSubManager() {
  MsgStream log(msgSvc(), name());
  if ( 0 == m_subMgr )  {
    Gaudi::setInstance((IAppMgrUI*)0);
    Gaudi::setInstance((ISvcLocator*)0);
    m_subMgr = Gaudi::createApplicationMgrEx("GaudiSvc", "ApplicationMgr");
    Gaudi::setInstance(m_subMgr);
  }
  else {
    log << MSG::WARNING << "2nd. layer is already present - reusing instance" << endmsg;
  }
  return setInstanceProperties(m_subMgr);
}

/// Configure Python based second level application manager
StatusCode GaudiTask::configPythonSubManager() {
  MsgStream log(msgSvc(), name());
  m_python = auto_ptr<PythonInterpreter>(new PythonInterpreter());
  m_subMgr = 0;
  Gaudi::setInstance((IAppMgrUI*)0);
  Gaudi::setInstance((ISvcLocator*)0);
  log << MSG::DEBUG << "Python setup:" << m_optOptions << endmsg;
  string cmd = (strncasecmp(m_optOptions.c_str(),"command=",8)==0) 
    ? m_optOptions.substr(8) : loadScript(m_optOptions);
  if( !cmd.empty() ) {
    string vsn = Py_GetVersion();
    log << MSG::INFO << "Starting python initialization. Python version: [" << vsn << "]" << endmsg;
    m_subMgr = Gaudi::createApplicationMgr("GaudiSvc", "ApplicationMgr");
    Gaudi::setInstance(m_subMgr);
    SmartIF<IProperty> ip(m_subMgr);
    if ( ip )  {
      ip->setProperty(StringProperty("AppName",""));
      ip->setProperty(StringProperty("MessageSvcType", m_msgsvcType));
    }
    ::PyRun_SimpleString((char*)cmd.c_str());
    if ( ::PyErr_Occurred() )   {
      ::PyErr_Print(); 
      ::PyErr_Clear();
      log << MSG::FATAL << "Failed to invoke python startup script." << endmsg;
      m_python = auto_ptr<PythonInterpreter>(0);
      if ( m_subMgr ) {
	m_subMgr->release();
	m_subMgr = 0;
	Gaudi::setInstance((IAppMgrUI*)0);
	Gaudi::setInstance((ISvcLocator*)0);
      }
      return StatusCode::FAILURE;
    }
    log << MSG::INFO << "Python initialization done. ";
    return StatusCode::SUCCESS;
  }
  log << MSG::FATAL << "Failed to invoke python startup script." << endmsg;
  m_python = auto_ptr<PythonInterpreter>(0);
  return StatusCode::FAILURE;
}
