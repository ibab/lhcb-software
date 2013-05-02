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
#include "GaudiOnline/DAQError.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MBM/Producer.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include <climits>
#include <cmath>

using MBM::Producer;
using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;
using namespace std;
using namespace LHCb;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPConverterSvc)

// Standard Constructor
MEPConverterSvc::MEPConverterSvc(const string& nam, ISvcLocator* svc)
  : OnlineService(nam,svc), m_mepMgr(0),m_producer(0),m_errProd(0),m_consumer(0),m_receiveEvts(false)
{
  m_mepCount = m_evtCount = m_packingFactor = m_minAlloc = 0;
  declareProperty("BurstMode",   m_burstMode=false);
  declareProperty("PrintFreq",   m_freq = 0.);
  declareProperty("Requirements",m_req);
  declareProperty("MEPManager",  m_mepMgrName="LHCb::MEPManager/MEPManager");
  declareProperty("HandleErrors",m_handleErrs=0);
  declareProperty("ErrorBuffer", m_errBuffer="SEND");
  declareProperty("RoutingBits", m_routingBits=0x400);
}

// Standard Destructor
MEPConverterSvc::~MEPConverterSvc()   {
}

// IInterface implementation: Query interface
StatusCode MEPConverterSvc::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( IRunable::interfaceID().versionMatch(riid) )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}

/// Incident handler implemenentation: Inform that a new incident has occured
void MEPConverterSvc::handle(const Incident& inc)    {
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

StatusCode MEPConverterSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  MsgStream log(msgSvc(),name());
  if ( sc.isSuccess() )  {
    if ( service(m_mepMgrName,m_mepMgr).isSuccess() )  {
      try {
        int partID = m_mepMgr->partitionID();
        const string& proc = m_mepMgr->processName();
        MEPID id = m_mepMgr->mepID();
        m_producer = new Producer(id->evtBuffer,proc,partID);
        m_consumer = new Consumer(id->mepBuffer,proc,partID);
        for(Requirements::iterator i=m_req.begin(); i!=m_req.end(); ++i)  {
          Requirement r;
          r.parse(*i);
          m_consumer->addRequest(r);
        }
	m_minAlloc = m_producer->minAlloc();
	if ( m_handleErrs > 0 ) {
	  m_errProd = m_mepMgr->createProducer(m_errBuffer,proc);
	}
        incidentSvc()->addListener(this,"DAQ_CANCEL");
        incidentSvc()->addListener(this,"DAQ_ENABLE");
        declareInfo("MEPsIn",   m_mepCount=0,"Number of MEPs processed.");
        declareInfo("ErrMEPs",  m_errMEPCount=0,"Number of ERROR Meps");
        declareInfo("EventsOut",m_evtCount=0,"Number of events processed.");
        declareInfo("Packing",  m_packingFactor=0,
                    "Packing factor of current/last event.");
	log << MSG::DEBUG << "Running in " 
	    << (const char*)(m_burstMode ? "multi burst" : "collision") 
	    << "mode." << endreq;
        return StatusCode::SUCCESS;
      }
      catch( exception& e)  {
        return error(string("Failed setup MEP buffers:")+e.what());
      }
    }
    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}

StatusCode MEPConverterSvc::finalize()  {
  if ( m_errProd    )  {
    delete m_errProd;
    m_errProd = 0;
  }
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
  undeclareInfo("ErrMEPs");
  m_minAlloc = 0;
  m_mepCount = 0;
  m_evtCount = 0;
  m_errMEPCount = 0;
  m_packingFactor = 0;
  return OnlineService::finalize();
}

int MEPConverterSvc::declareAllSubEvents(const EventDesc& evt, SubEvents& events)  {
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
    int sub_evt_len = RawEventHeader::size(frags.size());
    tot_evt_len += ((sub_evt_len+1)*sizeof(int))/sizeof(int);
    tot_evt_len += m_minAlloc; // Need more to handle internal event padding
  }
  int sc = m_producer->getSpace(tot_evt_len);
  if ( sc == MBM_NORMAL ) {
    for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i)  {
      const Frags& frags = (*i).second;
      int sub_evt_len = RawEventHeader::size(frags.size());
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

int MEPConverterSvc::declareSubEvents(const EventDesc& evt, SubEvents& events)  {
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
      int sub_evt_len = RawEventHeader::size(frags.size());
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

int MEPConverterSvc::declareSubEvent(const EventDesc& evt, int evID, const Frags& frags, int sub_evt_len)  {
  MEPID id = m_mepMgr->mepID();
  EventDesc& e = m_producer->event();
  MEPEVENT* ev = (MEPEVENT*)evt.data;
  RawEventHeader* h = (RawEventHeader*)e.data;
  h->setEventID(evID);
  h->setMEPID(ev->evID);
  h->setDataStart(ev->begin);
  h->setNumberOfFragments(frags.size());
  h->setErrorMask(0);
  h->setNumberOfMissing(0);
  h->setOffsetOfMissing(0);
  for(size_t j=0; j<frags.size(); ++j)  {
    MEPFragment* f = frags[j];
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

/// Save event data on error
StatusCode MEPConverterSvc::saveEvents(void* data, size_t length, const unsigned int* mask) {
  if ( data || length ) {
    unsigned int partitionID = 0;
    typedef map<unsigned int, RawEvent*> SubEvents;
    typedef vector<RawBank*> Banks;
    MEPEVENT* ev = (MEPEVENT*)data;
    MEPEvent* me = (MEPEvent*)ev->data;
    SubEvents events;

    info("Got request to save MEP with ERRORS as single events.....");    
    if ( ev->magic != mep_magic_pattern() )  {
      return error("Bad MEP magic pattern!!!!");
    }
    decodeMEP(me, false, partitionID, events);
    const OnlineRunInfo* odin_info = 0;
    // In case there are ODIN banks missing, take any first odin bank!
    for(SubEvents::const_iterator k=events.begin(); k!=events.end(); ++k)  {
      const Banks& odin = (*k).second->banks(RawBank::ODIN);
      if ( !odin.empty() ) {
	odin_info = odin[0]->begin<OnlineRunInfo>();
	break;
      }
    }
    // Now write the events to the output buffer
    for(SubEvents::const_iterator i=events.begin(); i!=events.end(); ++i)  {
      RawEvent* raw = (*i).second;
      size_t len, ev_len = rawEventLength(raw);
      int ret = m_errProd->getSpace(ev_len);
      if ( MBM_NORMAL == ret )    {
	unsigned int trMask[] = {0u,0u,mask[3],m_routingBits};
	EventDesc& e = m_errProd->event();
	const Banks& daq  = raw->banks(RawBank::DAQ);
	const Banks& odin = raw->banks(RawBank::ODIN);
	char* space = (char*)e.data;
	Banks daq_banks;
	const OnlineRunInfo* oi = 0;
	for(Banks::const_iterator j=daq.begin(); j != daq.end(); ++j)  {
	  RawBank* b = *j;
	  if( b->version() == DAQ_STATUS_BANK ) {
	    oi = odin.empty() ? odin_info : odin[0]->begin<OnlineRunInfo>();
	    if ( oi ) {
	      MDFHeader::SubHeader sh = b->begin<MDFHeader>()->subHeader();
	      sh.H1->setTriggerMask(trMask);
	      sh.H1->setRunNumber(oi->Run);
	      sh.H1->setOrbitNumber(oi->Orbit);
	      sh.H1->setBunchID(oi->bunchID);
	    }
	    daq_banks.push_back(b);
	    break;
	  }
	}
	if ( 0 == oi ) {
	  error("No ODIN bank found in event. Failed to encode event to ERROR stream");
	}
	else if ( daq_banks.empty() ) {
	  error("No MDF Header bank found. Failed to encode event to ERROR stream");
	}
	else if( !encodeRawBanks(daq_banks,space,ev_len,false,&len).isSuccess() ) {
	  error("Failed to encode event with Raw banks to ERROR stream");
	}
	else if ( !encodeRawBanks(raw,space+len,ev_len-len,true).isSuccess() ) {
	  error("Failed to encode event with Raw banks to ERROR stream");
	}
	else {
	  ::memcpy(e.mask,trMask,sizeof(e.mask));
	  e.type = EVENT_TYPE_EVENT;
	  e.len  = ev_len;
	  ret = m_errProd->sendEvent();
	  if ( MBM_NORMAL != ret )   {
	    error("Failed to declare event to ERROR stream");
	  }
	}
	continue;
      }
      error("Failed to get space for ERROR stream");
    }
    // Now all events are saved. We have to release the RawEvent structures now.
    for(SubEvents::const_iterator m=events.begin(); m!=events.end(); ++m)
      delete (*m).second;
    events.clear();
    error("Finished saving MEP events causing HLT errors.");
  }
  return StatusCode::SUCCESS;
}

/// Save MEP event data on error
StatusCode MEPConverterSvc::saveMEP(void* data, size_t length,const unsigned int* mask) {
  if ( data || length ) {
    static const size_t BANK_LIMIT = 60*1024;
    info("Got request to save MEP with ERRORS.....");    
    const char* start = (const char*)data, *end = start+length;
    size_t len = sizeof(MEPEVENT)+((length/BANK_LIMIT)+1)*sizeof(RawBank);
    int ret = m_errProd->getSpace(len+length);
    if ( ret == MBM_NORMAL )    {
      size_t hdrSize = 0;
      EventDesc& e = m_errProd->event();
      char* target = (char*)e.data;
      unsigned int trMask[] = {0u,0u,mask[3],m_routingBits};
      MEPEvent* me = (MEPEvent*)((MEPEVENT*)data)->data;

      // First create the MDF bank to ensure proper data handling
      RawBank* b = (RawBank*)target;
      b->setMagic();
      b->setType(RawBank::DAQ);
      b->setVersion(DAQ_STATUS_BANK);
      b->setSourceID(0);
      b->setSize(sizeof(MDFHeader)+sizeof(MDFHeader::Header1));
      MDFHeader* hdr = (MDFHeader*)b->data();
      hdr->setChecksum(0);
      hdr->setCompression(0);
      hdr->setHeaderVersion(3);
      hdr->setSpare(0);
      hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
      hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
      hdr->setSize(len);
      MDFHeader::SubHeader h = hdr->subHeader();
      h.H1->setTriggerMask(trMask);
      h.H1->setRunNumber(0);
      h.H1->setOrbitNumber(0);
      h.H1->setBunchID(0);
      bool filled = false;
      for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
	for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
	  for(RawBank *curr=f->first(), *l=f->last(); curr<l; curr=f->next(curr)) {
	    if ( curr->type() == RawBank::DAQ && curr->version() == DAQ_STATUS_BANK ) {
	      MDFHeader::SubHeader c = curr->begin<MDFHeader>()->subHeader();
	      h.H1->setRunNumber(c.H1->runNumber());
	      h.H1->setOrbitNumber(c.H1->orbitNumber());
	      h.H1->setBunchID(c.H1->bunchID());
	      filled = true;
	      break;
	    }
	  }
	  if ( filled ) break;
	}
	if ( filled ) break;
      }
      hdrSize = b->totalSize();
      target += hdrSize;
      // MEP header with failure information
      b = (RawBank*)target;
      b->setMagic();
      b->setType(RawBank::DAQ);
      b->setVersion(DAQ_PROCERR_HEADER);
      b->setSourceID(0);
      b->setSize(MEPEVENT::hdrSize());
      ::memcpy(b->data(),start,MEPEVENT::hdrSize());
      start += MEPEVENT::hdrSize();
      target += b->totalSize();
      // Now add all the banks from the MEP
      while(start<end) {
	size_t blen = size_t(end-start)>BANK_LIMIT ? BANK_LIMIT : size_t(end-start);
	b = (RawBank*)target;
	b->setMagic();
	b->setType(RawBank::DAQ);
	b->setVersion(DAQ_PROCERR_BANK);
	b->setSourceID(0);
	b->setSize(blen);
	::memcpy(b->data(),start,blen);
	start += blen;
	target += b->totalSize();
      }
      ::memcpy(e.mask,trMask,sizeof(e.mask));
      e.type = EVENT_TYPE_EVENT;
      e.len = target - (char*)e.data;
      // Update header with proper size
      hdr->setSize(e.len-hdrSize);
      ret = m_errProd->sendEvent();
      if ( MBM_NORMAL == ret )   {
	return StatusCode::SUCCESS;
      }
      return error("Failed to declare event to ERROR stream");
    }
    return error("Failed to get space for ERROR stream");
  }
  return StatusCode::SUCCESS;
}

/// Process single event
StatusCode MEPConverterSvc::run()  {
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
      MEPEvent* me = (MEPEvent*)ev->data;
      //mep_increment(id, ev, 99);
      if ( e.type == EVENT_TYPE_ERROR ) {
	// We received an error fragment - if we were subscribed to error events
	MsgStream log(msgSvc(),name());
	if ( m_handleErrs > 0 )  {
	  bool handle = true;
	  int typ = e.mask[3];
	  const char* err_typ = "Unknown MEP error";
	  switch(typ) {
	  case MissingSrc:
	    err_typ = "Missing source MEP error";
	    handle = true;
	    break;
	  case BadPkt:             // do not attempt to decode
	    err_typ = "Bad packet received from TELL1 board";
	    handle = false;
	    break;
	  case Multiple:
	    err_typ = "Multiple packets received from TELL1 board";
	    handle = true;
	    break;
	  case ShortPkt:           // do not attempt to decode
	    err_typ = "Short packet received from TELL1 board";
	    handle = false;
	    break;
	  case WrongPartitionID:
	    err_typ = "Packet with wrong paretition ID received from TELL1 board";
	    handle = true;
	    break;
	  case WrongPackingFactor: // do not attempt to decode
	    err_typ = "MEP fragment with wrong packing factor received";
	    handle = false;
	    break;
	  case EmptyMEP:
	    err_typ = "Empty MEP received from TELL1 board";
	    handle = true;
	    break;
	  case MissingOdin:
	    err_typ = "MEP contains no ODIN information";
	    handle = false;
	    break;
	  default:
	    handle = false;
	    break;
	  }
	  if ( handle ) {
	    log << MSG::ERROR << "Found ERROR MEP. Start handling error:" << err_typ << endmsg;
	    if ( m_handleErrs == 1) saveMEP(e.data,e.len,e.mask).ignore();
	    if ( m_handleErrs == 2) saveEvents(e.data,e.len,e.mask).ignore();
	  }
	  else {
	    log << MSG::ERROR << "Found ERROR MEP. Error " << err_typ << endmsg;
	    log << "This error cannot be handled. DATA DISCARDED." << endmsg;
	  }
	}
	else {
	  log << MSG::ERROR << "Found ERROR MEP -- Error handling is not enabled -- DATA DISCARDED." << endmsg;
	}
	++m_errMEPCount;
	mep_decrement(id, ev, 1);
      }
      else {
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
    }
  } catch( exception& e)  {
    return error(string("Failed to access MEP buffers:")+e.what());
  }
  return StatusCode::SUCCESS;
}
