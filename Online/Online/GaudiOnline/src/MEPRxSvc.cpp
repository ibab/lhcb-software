//	============================================================
//
//	MEPRxSvc.cpp
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPRxSvc.h"
#include "GaudiOnline/DimTaskFSM.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPRxSvc)

// Standard Constructor
LHCb::MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc), m_handle(0), m_receiveEvents(false), m_incidentSvc(0)
{
  declareProperty("ConsumeEvents",m_receiveEvents);
}

// Standard Destructor
LHCb::MEPRxSvc::~MEPRxSvc()   {
}

int LHCb::MEPRxSvc::exec(void* arg)  {
  LHCb::MEPRxSvc* s = (LHCb::MEPRxSvc*)arg;
  return s->execute();
}

int LHCb::MEPRxSvc::execute()  {
  MsgStream log(msgSvc(),name());
  bool exe = true;
  for(int cnt=1; exe || m_receiveEvents; ++cnt )  {
    lib_rtl_sleep(500);
    if ( m_receiveEvents )  {
      exe = false;
      log << MSG::INFO << "Sleep[" << cnt << "]" << endmsg;
      if ( (cnt%20) == 0 )  {
        Incident incident(name(),"DAQ_ERROR");
        m_incidentSvc->fireIncident(incident);
      }
    }
  }
  return 1;
}

// IInterface implementation: Query interface
StatusCode 
LHCb::MEPRxSvc::queryInterface(const InterfaceID& riid,void** ppvInterface) {
  if ( riid == IID_IRunable )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly availible: try out a base class
  return Service::queryInterface(riid, ppvInterface);
}

StatusCode LHCb::MEPRxSvc::initialize()  {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering initialize....." << endmsg;
  if ( service("IncidentSvc",m_incidentSvc).isSuccess() )  {
    int sc = lib_rtl_start_thread(MEPRxSvc::exec, this, &m_handle);
    if ( lib_rtl_is_success(sc) )  {
      return StatusCode::SUCCESS;
    }
    log << MSG::ERROR << "Failed to spawn off thread." << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::ERROR << "Failed to access incident service." << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MEPRxSvc::finalize()  {
  m_receiveEvents = false;
  int sc = lib_rtl_join_thread(m_handle);
  if ( m_incidentSvc ) m_incidentSvc->release();
  m_incidentSvc = 0;
  return StatusCode::SUCCESS;
}

// IRunable implementation: Run the object
StatusCode LHCb::MEPRxSvc::run() {
  m_receiveEvents = true;
  return StatusCode::SUCCESS;
}
