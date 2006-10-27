// $Id: EventRunable.cpp,v 1.3 2006-10-27 16:09:25 frankb Exp $
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiOnline/EventRunable.h"
#include "GaudiOnline/MEPManager.h"

// Instantiation of a static factory class used by clients to create instances of this service

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventRunable)

// Standard Constructor
LHCb::EventRunable::EventRunable(const std::string& nam, ISvcLocator* svcLoc)   
: Service(nam, svcLoc), m_mepMgr(0), m_incidentSvc(0), m_dataSvc(0),
  m_receiveEvts(false), m_nerr(0)
{
  declareProperty("EvtMax", m_evtMax=1);
  declareProperty("NumErrorToStop", m_nerrStop=5);
  declareProperty("MEPManager",  m_mepMgrName="LHCb::MEPManager/MEPManager");
}

// Standard Destructor
LHCb::EventRunable::~EventRunable()   
{
}

// IInterface implementation : queryInterface
StatusCode LHCb::EventRunable::queryInterface(const InterfaceID& riid, void** ppIf)   {
  if ( riid == IID_IRunable )
    *ppIf = (IRunable*)this;
  else if ( riid == IID_IIncidentListener )
    *ppIf = (IIncidentListener*)this;
  else
    return Service::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
}

// IService implementation: initialize the service
StatusCode LHCb::EventRunable::initialize()   {
  StatusCode sc = Service::initialize();
  MsgStream log(msgSvc(), name());
  if ( !sc.isSuccess() )     {
    log << MSG::ERROR << "Failed to initialize service base class." << endmsg;
    return sc;
  }
  if ( !m_mepMgrName.empty() )  {
    if ( !(sc=service(m_mepMgrName,m_mepMgr)).isSuccess() )  {
      log << MSG::ERROR << "Failed to access MEP manager service." << endmsg;
      return sc;
    }
  }
  if( !(sc=service("EventDataSvc",m_dataSvc,true)).isSuccess() )  {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataProviderSvc." << endreq;
    return sc;
  }
  if ( !(sc=service("IncidentSvc",m_incidentSvc,true)).isSuccess() )  {
    log << MSG::ERROR << "Failed to access incident service." << endmsg;
    return sc;
  }
  m_incidentSvc->addListener(this,"DAQ_CANCEL");
  return sc;
}

// IService implementation: finalize the service
StatusCode LHCb::EventRunable::finalize()     {
  if ( m_incidentSvc )  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();  
    m_incidentSvc = 0;
  }
  if ( m_dataSvc )  {
    m_dataSvc->release();
    m_dataSvc = 0;
  }
  if ( m_mepMgr      )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return Service::finalize();
}

/// Incident handler implemenentation: Inform that a new incident has occured
void LHCb::EventRunable::handle(const Incident& inc)    {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Got incident:" << inc.source()
      << " of type " << inc.type() << endmsg;
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_receiveEvts = false;
    m_mepMgr->cancel();
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_receiveEvts = true;
  }
}

// IRunable implementation : Run the class implementation
StatusCode LHCb::EventRunable::run()   {
  SmartIF<IAppMgrUI> ui(serviceLocator());
  MsgStream log(msgSvc(), name());
  if ( ui )    {
    m_receiveEvts = true;
    while ( m_receiveEvts )   {
      // loop over the events
      DataObject* pObj = 0;
      StatusCode sc = ui->nextEvent(m_evtMax);
      if ( sc.isSuccess() )  {
        if ( m_nerr > 0 )  {
          Incident incident(name(),"DAQ_ERROR_CLEAR");
          m_incidentSvc->fireIncident(incident);
        }
        m_nerr = 0;
        if ( !m_dataSvc->findObject("/Event",pObj).isSuccess() )  {
          log << MSG::INFO << "End of event input reached." << endmsg;
          break;
        }
        continue;
      }
      /// Consecutive errors: go into error state
      log << MSG::ERROR << "Failed to process event." << endmsg;
      m_nerr++;
      if ( m_nerr > m_nerrStop )  {
        Incident incident(name(),"DAQ_FATAL");
        m_incidentSvc->fireIncident(incident);
        return StatusCode::FAILURE;
      }
      else if ( m_nerr > 0 )  {
        Incident incident(name(),"DAQ_ERROR");
        m_incidentSvc->fireIncident(incident);
      }
    }
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Failed to access Application manager UI." << endmsg;
  return StatusCode::FAILURE;
}
