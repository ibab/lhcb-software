//  ============================================================
//
//  MEPConverterSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPConverterSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "MBM/Producer.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include <climits>
#include <cmath>

using MBM::Producer;
using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPConverterSvc)

// Standard Constructor
LHCb::MEPConverterSvc::MEPConverterSvc(const std::string& nam, ISvcLocator* svc)
: OnlineService(nam,svc), m_mepMgr(0),m_producer(0),m_consumer(0),m_receiveEvts(false)
{
  m_mepCount = m_evtCount = m_packingFactor = m_minAlloc = 0;
  declareProperty("BurstMode",   m_burstMode=false);
  declareProperty("PrintFreq",   m_freq = 0.);
  declareProperty("Requirements",m_req);
  declareProperty("MEPManager",  m_mepMgrName="LHCb::MEPManager/MEPManager");
}

// Standard Destructor
LHCb::MEPConverterSvc::~MEPConverterSvc()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::MEPConverterSvc::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid == IRunable::interfaceID() )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// Incident handler implemenentation: Inform that a new incident has occured
void LHCb::MEPConverterSvc::handle(const Incident& inc)    {
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

StatusCode LHCb::MEPConverterSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  MsgStream log(msgSvc(),name());
  if ( sc.isSuccess() )  {
    if ( service(m_mepMgrName,m_mepMgr).isSuccess() )  {
      try {
        int partID = m_mepMgr->partitionID();
        const std::string& proc = m_mepMgr->processName();
        MEPID id = m_mepMgr->mepID();
        m_producer = new Producer(id->evtBuffer,proc,partID);
        m_consumer = new Consumer(id->mepBuffer,proc,partID);
        for(Requirements::iterator i=m_req.begin(); i!=m_req.end(); ++i)  {
          Requirement r;
          r.parse(*i);
          m_consumer->addRequest(r);
        }
	m_minAlloc = m_producer->minAlloc();
        incidentSvc()->addListener(this,"DAQ_CANCEL");
        incidentSvc()->addListener(this,"DAQ_ENABLE");
        declareInfo("MEPsIn",   m_mepCount=0,"Number of MEPs processed.");
        declareInfo("EventsOut",m_evtCount=0,"Number of events processed.");
        declareInfo("Packing",  m_packingFactor=0,
                    "Packing factor of current/last event.");
	log << MSG::DEBUG << "Running in " 
	    << (const char*)(m_burstMode ? "multi burst" : "collision") 
	    << "mode." << endreq;
        return StatusCode::SUCCESS;
      }
      catch( std::exception& e)  {
        return error(std::string("Failed setup MEP buffers:")+e.what());
      }
    }
    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}

StatusCode LHCb::MEPConverterSvc::finalize()  {
  if ( m_producer    )  {
    delete m_producer;
    m_producer = 0;
  }
  if ( m_consumer    )  {
    delete m_consumer;
    m_consumer = 0;
  }
  if ( m_mepMgr      )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  undeclareInfo("EventsOut");
  undeclareInfo("MEPsIn");
  undeclareInfo("Packing");
  m_minAlloc = 0;
  m_mepCount = 0;
  m_evtCount = 0;
  m_packingFactor = 0;
  return OnlineService::finalize();
}

int LHCb::MEPConverterSvc::declareAllSubEvents(const EventDesc& evt, SubEvents& events)  {
  int evID = 0;
  size_t count = 0, numEvt = events.size();
  int tot_evt_len = 0;

  for(SubEvents::const_iterator j=events.begin(); j!=events.end(); ++j, ++count)  {
    if ( count > numEvt ) {
      error("Subevent iteration error: [found more events than declared]");
      // Continue without error...
      m_mepCount++;
      return MBM_NORMAL;
    }
    const Frags& frags = (*j).second;
    int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
    tot_evt_len += ((sub_evt_len+1)*sizeof(int))/sizeof(int);
    tot_evt_len += m_minAlloc; // Need more to handle internal event padding
  }
  int sc = m_producer->getSpace(tot_evt_len);
  if ( sc == MBM_NORMAL ) {
    for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i)  {
      const Frags& frags = (*i).second;
      int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
      sc = declareSubEvent(evt, ++evID, frags, sub_evt_len);
      if ( sc != MBM_NORMAL )
	return sc;
    }
    sc = m_producer->sendSpace();
    if ( sc == MBM_NORMAL )
      m_mepCount++;
  }
  return sc;
}

int LHCb::MEPConverterSvc::declareSubEvents(const EventDesc& evt, SubEvents& events)  {
  int evID = 0;
  size_t count = 0, numEvt = events.size();
  for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i, ++count)  {
    if ( count > numEvt ) {
      error("Subevent iteration error: [found more events than declared]");
      // Continue without error...
      m_mepCount++;
      return MBM_NORMAL;
    }
    else {
      const Frags& frags = (*i).second;
      int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
      int sc = m_producer->getSpace(sub_evt_len);
      if ( sc == MBM_NORMAL ) {
	sc = declareSubEvent(evt, ++evID, frags, sub_evt_len);
	if ( sc == MBM_NORMAL ) {
	  sc = m_producer->sendSpace();
	  if ( sc == MBM_NORMAL )  {
	    continue;
	  }
	}
      }
      return sc;
    }
  }
  m_mepCount++;
  return MBM_NORMAL;
}

int LHCb::MEPConverterSvc::declareSubEvent(const EventDesc& evt, int evID, const Frags& frags, int sub_evt_len)  {
  MEPID id = m_mepMgr->mepID();
  EventDesc& e = m_producer->event();
  MEPEVENT* ev = (MEPEVENT*)evt.data;
  //MEP_SINGLE_EVT* sev = (MEP_SINGLE_EVT*)e.data;
  LHCb::RawEventHeader* h = (LHCb::RawEventHeader*)e.data;
  //sev->begin = long(ev)-id->mepStart;
  //sev->evID  = evID;
  h->setEventID(evID);
  h->setMEPID(ev->evID);
  h->setDataStart(ev->begin);
  h->setNumberOfFragments(frags.size());
  h->setErrorMask(0);
  h->setNumberOfMissing(0);
  h->setOffsetOfMissing(0);
  for(size_t j=0; j<frags.size(); ++j)  {
    LHCb::MEPFragment* f = frags[j];
    long off =  long(long(f)-id->mepStart);
    // printf("mep:%p Event:%d fragment:%p %d offset:%ld size:%ld\n",
    //        id->mepStart,evID,f,j,off,long(f->size()));
    h->setOffset(j, off);
  }
  ev->events[evID].begin  = long(ev)-id->mepStart;
  ev->events[evID].status = EVENT_TYPE_OK;
  ev->events[evID].signal = 0;
  ev->events[evID].evID   = evID;

  e.mask[0] = id->partitionID;
  e.mask[1] = 0;
  e.mask[2] = 0;
  e.mask[3] = 0;
  e.type    = EVENT_TYPE_EVENT;
  e.len     = sub_evt_len;
  int sc = m_producer->declareEvent();
  if ( sc == MBM_NORMAL ) {
    m_evtCount++;
  }
  return sc;
}

/// Process single event
StatusCode LHCb::MEPConverterSvc::run()  {
  ulonglong prtCount = fabs(m_freq) > 1./ULONGLONG_MAX ? ulonglong(1./m_freq) : ULONGLONG_MAX;
  m_receiveEvts = true;
  m_evtCount = 0;
  try {
    for(int sc=m_consumer->getEvent();sc==MBM_NORMAL && m_receiveEvts; sc=m_consumer->getEvent())  {
      SubEvents events;
      MEPID id = m_mepMgr->mepID();
      unsigned int pid = id->partitionID;
      const EventDesc& e = m_consumer->event();
      MEPEVENT* ev = (MEPEVENT*)e.data;
      LHCb::MEPEvent* me = (LHCb::MEPEvent*)ev->data;
      decodeMEP2EventFragments(me, pid, events);
      if ( ev->magic != mep_magic_pattern() )  {
        error("Bad MEP magic pattern!!!!");
      }
      m_packingFactor = ev->packing = events.size();
      if ( m_burstMode )  {
	if ( declareAllSubEvents(e,events) != MBM_NORMAL )  {
	  return m_receiveEvts ? StatusCode::FAILURE : StatusCode::SUCCESS;
	}
      }
      else if ( declareSubEvents(e, events) != MBM_NORMAL )  {
        return m_receiveEvts ? StatusCode::FAILURE : StatusCode::SUCCESS;
      }
      mep_decrement(id, ev, 1);
      if ( m_consumer->eventAction() != MBM_NORMAL ) {
        return m_receiveEvts ? StatusCode::FAILURE : StatusCode::SUCCESS;
      }
      if ( 0 == ((m_evtCount+1)%prtCount) )  {
        MsgStream log(msgSvc(),name());
        log << MSG::INFO << "Decoded " << m_mepCount 
          << " MEPs leading to " << m_evtCount << " Events." << endmsg;
      }
    }
  } catch( std::exception& e)  {
    return error(std::string("Failed to access MEP buffers:")+e.what());
  }
  return StatusCode::SUCCESS;
}
