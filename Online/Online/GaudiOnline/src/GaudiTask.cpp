#include "GaudiOnline/GaudiTask.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "CPP/IocSensor.h"
#include "WT/wt_facilities.h"

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)

LHCb::GaudiTask::GaudiTask(IInterface*) 
: DimTaskFSM(0), m_appMgr(0), m_subMgr(0), m_incidentSvc(0), m_msgSvc(0) 
{
  propertyMgr().declareProperty("Runable",        m_runable     = "LHCb::OnlineRunable");
  propertyMgr().declareProperty("EventLoop",      m_evtloop     = "MinimalEventLoopMgr");
  propertyMgr().declareProperty("MessageSvcType", m_msgsvcType  = "MessageSvc");
  propertyMgr().declareProperty("JobOptionsPath", m_mainOptions = "");  propertyMgr().declareProperty("OptionalOptions",m_optOptions  = "");
  m_haveEventLoop = false;
}

LHCb::GaudiTask::~GaudiTask()  {
  if ( m_msgSvc ) m_msgSvc->release();
  m_msgSvc = 0;
  if ( m_appMgr ) m_appMgr->release();
  m_appMgr= 0;
}

/// Run the complete job (from intialize to terminate)
StatusCode LHCb::GaudiTask::run()  {
  SmartIF<IAppMgrUI> ui(Gaudi::createInstance("ApplicationMgr","ApplicationMgr","GaudiSvc"));
  if ( ui )  {
    SmartIF<IProperty> ip(ui);
    if ( ip )  {
      ip->setProperty(IntegerProperty("OutputLevel",5));
      ip->setProperty(StringProperty("EvtSel","NONE"));
      ip->setProperty(StringProperty("HistogramPersistency","NONE"));
      ip->setProperty(StringProperty("Runable",        m_runable));
      ip->setProperty(StringProperty("EventLoop",      m_evtloop));
      ip->setProperty(StringProperty("MessageSvcType", m_msgsvcType));
      if ( m_mainOptions.empty() )  {
        ip->setProperty(StringProperty("JobOptionsType", "NONE"));
      }
      else  {
        ip->setProperty(StringProperty("JobOptionsPath", m_mainOptions));
      }
      m_appMgr = ui;
      return m_appMgr->run();
    }
  }
  return DimTaskFSM::unload();
}

void LHCb::GaudiTask::output(const char* s)  {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << s << endmsg;
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
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

/// Set properties of application manager instance
StatusCode LHCb::GaudiTask::setInstanceProperties(IAppMgrUI* inst)  {
  SmartIF<IProperty> ip(inst);
  if ( ip )  {
    ip->setProperty(StringProperty("JobOptionsPath",m_optOptions));
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCb::GaudiTask::configure()  {
  m_subMgr = Gaudi::createApplicationMgrEx("GaudiSvc", "ApplicationMgr");
  if ( m_subMgr )  {
    Gaudi::setInstance(m_subMgr);
    StatusCode sc = setInstanceProperties(m_subMgr);
    if ( sc.isSuccess() )  {
      sc = m_subMgr->configure();
      if ( sc.isSuccess() )  {
        return DimTaskFSM::configure();
      }
      m_subMgr->terminate();
    }
    return sc;
  }
  declareState(NOT_READY);
  return StatusCode::FAILURE;
}

StatusCode LHCb::GaudiTask::initialize()  {
  MsgStream log(msgSvc(), name());
  std::string nam, runable_name;
  SmartIF<IProperty>   ip(m_subMgr);
  SmartIF<ISvcLocator> loc(m_subMgr);

  StatusCode sc = m_subMgr->initialize();
  if ( sc.isSuccess() )   {
    if ( loc )  {
      if ( ip->getProperty("Runable",nam).isSuccess() )  {
        size_t id1 = nam.find_first_of("\"");
        size_t id2 = nam.find_last_of("\"");
        runable_name = nam.substr(id1+1,id2-id1-1);
        IRunable* runable = 0;
        if ( loc->service(runable_name,runable).isSuccess() )  {
          if ( loc->service("IncidentSvc",m_incidentSvc).isSuccess() )  {
            m_incidentSvc->addListener(this,"DAQ_ERROR");
            DimTaskFSM::initialize();
            sc = runable->run();
            return sc;
          }
          log << MSG::ERROR << "Failed to access incident service." << endmsg;
          goto Failed;
        }
      }
      log << MSG::ERROR << "Failed to access Runable:" << nam << endmsg;
      goto Failed;
    }
    log << MSG::ERROR << "Failed to service locator object" << endmsg;
    goto Failed;
  }
  log << MSG::ERROR << "Failed to initialize application manager" << endmsg;

Failed:
  declareState(READY);
  return sc;
}
	
StatusCode LHCb::GaudiTask::nextEvent(int /* num_event */)  {
  //_asm int 3
  //return m_subMgr->nextEvent(1).isSuccess()  ? DimTaskFSM::nextEvent(1)
  //  : printErr(1,"Failed to execute single shot.");
  return DimTaskFSM::nextEvent(1);
}
	
StatusCode LHCb::GaudiTask::finalize()  {
  if ( m_incidentSvc ) m_incidentSvc->release();
  m_incidentSvc= 0;
  StatusCode sc = m_subMgr->finalize();
  if ( sc.isSuccess() )   {
    return DimTaskFSM::finalize();
  }
  declareState(RUNNING);
  return sc;
}

StatusCode LHCb::GaudiTask::terminate()  {
  StatusCode sc = m_subMgr->terminate();
  if ( sc.isSuccess() )   {
    m_subMgr->release();
    m_subMgr = 0;
    Gaudi::setInstance(m_appMgr);
    return DimTaskFSM::terminate();
  }
  declareState(READY);
  return sc;
}
