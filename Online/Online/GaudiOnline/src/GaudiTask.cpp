#include "GaudiOnline/GaudiTask.h"
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

static    lib_rtl_lock_t  s_lock = 0;


DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)

int gauditask_task_lock() {
  return ::lib_rtl_lock(s_lock);
}
int gauditask_task_trylock() {
  return ::lib_rtl_trylock(s_lock);
}
int gauditask_task_unlock() {
  return ::lib_rtl_unlock(s_lock);
}

// Static thread routine to execute a Gaudi runable
static int execRunable(void* arg)  {
  IRunable* runable = (IRunable*)arg;
  return runable->run();
}

LHCb::GaudiTask::GaudiTask(IInterface*) 
: DimTaskFSM(0), m_handle(0), m_appMgr(0), m_subMgr(0), m_incidentSvc(0), m_msgSvc(0), m_nerr(0)
{
  propertyMgr().declareProperty("Runable",        m_runable     = "LHCb::OnlineRunable/Runable");
  propertyMgr().declareProperty("EventLoop",      m_evtloop     = "LHCb::OnlineRunable/EmptyEventLoop");
  propertyMgr().declareProperty("MessageSvcType", m_msgsvcType  = "MessageSvc");
  propertyMgr().declareProperty("JobOptionsPath", m_mainOptions = "");
  propertyMgr().declareProperty("OptionalOptions",m_optOptions  = "");
  propertyMgr().declareProperty("AutoStart",      m_autostart   = false);
  ::lib_rtl_create_lock(0,&s_lock);
}

LHCb::GaudiTask::~GaudiTask()  {
  if ( m_msgSvc ) m_msgSvc->release();
  m_msgSvc = 0;
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
  ::lib_rtl_delete_lock(s_lock);
}

void LHCb::GaudiTask::lock() {
  gauditask_task_lock();
}

void LHCb::GaudiTask::unlock() {
  gauditask_task_unlock();
}

/// Run the complete job (from intialize to terminate)
StatusCode LHCb::GaudiTask::run()  {
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
      if ( m_autostart )  {
	std::cout << "Commencing autostart sequence..." << std::endl;
	IOCSENSOR.send(this,CONFIGURE);
	IOCSENSOR.send(this,INITIALIZE);
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

void LHCb::GaudiTask::output(int level, const char* s)  {
  MsgStream log(msgSvc(), name());
  log << MSG::Level(level) << s << endmsg;
}

StatusCode LHCb::GaudiTask::unload()  {
  m_appMgr->finalize();
  m_appMgr->terminate();
  return DimTaskFSM::unload();
}

/// Access to message service object
IMessageSvc* LHCb::GaudiTask::msgSvc()  {
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
void LHCb::GaudiTask::handle(const Incident& inc)    {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Got incident:" << inc.source()
      << " of type " << inc.type() << endmsg;
  if ( inc.type() == "DAQ_ERROR" )  {
    IOCSENSOR.send(this,ERROR);
  }
  else if ( inc.type() == "DAQ_FATAL" )  {
    error();
  }
}

StatusCode LHCb::GaudiTask::cancel()  {
  if ( m_incidentSvc )  {
    Incident incident(name(),"DAQ_CANCEL");
    m_incidentSvc->fireIncident(incident);
  }
  return StatusCode::SUCCESS;
}

/// Set properties of application manager instance
StatusCode LHCb::GaudiTask::setInstanceProperties(IAppMgrUI* inst)  {
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

StatusCode LHCb::GaudiTask::startRunable(IRunable* runable)   {
  MsgStream log(msgSvc(), name());
  int sc = lib_rtl_start_thread(execRunable, runable, &m_handle);
  if ( lib_rtl_is_success(sc) )  {
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Failed to spawn off execution thread." << endmsg;
  return StatusCode::FAILURE;
}

int LHCb::GaudiTask::configApplication()  {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;
  if ( 0 == m_subMgr )  {
    m_subMgr = Gaudi::createApplicationMgrEx("GaudiSvc", "ApplicationMgr");
  }
  else {
    log << MSG::INFO << "2nd. layer is already present - reusing instance" << endmsg;
  }
  if ( m_subMgr )  {
    Gaudi::setInstance(m_subMgr);
    StatusCode sc = setInstanceProperties(m_subMgr);
    if ( sc.isSuccess() )  {
      sc = m_subMgr->configure();
      if ( sc.isSuccess() )  {
        log << MSG::INFO << "2nd Level successfully configured." << endmsg;	
        return 1;
      }
      m_subMgr->terminate();
      m_subMgr->release();
      m_subMgr = 0;
      // This means job options were not found - this is an error
      log << MSG::ERROR << "Failed to configure 2nd. level. Bad options ?" << endmsg;
      return 0;
    }
    log << MSG::ERROR << "Failed to configure 2nd. level." << endmsg;
    return sc;
  }
  return sc;
}

int LHCb::GaudiTask::initApplication()  {
  MsgStream log(msgSvc(), name());
  if ( 0 != m_subMgr )  {
    std::string nam, runable_name, evtloop_name;
    SmartIF<IProperty>   ip(m_subMgr);
    SmartIF<ISvcLocator> loc(m_subMgr);
    if ( 0 == m_handle )  {
      StatusCode sc = m_subMgr->initialize();
      if ( sc.isSuccess() )   {
        if ( loc )  {
          if ( ip->getProperty("Runable",nam).isSuccess() )  {
            size_t id1 = nam.find_first_of("\"");
            size_t id2 = nam.find_last_of("\"");
            runable_name = nam.substr(id1+1,id2-id1-1);
            IRunable* runable = 0;
            if ( loc->service(runable_name,runable).isSuccess() )  {
              ip->getProperty("EventLoop",nam);
              id1 = nam.find_first_of("\"");
              id2 = nam.find_last_of("\"");
              evtloop_name = nam.substr(id1+1,id2-id1-1);
              ListItem itm(evtloop_name);
              if ( loc->service("IncidentSvc",m_incidentSvc, true).isSuccess() )  {
                m_incidentSvc->addListener(this,"DAQ_ERROR");
                sc = startRunable(runable);
                if ( sc.isSuccess() )  {
                  return 1;
                }
                log << MSG::ERROR << "Failed to start runable." << endmsg;
                return 0;
              }
              log << MSG::ERROR << "Failed to access incident service." << endmsg;
              return 0;
            }
          }
          log << MSG::ERROR << "Failed to access Runable:" << nam << endmsg;
          return 0;
        }
        log << MSG::ERROR << "Failed to service locator object" << endmsg;
        return 0;
      }
      log << MSG::ERROR << "Failed to initialize application manager" << endmsg;
      return 0;
    }
    else  {
      log << MSG::INFO << "2nd. layer is already executing" << endmsg;
      return 3;
    }
  }
  else  {
    log << MSG::ERROR << "2nd. layer is not initialized...did you call configure?" << endmsg;
    return 0;
  }
  return 0;
}

int LHCb::GaudiTask::finalizeApplication()  {
  if ( m_subMgr )  {
    if ( m_handle )  {
      ::lib_rtl_join_thread(m_handle);
      m_handle = 0;
    }
    gauditask_task_lock();
    try {
      if ( m_incidentSvc ) m_incidentSvc->release();
      m_incidentSvc= 0;
      StatusCode sc = StatusCode::SUCCESS;
      if ( m_subMgr ) sc = m_subMgr->finalize();
      if ( !sc.isSuccess() )   {
	gauditask_task_unlock();
	return 0;
      }
    }
    catch(...) {
    }
    gauditask_task_unlock();
  }
  return 1;
}

int LHCb::GaudiTask::terminateApplication()  {
  if ( m_subMgr )  {
    try {
      StatusCode sc = m_subMgr->terminate();
      if ( sc.isSuccess() )   {
	// Release is called by Gaudi::setInstance; danger of releasing twice?
	// m_subMgr->release();
	m_subMgr = 0;
	Gaudi::setInstance(m_appMgr);
	return 1;
      }
      return 0;
    }
    catch(...) {
    }
  }
  return 1;
}

StatusCode LHCb::GaudiTask::enable()  {
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

StatusCode LHCb::GaudiTask::nextEvent(int /* num_event */)  {
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
