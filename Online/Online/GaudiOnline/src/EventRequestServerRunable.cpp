// $Id: EventRequestServerRunable.cpp,v 1.3 2008-10-06 11:49:19 frankb Exp $
//====================================================================
//  EventRequestServerRunable
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Runable to serve independent network clients with event
//               data. Requests are purely served on demand ie. each
//               client has to ask for every event to be processed.
//               One event is sent to exactly one target process.
//
//  Author     : M.Frank
//  Created    : 4/12/2007
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/EventRequestServerRunable.cpp,v 1.3 2008-10-06 11:49:19 frankb Exp $
#include "GaudiOnline/EventRequestServerRunable.h"
#include "WT/wt_facilities.h"

using namespace LHCb;

// Standard Constructor
EventRequestServerRunable::EventRequestServerRunable(const std::string& nam, ISvcLocator* svc)   
: EventServerRunable(nam, svc)
{
}

/// Standard Destructor
EventRequestServerRunable::~EventRequestServerRunable()  {
}

/// IService implementation: initialize the service
StatusCode EventRequestServerRunable::start()   {
  StatusCode sc = EventServerRunable::start();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize service base class.");
  }
  m_consumer->addRequest(m_request);
  return sc;
}

/// Send event data to a list of waiting clients
StatusCode EventRequestServerRunable::sendEvent()  {
  const MBM::EventDesc& e = m_consumer->event();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); ++i) {
    int sc = net_send(m_netPlug,e.data,e.len,(*i).first,WT_FACILITY_CBMEVENT);
    if ( sc==DataTransfer::NET_SUCCESS )   {
      m_recipients.erase(i);
      m_consumer->freeEvent();
      return StatusCode::SUCCESS;
    }
    error("Cannot Send event data to "+(*i).first);
  }
  m_consumer->freeEvent();
  return StatusCode::FAILURE;
}

/// Restart event request handling
void EventRequestServerRunable::restartRequests()  {
  if ( m_consumer && m_recipients.size() > 0 )  {
    m_consState = WAIT_EVT;
  }
}
