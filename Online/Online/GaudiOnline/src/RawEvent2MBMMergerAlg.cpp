// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawEvent2MBMMergerAlg.cpp,v 1.9 2009-01-09 10:35:48 frankb Exp $
//  ====================================================================
//  DecisionSetterAlg.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/MEPManager.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "MDF/MDFWriter.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "RTL/rtl.h"

using namespace MBM;
using namespace std;
static const unsigned int NO_ROUTING = (unsigned int)~0x0;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class RawEvent2MBMMergerAlg 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RawEvent2MBMMergerAlg : public MDFWriter   {
    /// Reference to MEP manager service
    IMEPManager*  m_mepMgr;
    /// Reference to BM producer
    Producer*     m_prod;
    /// Property: output buffer name
    string        m_bufferName;
    /// Property: Word 4 of trigger mask (Routing bits)
    unsigned int  m_routingBits;
    /// Monitoring quantity: Counter of number of bytes sent
    int           m_bytesDeclared;

  public:
    /// Standard algorithm constructor
    RawEvent2MBMMergerAlg(const string& nam, ISvcLocator* pSvc)
    :  MDFWriter(MDFIO::MDF_NONE, nam, pSvc), m_mepMgr(0), m_prod(0)
    {
      declareProperty("Buffer",      m_bufferName="RESULT");
      declareProperty("RoutingBits", m_routingBits=NO_ROUTING);
    }
    /// Standard Destructor
    virtual ~RawEvent2MBMMergerAlg()      {                                 }
    /// Initialize
    virtual StatusCode initialize()   {
      StatusCode status = service("MEPManager",m_mepMgr);
      if ( !status.isSuccess() )   {
	return error("Failed to access MEPManager service.");
      }
      m_prod = m_mepMgr->createProducer(m_bufferName,RTL::processName());
      if ( 0 == m_prod ) {
	return error("Failed to create event producer for buffer:"+m_bufferName);
      }
      declareInfo("SpaceCalls", m_spaceActions=0,  "Total number successful space requests.");
      declareInfo("SpaceErrors",m_spaceErrors=0,   "Total number failed space requests.");
      declareInfo("EventsOut",  m_writeActions=0,  "Total number of events declared to BM.");
      declareInfo("ErrorsOut",  m_writeErrors=0,   "Total number of declare errors.");
      declareInfo("BytesOut",   m_bytesDeclared=0, "Total number of bytes declared to BM.");
      return StatusCode::SUCCESS;
    }

    /// Finalize
    virtual StatusCode finalize()     {    
      if ( monitorSvc() ) monitorSvc()->undeclareAll(this);
      if ( m_mepMgr )  {
        m_mepMgr->release();
        m_mepMgr = 0;
      }
      if ( m_prod )  {
        delete m_prod;
        m_prod = 0;
      }
      return StatusCode::SUCCESS;
    }
    virtual StatusCode sysReinitialize()   {
      return StatusCode::SUCCESS;
    }

    pair<MDFHeader*,const RawBank*> getHeader(bool with_hltbits) {
      typedef const vector<RawBank*> _V;
      pair<const char*,int> data;
      pair<MDFHeader*,const RawBank*> res(0,0);
      switch(m_inputType)   {
      case MDFIO::MDF_NONE: {
	RawEvent *raw = 0;
	StatusCode sc = eventSvc()->retrieveObject(m_bankLocation,(DataObject*&)raw);
	if ( sc.isSuccess() ) {
	  const _V& bnks = raw->banks(RawBank::DAQ);
	  for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
	    RawBank* b = *i;
	    if ( b->version() == DAQ_STATUS_BANK )  {
	      res.first = b->begin<MDFHeader>();
	      break;
	    }
	  }
	  if ( with_hltbits ) {
	    const _V& bits = raw->banks(RawBank::HltRoutingBits);
	    for(_V::const_iterator i=bits.begin(); i != bits.end(); ++i)  {
	      res.second = *i;
	      break;
	    }
	  }
	}
	return res;
      }
      case MDFIO::MDF_BANKS: {
	data = getDataFromAddress();
	if ( data.first == 0 || data.second<0 ) {
	  return res;
	}
	res.first = data.first ? ((RawBank*)data.first)->begin<MDFHeader>() : 0;
	if ( with_hltbits ) {
	  const char *s = data.first;
	  const char *e = ((char*)res.first)+res.first->recordSize();
	  while (s < e)  {
	    RawBank* b = (RawBank*)s;
	    if ( b->type() == RawBank::HltRoutingBits ) {
	      res.second = b;
	      break;
	    }
	    s += b->totalSize();
	  }
	}
	return res;
      }
      case MDFIO::MDF_RECORDS:
	data = getDataFromAddress();
	if ( data.first == 0 || data.second<0 ) {
	  return res;
	}
	res.first = (MDFHeader*)data.first;
	if ( with_hltbits ) {
	  const char *s   = ((char*)res.first)+MDFHeader::sizeOf(res.first->headerVersion());
	  const char *e = ((char*)res.first)+res.first->recordSize();
	  while (s < e)  {
	    RawBank* b = (RawBank*)s;
	    if ( b->type() == RawBank::HltRoutingBits ) {
	      res.second = b;
	      break;
	    }
	    s += b->totalSize();
	  }
	}
	return res;
      default:
	return res;
      }
      return res;
    }

    virtual StatusCode execute() {
      setupMDFIO(msgSvc(),eventSvc());
      pair<MDFHeader*,const RawBank*> h = getHeader(true);
      if ( h.first ) {
	const unsigned int* hmask = h.first->subHeader().H1->triggerMask();
	const unsigned int* tmask = h.second ? h.second->begin<unsigned int>() : hmask;
	unsigned int m[] = { tmask[0], tmask[1], tmask[2], m_routingBits != NO_ROUTING ? m_routingBits : hmask[3]};
	h.first->subHeader().H1->setTriggerMask(m);
      }
      switch(m_inputType)   {
      case MDFIO::MDF_NONE:
	break;   // Nothing to do...the checksum will anyhow be calculated
      case MDFIO::MDF_BANKS:
	break;
      case MDFIO::MDF_RECORDS:
	break;
      default:
	break;
      }
      return MDFWriter::execute();
    }

    /// Issue error condition
    StatusCode error(const string& msg) const {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << msg << endmsg;
      return StatusCode::FAILURE;
    }

    /// Allocate space for IO buffer
    virtual MDFIO::MDFDescriptor getDataSpace(void* const /* ioDesc */, size_t len)  {
      try {
	int ret = m_prod->getSpace(len+additionalSpace());
	if ( ret == MBM_NORMAL ) {
	  EventDesc& e = m_prod->event();
	  e.len = len;
	  return MDFDescriptor((char*)e.data,len);
	}
      }
      catch(exception& e)  {
	error("Got exception when asking for BM space:"+string(e.what()));
      }
      catch(...)  {
	error("Got unknown exception when asking for BM space");
      }
      error("Failed to get space for buffer manager.");
      return MDFDescriptor(0,-1);
    }

    /// Write MDF record from serialization buffer
    StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len)  {
      try {
	EventDesc& e = m_prod->event();
	if ( 0 == e.data )   {
	  // If we end up here, the data were assembled from the raw event, but rather
	  // taken from an already existing MDF or BANK buffer.
	  // In this event we have to copy the data to the requested space.
	  MDFIO::MDFDescriptor dsc = getDataSpace(ioDesc,len);
	  if ( dsc.first == 0 ) {
	    return error("Failed to get space to copy data!");
	  }
	  ::memcpy(dsc.first,data,len);
	}
	if ( 0 != e.data ) {
	  RawBank*   b = (RawBank*)e.data;
	  MDFHeader* h = (MDFHeader*)b->data();
	  e.type       = EVENT_TYPE_EVENT;
	  e.len        = len;
	  ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));
	  int ret = m_prod->sendEvent();
	  if ( MBM_NORMAL == ret )   {
	    MsgStream log(msgSvc(),name());
	    e.len = 0;
	    e.data = 0;
	    m_bytesDeclared += len;
	    log << MSG::DEBUG << "Wrote data of size:" << len << " to buffer " << m_bufferName << endmsg;
	    return StatusCode::SUCCESS;
	  }
	  e.len = 0;
	  e.data = 0;
	  return error("Failed to declare event to buffer manager:"+m_bufferName);
	}
	return error("Failed to declare event - no data present!");
      }
      catch(exception& e)  {
	return error("Got exception when declaring event:"+m_bufferName+" "+string(e.what()));
      }
      catch(...)  {
	return error("Got unknown exception when declaring event:"+m_bufferName);
      }
      return StatusCode::FAILURE;
    }
  };
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEvent2MBMMergerAlg);
