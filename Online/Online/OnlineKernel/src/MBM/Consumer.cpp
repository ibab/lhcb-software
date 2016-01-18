#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include "WT/wtdef.h"
#include <stdexcept>
#include <cstring>

// Initializing constructor
MBM::Consumer::Consumer(const std::string& buffer_name, const std::string& client_name, int partition_id, int com_type)
  : Client(buffer_name, client_name, partition_id, com_type)
{
}

// Initializing constructor
MBM::Consumer::Consumer(BMID bmid, const std::string& client_name, int partition_id)
  : Client(bmid, client_name, partition_id)
{
}

// Standard destructor
MBM::Consumer::~Consumer()   {
}

// Switch to non-blocking execution mode
void MBM::Consumer::setNonBlocking(int facility, bool subscribe) {
  Client::setNonBlocking(facility,false);
  if ( subscribe ) {
    int sc = ::wtc_subscribe(facility, eventRearm, eventAction, this);
    if( sc != WT_SUCCESS ) {
      throw std::runtime_error("Failed to subscribe action:"+m_buffName+" [Internal Error]");
    }
  }
}

int MBM::Consumer::eventAst(void* param) {
  Consumer* cons = (Consumer*)param;
  return cons->eventAst();
}

// Ast to be called on event receival (may be overloaded by clients)
int MBM::Consumer::eventAst() {
  //::lib_rtl_output(LIB_RTL_INFO,"Consumer AST\n");
  int sc = MBM_NORMAL;// ::mbm_get_event_ast((void*)m_bmid);
  if ( sc == MBM_NORMAL ) {
    if ( !m_blocking ) {
      sc = ::wtc_insert(m_facility, this);
      if( sc == WT_SUCCESS ) {
        return MBM_NORMAL;
      }
      throw std::runtime_error("Failed to wtc_insert on get event AST:"+m_buffName+" [Internal Error]");
    }
    return MBM_NORMAL;
  }
  throw std::runtime_error("Failed mbm_get_event_ast MBM buffer:"+m_buffName+" [Internal Error]");
}

// Static action to be called on event receival
int MBM::Consumer::eventAction(unsigned int facility, void* param) {
  Consumer* cons = (Consumer*)param;
  if ( facility != cons->m_facility )   {
    // Error ?
  }
  return cons->eventAction();
}

// Static event receival rearm
int MBM::Consumer::eventRearm(unsigned int facility, void* param) {
  Consumer* cons = (Consumer*)param;
  if ( facility != cons->m_facility ) {
    // Error ?
  }
  //::lib_rtl_output(LIB_RTL_INFO,"Consumer Rearm\n");
  return cons->eventRearm();
}

// Action to be called on event receival
int MBM::Consumer::eventAction() {
  return freeEvent();
}

// Event receival rearm
int MBM::Consumer::eventRearm() {
  EventDesc& e = m_event;
  ::memset(&e, 0x0, sizeof(EventDesc));
  int sc = ::mbm_get_event_a(m_bmid,&e.data,&e.len,&e.type,e.mask,m_partID,eventAst,this);
  if( sc == MBM_NORMAL || sc == MBM_NO_EVENT ) {
    sc = (m_blocking) ? ::mbm_wait_event(m_bmid) : ::mbm_wait_event_a(m_bmid);
    if( sc == MBM_NORMAL || sc == MBM_REQ_CANCEL ) {
      return sc;
    }
    throw std::runtime_error("Failed to rearm event action:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to queue event access:"+m_buffName+" [Internal Error]");
}

void MBM::Consumer::addRequest(const Requirement& r)  {
  return addRequest(r.evtype,r.trmask,r.vetomask,r.maskType,r.userType,r.freqType,r.freq);
}

void MBM::Consumer::addRequest(int evtype, const unsigned int trmask[4], const unsigned int vetomask[4], int masktype, int usertype, int freqmode, float freq)  {
  if ( m_bmid != (BMID)-1 ) {
    int sc = ::mbm_add_req(m_bmid,evtype,trmask,vetomask,masktype,usertype,freqmode,freq);
    if ( sc == MBM_NORMAL )  {
      return;
    }
    throw std::runtime_error("Failed to add request to MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to add request to MBM buffer:"+m_buffName+" [Buffer not connected]");
}

void MBM::Consumer::delRequest(const Requirement& r)  {
  return delRequest(r.evtype, r.trmask,r.vetomask,r.maskType,r.userType);
}

void MBM::Consumer::delRequest(int evtype, const unsigned int trmask[4], const unsigned int vetomask[4], int masktype, int usertype)
{
  if ( m_bmid != (BMID)-1 ) {
    int sc = ::mbm_del_req(m_bmid,evtype,trmask,vetomask,masktype,usertype);
    if ( sc == MBM_NORMAL )  {
      return;
    }
    throw std::runtime_error("Failed to delete request to MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to delete request to MBM buffer:"+m_buffName+" [Buffer not connected]");
}

int MBM::Consumer::getEventAsync() {
  if ( m_bmid != (BMID)-1 ) {
    EventDesc& e = m_event;
    int sc = ::mbm_get_event_a(m_bmid,&e.data,&e.len,&e.type,e.mask,m_partID,eventAst,this);
    if ( sc == MBM_NORMAL || sc == MBM_REQ_CANCEL )  {
      return sc;
    }
    throw std::runtime_error("Failed to get event from MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to get event from MBM buffer:"+m_buffName+" [Buffer not connected]");
}

int MBM::Consumer::getEvent() {
  if ( m_bmid != (BMID)-1 ) {
    EventDesc& e = m_event;
    int sc = ::mbm_get_event(m_bmid,&e.data,&e.len,&e.type,e.mask,m_partID);
    if ( sc == MBM_NORMAL || sc == MBM_REQ_CANCEL )  {
      return sc;
    }
    throw std::runtime_error("Failed to get event from MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to get event from MBM buffer:"+m_buffName+" [Buffer not connected]");
}

int MBM::Consumer::freeEvent() {
  if ( m_bmid != (BMID)-1 ) {
    int sc = ::mbm_free_event(m_bmid);
    if ( sc == MBM_NORMAL || sc == MBM_NO_EVENT )  {
      return MBM_NORMAL;
    }
    throw std::runtime_error("Failed to free event from MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to free event from MBM buffer:"+m_buffName+" [Buffer not connected]");
}

// Pause event requests
int MBM::Consumer::pause()  {
  if ( m_bmid != (BMID)-1 ) {
    int sc = ::mbm_pause(m_bmid);
    if ( sc == MBM_NORMAL )  {
      return sc;
    }
    throw std::runtime_error("Failed to pause access to MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to pause access to MBM buffer:"+m_buffName+" [Buffer not connected]");
}

/// Cancel pending request
int MBM::Consumer::cancel()   {
  if ( m_bmid != (BMID)-1 ) {
    int sc = ::mbm_cancel_request(m_bmid);
    if ( sc == MBM_NORMAL )  {
      return sc;
    }
    throw std::runtime_error("Failed to cancel request to MBM buffer:"+m_buffName+" [Internal Error]");
  }
  throw std::runtime_error("Failed to cancel request to MBM buffer:"+m_buffName+" [Buffer not connected]");
}

// Run the application in synchonous mode
int MBM::Consumer::runSynchronous() {
  int sc = eventRearm();
  if ( sc != MBM_NORMAL ) {
    return sc;
  }
  sc = eventAction();
  if ( sc != MBM_NORMAL ) {
    return sc;
  }
  return MBM_NORMAL;
}
