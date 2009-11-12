//  ============================================================
//
//  MEPHolderSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Markus Frank
//
//  ===========================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPHolderSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MBM/Consumer.h"
#include "MBM/Producer.h"
#include "MBM/Requirement.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"


DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPHolderSvc)

using MBM::Consumer;
using MBM::EventDesc;
using MBM::Requirement;
using namespace std;
using namespace LHCb;
  
static void handle_errors(void* param, void* data,size_t length) {
  MEPHolderSvc* svc = (MEPHolderSvc*)param;
  svc->saveOnError(data,length).ignore();
}

// Standard Constructor
MEPHolderSvc::MEPHolderSvc(const string& nam, ISvcLocator* svc)
: OnlineService(nam, svc), m_mepMgr(0), m_consumer(0), m_producer(0), 
  m_receiveEvts(false), m_evtCount(0)
{
  declareProperty("HandleErrors",   m_handleErrs=0);
  declareProperty("Requirements",   m_req);
  declareProperty("ReleaseTimeout", m_releaseTMO=1000);
  declareProperty("MEPManager",     m_mepMgrName="LHCb::MEPManager/MEPManager");
  declareProperty("ErrorBuffer",    m_prodBuffer="SEND");
  declareProperty("RoutingBits",    m_routingBits=0x400);
}

// Standard Destructor
MEPHolderSvc::~MEPHolderSvc()   {
}

// IInterface implementation: Query interface
StatusCode MEPHolderSvc::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid == IRunable::interfaceID() )  {
    *ppIf = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineService::queryInterface(riid, ppIf);
}


/// Save event data on error
StatusCode MEPHolderSvc::saveOnError(void* data, size_t length) {
  return (m_handleErrs == 1) ? saveMEP(data,length) : saveEvents(data,length);
}

/// Save event data on error
StatusCode MEPHolderSvc::saveEvents(void* data, size_t length) {
  if ( data || length ) {
    unsigned int partitionID = 0;
    typedef std::map<unsigned int, RawEvent*> SubEvents;
    typedef std::vector<RawBank*> Banks;
    MEPEVENT* ev = (MEPEVENT*)data;
    LHCb::MEPEvent* me = (LHCb::MEPEvent*)ev->data;
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
      int ret = m_producer->getSpace(ev_len);
      if ( MBM_NORMAL == ret )    {
	unsigned int trMask[] = {0x0,0x0,0x0,m_routingBits};
	EventDesc& e = m_producer->event();
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
	  ret = m_producer->sendEvent();
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
StatusCode MEPHolderSvc::saveMEP(void* data, size_t length) {
  if ( data || length ) {
    static const size_t BANK_LIMIT = 60*1024;
    info("Got request to save MEP with ERRORS.....");    
    const char* start = (const char*)data, *end = start+length;
    size_t len = sizeof(MEPEVENT)+((length/BANK_LIMIT)+1)*sizeof(RawBank);
    int ret = m_producer->getSpace(len+length);
    if ( ret == MBM_NORMAL )    {
      size_t hdrSize = 0;
      EventDesc& e = m_producer->event();
      char* target = (char*)e.data;
      unsigned int trMask[] = {0x0,0x0,0x0,m_routingBits};
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
      int ret = m_producer->sendEvent();
      if ( MBM_NORMAL == ret )   {
	return StatusCode::SUCCESS;
      }
      return error("Failed to declare event to ERROR stream");
    }
    return error("Failed to get space for ERROR stream");
  }
  return StatusCode::SUCCESS;
}

// Incident handler implemenentation: Inform that a new incident has occured
void MEPHolderSvc::handle(const Incident& inc)    {
  info("Got incident:"+inc.source()+" of type "+inc.type());
  if ( inc.type() == "DAQ_CANCEL" )  {
    m_receiveEvts = false;
    m_mepMgr->cancel();
  }
  else if ( inc.type() == "DAQ_ENABLE" )  {
    m_receiveEvts = true;
  }
}

StatusCode MEPHolderSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  if ( sc.isSuccess() )  {
    declareInfo("MEPsIn",m_evtCount=0,"Number of events received from network");
    if ( service(m_mepMgrName,m_mepMgr).isSuccess() )  {
      incidentSvc()->addListener(this,"DAQ_CANCEL");
      try {
        int partID = m_mepMgr->partitionID();
        const string& proc = m_mepMgr->processName();
        MEPID id = m_mepMgr->mepID();
        m_consumer = new Consumer(id->mepBuffer,proc,partID);
        for(Requirements::iterator i=m_req.begin(); i!=m_req.end(); ++i)  {
          Requirement r;
          r.parse(*i);
          m_consumer->addRequest(r);
        }
        if ( m_handleErrs ) {
	  ::mep_set_error_call(id, this, handle_errors);
	  m_producer = m_mepMgr->createProducer(m_prodBuffer,RTL::processName());
	  if ( 0 == m_producer ) {
	    return error("Failed to create error event producer for buffer:"+m_prodBuffer);
	  }
	}
        return StatusCode::SUCCESS;
      }
      catch( exception& e)  {
        return error("Failed setup MEP buffers:"+string(e.what()));
      }
    }
    return error("Failed to access MEP manager service.");
  }
  return error("Failed to initialize service base class.");
}

StatusCode MEPHolderSvc::finalize()  {
  undeclareInfo("MEPsIn");
  if ( m_mepMgr && m_handleErrs ) {
    MEPID id = m_mepMgr->mepID();
    ::mep_set_error_call(id, 0, 0);
    if ( m_producer )  {
      delete m_producer;
      m_producer = 0;
    }
  }
  if ( m_consumer    )  {
    delete m_consumer;
    m_consumer = 0;
  }
  if ( m_mepMgr      )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return OnlineService::finalize();
}

// Process single event
StatusCode MEPHolderSvc::run()  {
  m_receiveEvts = true;
  MEPID id = m_mepMgr->mepID();
  int m_procType = 1;
  try  {
    debug("Starting event loop ....");
    if ( m_procType == 1 )  {
      while(m_receiveEvts)  {
        mep_scan(id,0);
        if ( m_receiveEvts )  {
          ::lib_rtl_usleep(m_releaseTMO);
        }
      }
      debug("Leaving event loop ....");
      return StatusCode::SUCCESS;
    }

    for(int sc=m_consumer->getEvent();sc==MBM_NORMAL && m_receiveEvts; sc=m_consumer->getEvent())  {
      const EventDesc& evt = m_consumer->event();
      MEPEVENT* e = (MEPEVENT*)(int*)evt.data;
      if ( e->magic != mep_magic_pattern() )  {
        error("Bad magic MEP pattern !!!!");
      }
      while ( 1 )  {
        if ( e->refCount <= 1 )    {
          if ( e->refCount != 1 )    {
            error("MEP release [%d] Event at address %08X MEP:%08X [%d] Pattern %08X",
              e->refCount,id->mepStart+e->begin,e,e->evID,e->magic);
          }
          break;
        }
        lib_rtl_usleep(100);
        if ( !m_receiveEvts )  {
          if ( e->refCount > 1 )    {
            lib_rtl_sleep(m_releaseTMO);
          }
          break;
        }
      }
      m_evtCount++;
      m_consumer->freeEvent();
    }
    debug("Leaving event loop ....");
    return StatusCode::SUCCESS;
  }
  catch(const exception& e)  {
    return error("Exception during event processing:"+string(e.what()));
    return StatusCode::FAILURE;
  }
  catch(...)  {
    return error("Unknown exception during event processing.");
  }
}
