// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/OnlineService.cpp,v 1.2 2006-11-27 13:46:37 frankb Exp $
//	====================================================================
//  MEPManager.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiOnline/OnlineService.h"
#include <stdexcept>
#include <cstdarg>
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

LHCb::OnlineService::OnlineService(const std::string& nam, ISvcLocator* svc) 
: Service(nam, svc), m_pMonitorSvc(0), m_incidentSvc(0), m_error(0), m_info(0), m_debug(0)
{
  m_defMsg = new MsgStream(0,nam);
  declareProperty("MonitorSvcType",m_monitorSvcType = "MonitorSvc");
}

LHCb::OnlineService::~OnlineService()  {
  delete m_defMsg;
}

/// IInterface implementation: Query interface
StatusCode LHCb::OnlineService::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid.versionMatch(IID_IIncidentListener) )  {
    *ppIf = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppIf);
}

/// Implementation of IService::initialize()
StatusCode LHCb::OnlineService::initialize()  {
  StatusCode sc = Service::initialize();
  m_error = new MsgStream(msgSvc(),name());
  m_info  = new MsgStream(msgSvc(),name());
  m_debug = new MsgStream(msgSvc(),name());
  if ( !sc.isSuccess() )  {
    return error("Cannot initialize service base class.");
  }
  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    return error("Cannot access incident service.");
  }
  sc = service(m_monitorSvcType,m_pMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    return error("Cannot access monitoring service of type "+m_monitorSvcType+".");
  }
  return monSvc() == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

/// Implementation of IService::finalize()
StatusCode LHCb::OnlineService::finalize()  {
  if ( m_error )  {
    delete m_error;
    m_error = 0;
  }
  if ( m_info )  {
    delete m_info;
    m_info = 0;
  }
  if ( m_debug )  {
    delete m_debug;
    m_debug = 0;
  }
  if ( m_pMonitorSvc )  {
    m_pMonitorSvc->undeclareAll(this);
    m_pMonitorSvc->release();
    m_pMonitorSvc = 0;
  }
  if ( m_incidentSvc )  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();  
    m_incidentSvc = 0;
  }
  return Service::finalize();
}

/// Pointer to error message stream object
MsgStream& LHCb::OnlineService::error()  const {
  if ( m_error ) return *m_error;
  return *m_defMsg;
}

/// Pointer to informational message stream object
MsgStream& LHCb::OnlineService::info() const  {
  if ( m_info ) return *m_info;
  return *m_defMsg;
}

/// Pointer to debug message stream object
MsgStream& LHCb::OnlineService::debug() const  {
  if ( m_debug ) return *m_debug;
  return *m_defMsg;
}

/// If not already located try to locate monitoring service with forcing a creation
IMonitorSvc* LHCb::OnlineService::monSvc()   const {
  if( !m_pMonitorSvc )  {
    throwError("Cannot access monitoring service of type "+m_monitorSvcType+".");
  }
  return m_pMonitorSvc;
}

/// If not already located try to locate monitoring service with forcing a creation
IIncidentSvc* LHCb::OnlineService::incidentSvc() const {
  if( !m_incidentSvc )  {
    throwError("Cannot access Incident service \"IncidentSvc\".");
  }
  return m_incidentSvc;
}

/// Dummy IIncidentListener implementation: Inform that a new incident has occured
void LHCb::OnlineService::handle(const Incident& /* incident */ )   {
}

void LHCb::OnlineService::undeclareInfo(const std::string& nam)  {
  monSvc()->undeclareInfo(nam,this);
}

void LHCb::OnlineService::undeclareAll()   {
  monSvc()->undeclareAll(this);
}

StatusCode LHCb::OnlineService::throwError(const std::string& msg)   const {
  error(msg);
  throw std::runtime_error(msg);
}

StatusCode LHCb::OnlineService::throwError(const char* msg,...) const  {
  MsgStream err(msgSvc(), name());
  err << MSG::ERROR;
  if ( err.isActive() )  {
    va_list args;
    va_start(args, msg);
    char buff[1024];
    size_t nSize = vsnprintf(buff, sizeof(buff), msg, args);
    err << buff << endmsg;
    if ( nSize == sizeof(buff) )  {
      err << MSG::FATAL << "Incomplete message - buffer overrun." << endreq;
    }
    throw std::runtime_error(err.stream().str());
  }
  return StatusCode::FAILURE;
}

StatusCode LHCb::OnlineService::error(const std::string& msg)   const {
  MsgStream& err = error();
  err << MSG::ERROR;
  if ( err.isActive() ) err << msg << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::OnlineService::error(const char* msg,...)   const {
  MsgStream& err = error();
  err << MSG::ERROR;
  if ( err.isActive() )  {
    va_list args;
    va_start(args, msg);
    char buff[1024];
    size_t nSize = vsnprintf(buff, sizeof(buff), msg, args);
    err << buff << endmsg;
    if ( nSize == sizeof(buff) )  {
      err << MSG::FATAL << "Incomplete message - buffer overrun." << endreq;
    }
  }
  return StatusCode::FAILURE;
}

void LHCb::OnlineService::info(const std::string& msg)   const {
  MsgStream& err = info();
  err << MSG::INFO;
  if ( err.isActive() ) err << msg << endmsg;
}

void LHCb::OnlineService::info(const char* msg,...)   const {
  MsgStream& err = info();
  err << MSG::INFO;
  if ( err.isActive() )  {
    va_list args;
    va_start(args, msg);
    char buff[1024];
    size_t nSize = vsnprintf(buff, sizeof(buff), msg, args);
    err << buff << endmsg;
    if ( nSize == sizeof(buff) )  {
      err << MSG::FATAL << "Incomplete message - buffer overrun." << endreq;
    }
  }
}

void LHCb::OnlineService::debug(const std::string& msg)   const {
  MsgStream& err = debug();
  err << MSG::DEBUG;
  if ( err.isActive() ) err << msg << endmsg;
}

void LHCb::OnlineService::debug(const char* msg,...)   const {
  MsgStream& err = debug();
  err << MSG::DEBUG;
  if ( err.isActive() )  {
    va_list args;
    va_start(args, msg);
    char buff[1024];
    size_t nSize = vsnprintf(buff, sizeof(buff), msg, args);
    err << buff << endmsg;
    if ( nSize == sizeof(buff) )  {
      err << MSG::FATAL << "Incomplete message - buffer overrun." << endreq;
    }
  }
}
