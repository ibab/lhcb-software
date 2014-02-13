// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: Runable to run Moore standalone on a single node.
//  Author     : M.Frank
//====================================================================
// $Header: $

#include "MooreMonitorSvc.h"

// Framework include files
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/SvcFactory.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MooreMonitorSvc)

using namespace std;
using namespace LHCb;

/// Initializing constructor
MooreMonitorSvc::MooreMonitorSvc(const string& nam, ISvcLocator* loc) 
  : OnlineService(nam,loc), Interactor()
{
}

/// Standard destructor
MooreMonitorSvc::~MooreMonitorSvc()  {
}

/// IService implementation: initialize the service
StatusCode MooreMonitorSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  incidentSvc()->addListener(this,"MOORE_MONITOR");
  m_clients.clear();
  m_data.clear();
  return sc;
}

/// IService implementation: finalize the service
StatusCode MooreMonitorSvc::finalize()   {
  incidentSvc()->removeListener(this);
  m_clients.clear();
  m_data.clear();
  return OnlineService::finalize();
}

/// Inform that a new incident has occurred
void MooreMonitorSvc::handle(const Incident& incident)   {
  ContextIncident<Statistics*>* inc = (ContextIncident<Statistics*>*)&incident;
  m_data = *(inc->tag());
  IocSensor::instance().send(this,MooreTest::CMD_UPDATE_MOORE_INFO,&m_data);
}

/// Interactor handler
void MooreMonitorSvc::handle (const Event& e)  {
  Clients::iterator i;
  switch ( e.eventtype )    {
  case IocEvent:
    switch(e.type)  {
    case MooreTest::CMD_UPDATE_MOORE_INFO:
      for(i=m_clients.begin(); i != m_clients.end(); ++i)
	IocSensor::instance().send(*i,e.type,e.data);
      break;
    case MooreTest::CMD_REMOVE_MOORE_CLIENT:
      for(i=m_clients.begin(); i != m_clients.end(); ++i)
	if ( (*i) == e.data ) { m_clients.erase(i); return; }
      break;
    case MooreTest::CMD_ADD_MOORE_CLIENT:
      m_clients.push_back((Interactor*)e.data);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}
