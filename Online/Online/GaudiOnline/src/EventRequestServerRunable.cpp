// $Id: EventRequestServerRunable.cpp,v 1.2 2008-10-02 06:19:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/EventRequestServerRunable.cpp,v 1.2 2008-10-02 06:19:00 frankb Exp $
#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/EventRequestServerRunable.h"
#include "WT/wt_facilities.h"

// Instantiation of a static factory class used by clients to create instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventRequestServerRunable)

using namespace LHCb;

// Standard Constructor
EventRequestServerRunable::EventRequestServerRunable(const std::string& nam, ISvcLocator* svc)   
: EventServerRunable(nam, svc)
{
}

// Standard Destructor
EventRequestServerRunable::~EventRequestServerRunable()  {
}

// IService implementation: initialize the service
StatusCode EventRequestServerRunable::initialize()   {
  StatusCode sc = EventServerRunable::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize service base class.");
  }
  m_consumer->addRequest(m_request);
  return sc;
}

// IService implementation: finalize the service
StatusCode EventRequestServerRunable::finalize()     {
  shutdownRequests();
  if ( m_consumer ) {
    // The MEPManager at this stage is already excluded since we are a runable.
    // Don't fuck it up here!
    //m_consumer->exclude();
  }
  return EventServerRunable::finalize();
}

void EventRequestServerRunable::restartRequests()  {
  if ( m_recipients.empty() )  {
    if ( m_consState != WAIT_REQ ) {
      m_consState = WAIT_REQ;
      ::lib_rtl_clear_event(m_suspend);
    }
  }
  else if ( m_consState == WAIT_REQ ) {
    ::lib_rtl_set_event(m_suspend);
  }
}

void EventRequestServerRunable::sendEvent()  {
  const MBM::EventDesc& e = m_consumer->event();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); ++i) {
    int sc = net_send(m_netPlug,e.data,e.len,(*i).second.second,WT_FACILITY_CBMEVENT);
    if ( sc==DataTransfer::NET_SUCCESS )   {
      m_recipients.erase(i);
      i = m_recipients.begin();
      return;
    }
    error("Cannot Send event data to "+(*i).first);
  }
}
