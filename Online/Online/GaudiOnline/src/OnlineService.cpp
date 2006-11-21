// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/OnlineService.cpp,v 1.1 2006-11-21 18:29:56 frankb Exp $
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


LHCb::OnlineService::OnlineService(const std::string& nam, ISvcLocator* svc) 
: Service(nam, svc), m_pMonitorSvc(0), m_incidentSvc(0)
{
  declareProperty("MonitorSvcType",m_monitorSvcType = "MonitorSvc");
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
  if ( !sc.isSuccess() )  {
    MsgStream err(msgSvc(),name());
    err << MSG::ERROR << "Cannot initialize service base class of type:"
        << m_monitorSvcType << "." << endmsg;
    return sc;
  }
  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    MsgStream err(msgSvc(),name());
    err << MSG::ERROR << "Cannot access incident service." << endmsg;
    return sc;
  }
  sc = service(m_monitorSvcType,m_pMonitorSvc,true);
  if ( !sc.isSuccess() )  {
    MsgStream err(msgSvc(),name());
    err << MSG::ERROR << "Cannot access monitoring service of type "
                      << m_monitorSvcType << "." << endmsg;
    return sc;
  }
  return sc;
  return monSvc() == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

/// Implementation of IService::finalize()
StatusCode LHCb::OnlineService::finalize()  {
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

/// If not already located try to locate monitoring service with forcing a creation
IMonitorSvc* LHCb::OnlineService::monSvc()   const {
  if( !m_pMonitorSvc )  {
    MsgStream log(msgSvc(),name());
    std::string err = "Cannot access monitoring service of type "
                      + m_monitorSvcType + ".";
    log << MSG::FATAL << err << endmsg;
    throw std::runtime_error(err);
  }
  return m_pMonitorSvc;
}

/// If not already located try to locate monitoring service with forcing a creation
IIncidentSvc* LHCb::OnlineService::incidentSvc()   const {
  if( !m_incidentSvc )  {
    MsgStream log(msgSvc(),name());
    std::string err = "Cannot access Incident service \"IncidentSvc\".";
    log << MSG::FATAL << err << endmsg;
    throw std::runtime_error(err);
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
