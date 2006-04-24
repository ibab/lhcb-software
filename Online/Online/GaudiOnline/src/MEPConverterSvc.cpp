//	============================================================
//
//	MEPConverterSvc.cpp
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
: Service(nam, svc), m_mepMgr(0), m_incidentSvc(0),
  m_producer(0), m_consumer(0), m_receiveEvts(false)
{
  declareProperty("PrintFreq",   m_freq = 0.);
  declareProperty("Requirements",m_req);
  declareProperty("MEPManager",  m_mepMgrName="LHCb::MEPManager/MEPManager");
}

// Standard Destructor
LHCb::MEPConverterSvc::~MEPConverterSvc()   {
}

// IInterface implementation: Query interface
StatusCode LHCb::MEPConverterSvc::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid == IID_IRunable )
    *ppIf = (IRunable*)this;
  else if ( riid == IID_IIncidentListener )
    *ppIf = (IIncidentListener*)this;
  else
    return Service::queryInterface(riid, ppIf);
  addRef();
  return StatusCode::SUCCESS;
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
  StatusCode sc = Service::initialize();
  MsgStream log(msgSvc(),name());
  if ( sc.isSuccess() )  {
    if ( service(m_mepMgrName,m_mepMgr).isSuccess() )  {
      if ( service("IncidentSvc",m_incidentSvc,true).isSuccess() )  {
        m_incidentSvc->addListener(this,"DAQ_CANCEL");
        m_incidentSvc->addListener(this,"DAQ_ENABLE");
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
          return StatusCode::SUCCESS;
        }
        catch( std::exception& e)  {
          log << MSG::ERROR << "Failed setup MEP buffers:" << e.what() << endmsg;
          return StatusCode::FAILURE;
        }
      }
      log << MSG::ERROR << "Failed to access incident service." << endmsg;
      return StatusCode::FAILURE;
    }
    log << MSG::ERROR << "Failed to access MEP manager service." << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::ERROR << "Failed to initialize service base class." << endmsg;
  return sc;
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
  if ( m_incidentSvc )  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();  
    m_incidentSvc = 0;
  }
  return Service::finalize();
}

int LHCb::MEPConverterSvc::declareSubEvents(const EventDesc& evt, SubEvents& events)  {
  int evID = 0;
  size_t count = 0, numEvt = events.size();
  for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i, ++count)  {
    if ( count > numEvt ) {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Subevent iteration error: [found more events than declared]"
          << endmsg;
      // Continue without error...
      return MBM_NORMAL;
    }
    else {
      int sc = declareSubEvent(evt, ++evID, (*i).second);
      if ( sc != MBM_NORMAL )  {
        return sc;
      }
    }
  }
  return MBM_NORMAL;
}

int LHCb::MEPConverterSvc::declareSubEvent(const EventDesc& evt, int evID, const Frags& frags)  {
  int sub_evt_len = LHCb::RawEventHeader::size(frags.size());
  int sc = m_producer->getSpace(sub_evt_len);
  if ( sc == MBM_NORMAL ) {
    MEPID id = m_mepMgr->mepID();
    EventDesc& e = m_producer->event();
    MEPEVENT* ev = (MEPEVENT*)evt.data;
    LHCb::RawEventHeader* h = (LHCb::RawEventHeader*)e.data;
    h->setEventID(evID);
    h->setMEPID(ev->evID);
    h->setDataStart(ev->begin);
    h->setNumberOfFragments(frags.size());
    h->setErrorMask(0);
    h->setNumberOfMissing(0);
    h->setOffsetOfMissing(0);
    for(size_t j=0; j<frags.size(); ++j)  {
      LHCb::MEPFragment* f = frags[j];
      h->setOffset(j, int(int(f)-id->mepStart));
    }
    e.mask[0] = id->partitionID;
    e.mask[1] = 0;
    e.mask[2] = 0;
    e.mask[3] = 0;
    e.type    = EVENT_TYPE_EVENT;
    e.len     = sub_evt_len;
    return m_producer->sendEvent();
  }
  return sc;
}

/// Process single event
StatusCode LHCb::MEPConverterSvc::run()  {
  m_receiveEvts = true;
  ulonglong mepCount = 0;
  ulonglong evtCount = 0;
  ulonglong prtCount = fabs(m_freq) > 1./ULONGLONG_MAX ? ulonglong(1./m_freq) : ULONGLONG_MAX;
  for(int sc=m_consumer->getEvent();sc==MBM_NORMAL && m_receiveEvts; sc=m_consumer->getEvent())  {
    SubEvents events;
    MEPID id = m_mepMgr->mepID();
    unsigned int pid = id->partitionID;
    const EventDesc& e = m_consumer->event();
    MEPEVENT* ev = (MEPEVENT*)e.data;
    LHCb::MEPEvent* me = (LHCb::MEPEvent*)ev->data;
    decodeMEP2EventFragments(me, pid, events);
    if ( ev->magic != mep_magic_pattern() )  {
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Bad MEP magic pattern!!!!" << endmsg;
    }
    ev->packing = events.size();
    evtCount   += ev->packing;
    mepCount++;
    if ( declareSubEvents(e, events) != MBM_NORMAL )  {
      return m_receiveEvts ? StatusCode::FAILURE : StatusCode::SUCCESS;
    }
    mep_decrement(id, ev, 1);
    if ( m_consumer->eventAction() != MBM_NORMAL ) {
      return m_receiveEvts ? StatusCode::FAILURE : StatusCode::SUCCESS;
    }
    if ( 0 == ((evtCount+1)%prtCount) )  {
      MsgStream log(msgSvc(),name());
      log << MSG::INFO << "Decoded " << mepCount 
          << " MEPs leading to " << evtCount << " Events." << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}
