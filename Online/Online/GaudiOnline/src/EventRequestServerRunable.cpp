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
  m_consState = WAIT_EVT;
  m_mbmCount = m_sendCount = 0;
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

DataTransfer::netentry_t* EventRequestServerRunable::getClient() {
  if ( m_recipients.empty() ) return 0;
  Recipients::iterator i=m_recipients.begin();
  DataTransfer::netentry_t* e=(*i).second.second;
  m_recipients.erase(i);
  return e;
}

StatusCode EventRequestServerRunable::sendEvent()  {
  const MBM::EventDesc& e = m_consumer->event();
  for(Recipients::iterator i=m_recipients.begin(); i!=m_recipients.end(); ++i) {
    int sc = net_send(m_netPlug,e.data,e.len,(*i).second.second,WT_FACILITY_CBMEVENT);
    if ( sc==DataTransfer::NET_SUCCESS )   {
      ++m_sendCount;
      if ( m_sendCount != m_mbmCount ) {
	char txt[64];
	::snprintf(txt,sizeof(txt),"  [%d - %d]",m_sendCount, m_mbmCount);
	m_mbmCount = m_sendCount = 0;
	info("Event request handler: Internal counter mismatch:"+(*i).first+txt);
      }
      m_recipients.erase(i);
      return StatusCode::SUCCESS;
    }
    error("Cannot Send event data to "+(*i).first);
  }
  return StatusCode::FAILURE;
}

/// Restart event request handling
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

/// IRunable implementation : Run the class implementation
StatusCode EventRequestServerRunable::run()   {
  int sc = MBM_NORMAL;
  int printN = 0;
  bool have_event = false;
  while ( 1 )  {
    //::lib_rtl_lock(m_lock);
    switch(m_consState)  {
      case DONE:
        //::lib_rtl_unlock(m_lock);
        return StatusCode::SUCCESS;
      case WAIT_REQ:
        //::lib_rtl_unlock(m_lock);
        //::lib_rtl_wait_for_event(m_suspend);
        //if ( m_consState == DONE )  {
	//  info("End of event requests reached. Stopping...");
        //  return StatusCode::SUCCESS;
        //}
        m_consState = WAIT_EVT;
        break;
      case WAIT_EVT:
        //::lib_rtl_unlock(m_lock);
        sc = MBM_NORMAL;
        if ( !have_event ) {
	  sc = m_consumer->getEvent();
	  have_event = true;
	  ++m_mbmCount;
	}
        if ( m_consState == DONE )  {
          return StatusCode::SUCCESS;
        }
        else if ( sc == MBM_NORMAL )  {
          void* lock = 0;
          try {
            // Need to first lock all network channels to avoid dead-locks
            // In the event a data-ready callback is already active, which
            // would also want to task the local lock.
            // Only afterwards it is safe to send data.
            //lock = net_lock(netPlug());
	    ::lib_rtl_lock(m_lock);
	    StatusCode iret = sendEvent();
	    restartRequests();
	    ::lib_rtl_unlock(m_lock);
	    if ( iret.isSuccess() ) {
	      if ( ((++printN)%m_printNum) == 0 ) {
		info("Delivered %9d events to clients.",printN);
	      }
	      m_consumer->freeEvent();
	      have_event = false;
	    }
	    //net_unlock(netPlug(),lock);
            lock = 0;
            //::lib_rtl_unlock(m_lock);
	    if ( have_event ) { 
	      // This should be the rare case....
	      // there should always be requests pending.
	      ::lib_rtl_sleep(10);
	    }
          }
          catch(...)  {
	    info("Exception....");
            if ( lock ) {
	      net_unlock(netPlug(),lock);
	      lock = 0;
	    }
            //::lib_rtl_unlock(m_lock);
          }
        }
        break;
    }
  }
}
