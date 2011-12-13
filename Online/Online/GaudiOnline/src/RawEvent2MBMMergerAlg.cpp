// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawEvent2MBMMergerAlg.cpp,v 1.16 2010-09-22 13:38:19 frankb Exp $
//  ====================================================================
//  DecisionSetterAlg.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiOnline/MEPManager.h"
#include "GaudiOnline/FileIdInfo.h"
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
  class RawEvent2MBMMergerAlg : public MDFWriter, virtual public IIncidentListener   {
    /// Reference to MEP manager service
    IMEPManager*  m_mepMgr;
    /// Reference to BM producer
    Producer*     m_prod;
    /// Reference to incident service
    IIncidentSvc* m_incidentSvc;

    /// Property: output buffer name
    string        m_bufferName;
    /// Property: TES location with banks to find FID
    string        m_fidLocation;
    /// Property: Timeout incident name
    string        m_tmoIncident;
    /// Property: Word 4 of trigger mask (Routing bits)
    unsigned int  m_routingBits;
    /// Property: Word 4 of trigger mask for timeout events (Routing bits)
    unsigned int  m_timeoutBits;
    /// Property: Force writing the event on timeout incident
    int           m_forceEvent;
    /// Monitoring quantity: Counter of number of bytes sent
    int           m_bytesDeclared;
    /// Monitoring quantity: Counter for number of events with errors (Timeout, ...)
    int           m_badEvents;
    /// Flag to add FID if required
    int           m_addFID;
    /// Flag to indicate that a timeout occurred during processing
    int           m_eventTMO;
    
    typedef const vector<RawBank*> _V;

  public:
    /// Standard algorithm constructor
    RawEvent2MBMMergerAlg(const string& nam, ISvcLocator* pSvc);

    /// Standard Destructor
    virtual ~RawEvent2MBMMergerAlg()      {                                 }

    /// IInterface implementation: Query interface
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppIf);

    /// Algorithm overload: initialize
    virtual StatusCode initialize();

    /// Algorithm overload: start
    virtual StatusCode start();    

    /// Algorithm overload: stop
    virtual StatusCode stop();

    /// Algorithm overload: finalize
    virtual StatusCode finalize(); 

    /// Algorithm overload: sysReinitializew
    virtual StatusCode sysReinitialize()   {
      return StatusCode::SUCCESS;
    }

    /// Algorithm overload: execute
    virtual StatusCode execute();

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);

    /// MDFIO overload: Allocate space for IO buffer
    virtual MDFIO::MDFDescriptor getDataSpace(void* const ioDesc, size_t len);

    /// MDFIO overload: Write MDF record from serialization buffer
    virtual StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len);

    /// Local implementation:
    pair<MDFHeader*,const RawBank*> getHeaderFromRaw(bool with_hltbits);
    pair<MDFHeader*,const RawBank*> getHeaderFromBanks(bool with_hltbits);
    pair<MDFHeader*,const RawBank*> getHeaderFromRecords(bool with_hltbits);
    pair<MDFHeader*,const RawBank*> getHeader(bool with_hltbits);

    StatusCode writeEvent(unsigned int routingBits);

    /// Issue error condition
    StatusCode error(const string& msg) const {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << msg << endmsg;
      return StatusCode::FAILURE;
    }
  };
}

using namespace LHCb;

/// Standard algorithm constructor
RawEvent2MBMMergerAlg::RawEvent2MBMMergerAlg(const string& nam, ISvcLocator* pSvc)
  :  MDFWriter(MDFIO::MDF_NONE, nam, pSvc), m_mepMgr(0), m_prod(0), m_incidentSvc(0)
{
  declareProperty("Buffer",         m_bufferName="RESULT");
  declareProperty("TimeoutIncident",m_tmoIncident="DAQ_TIMEOUT");
  declareProperty("RoutingBits",    m_routingBits=NO_ROUTING);
  declareProperty("TimeoutBits",    m_timeoutBits=NO_ROUTING);
  declareProperty("AddFID",         m_addFID=0);
  declareProperty("ForceEvent",     m_forceEvent=0);
  declareProperty("FIDLocation",    m_fidLocation="/Event");
  declareProperty("Silent",         m_silent);
}

/// IInterface implementation: Query interface
StatusCode RawEvent2MBMMergerAlg::queryInterface(const InterfaceID& riid,void** ppIf) {
  if ( riid.versionMatch(IIncidentListener::interfaceID()) )  {
    *ppIf = (IIncidentListener*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return MDFWriter::queryInterface(riid, ppIf);
}

/// Algorithm overload: initialize
StatusCode RawEvent2MBMMergerAlg::initialize()   {
  StatusCode status = service("MEPManager",m_mepMgr);
  if ( !status.isSuccess() )   {
    return error("Failed to access MEPManager service.");
  }
  if ( m_mepMgr->connectWhen() == "initialize" ) {
    m_prod = m_mepMgr->createProducer(m_bufferName,RTL::processName());
    if ( 0 == m_prod ) {
      return error("Failed to create event producer for buffer:"+m_bufferName);
    }
  }
  status = service("IncidentSvc",m_incidentSvc,true);
  if ( !status.isSuccess() )  {
    return error("Cannot access incident service.");
  }
  if ( m_timeoutBits != NO_ROUTING ) {
    m_incidentSvc->addListener(this,m_tmoIncident);
  }
  declareInfo("SpaceCalls", m_spaceActions=0,  "Total number successful space requests.");
  declareInfo("SpaceErrors",m_spaceErrors=0,   "Total number failed space requests.");
  declareInfo("EventsOut",  m_writeActions=0,  "Total number of events declared to BM.");
  declareInfo("ErrorsOut",  m_writeErrors=0,   "Total number of declare errors.");
  declareInfo("BytesOut",   m_bytesDeclared=0, "Total number of bytes declared to BM.");
  declareInfo("BadEvents",  m_badEvents=0,     "Total number of bytes declared to BM.");
  m_eventTMO = 0;
  return StatusCode::SUCCESS;
}

/// Algorithm overload: start
StatusCode RawEvent2MBMMergerAlg::start()     {    
  if ( m_mepMgr->connectWhen() == "start" ) {
    m_prod = m_mepMgr->createProducer(m_bufferName,RTL::processName());
    if ( 0 == m_prod ) {
      return error("Failed to create event producer for buffer:"+m_bufferName);
    }
  }
  return StatusCode::SUCCESS;
}

/// Algorithm overload: stop
StatusCode RawEvent2MBMMergerAlg::stop()     {    
  if ( m_mepMgr->connectWhen() == "start" ) {
    if ( m_prod )  {
      delete m_prod;
      m_prod = 0;
    }
  }
  return StatusCode::SUCCESS;
}
/// Algorithm overload: finalize
StatusCode RawEvent2MBMMergerAlg::finalize()     {    
  if ( m_incidentSvc )  {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  if ( monitorSvc() )  {
    monitorSvc()->undeclareAll(this);
  }
  if ( m_prod )  {
    delete m_prod;
    m_prod = 0;
  }
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode RawEvent2MBMMergerAlg::execute() {
  unsigned int bits = m_routingBits;
  if ( m_eventTMO != 0 && m_timeoutBits != NO_ROUTING ) {
    bits = m_timeoutBits;
    m_eventTMO = 0;
  }
  return writeEvent(bits);
}

/// Incident handler implemenentation: Inform that a new incident has occured
void RawEvent2MBMMergerAlg::handle(const Incident& inc)    {
  if ( inc.type() == m_tmoIncident )  {
    MsgStream err(msgSvc(), name());
    err << MSG::WARNING << inc.type() << ": Write data after timeout during event processing." << std::endl;
    ++m_badEvents;
    m_eventTMO = 1;
    if ( m_forceEvent ) {
      if ( writeEvent(m_timeoutBits).isSuccess() ) {
	m_eventTMO = 0;
      }
    }
  }
}

/// MDFIO overload: Allocate space for IO buffer
MDFIO::MDFDescriptor RawEvent2MBMMergerAlg::getDataSpace(void* const /* ioDesc */, size_t len)  {
  try   {
    // This is a bit hacky: re-use silent flag to do unblocking opertions, which may fail
    int ret = m_silent 
      ? m_prod->getSpaceTry(len+additionalSpace())
      : m_prod->getSpace(len+additionalSpace());
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
  if ( !m_silent ) error("Failed to get space for buffer manager.");
  setFilterPassed(false);
  return MDFDescriptor(0,-1);
}

pair<MDFHeader*,const RawBank*> RawEvent2MBMMergerAlg::getHeaderFromRaw(bool with_hltbits) {
  DataObject* pDO = 0;
  StatusCode sc = eventSvc()->retrieveObject(m_bankLocation,pDO);
  if ( sc.isSuccess() ) {
    RawEvent *raw = (RawEvent*)pDO;
    const _V& bnks = raw->banks(RawBank::DAQ);
    for(_V::const_iterator i=bnks.begin(); i != bnks.end(); ++i)  {
      RawBank* bank = *i;
      if ( bank->version() == DAQ_STATUS_BANK && bank->type() == RawBank::DAQ )  {
	if ( with_hltbits ) {
	  const _V& bits = raw->banks(RawBank::HltRoutingBits);
	  for(_V::const_iterator i=bits.begin(); i != bits.end(); ++i)  {
	    return make_pair(bank->begin<MDFHeader>(),*i);
	  }
	}
	return pair<MDFHeader*,const RawBank*>(bank->begin<MDFHeader>(),0);
      }
    }
  }
  return pair<MDFHeader*,const RawBank*>(0,0);
}

pair<MDFHeader*,const RawBank*> RawEvent2MBMMergerAlg::getHeaderFromBanks(bool with_hltbits) {
  pair<const char*,int> data = getDataFromAddress();
  RawBank* header = (RawBank*)data.first;
  MDFHeader*  mdf = header ? header->begin<MDFHeader>() : 0;
  if ( header && with_hltbits ) {
    const char *s = data.first;
    const char *e = ((char*)mdf)+mdf->recordSize();
    for(RawBank* bank=header; s < e; bank = (RawBank*)(s += bank->totalSize()))  {
      if ( bank->type() == RawBank::HltRoutingBits ) {
	return make_pair(mdf,bank);
      }
    }
  }
  return pair<MDFHeader*,const RawBank*>(mdf,0);
}

pair<MDFHeader*,const RawBank*> RawEvent2MBMMergerAlg::getHeaderFromRecords(bool with_hltbits) {
  pair<const char*,int> data = getDataFromAddress();
  MDFHeader *mdf = (MDFHeader*)data.first;
  if ( data.first && with_hltbits ) {
    const char *s   = ((char*)mdf)+MDFHeader::sizeOf(mdf->headerVersion());
    const char *e   = ((char*)mdf)+mdf->recordSize();
    for(RawBank* bank=(RawBank*)s; s < e; bank = (RawBank*)(s += bank->totalSize()))  {
      if ( bank->type() == RawBank::HltRoutingBits ) {
	return make_pair(mdf,bank);
      }
    }
  }
  return pair<MDFHeader*,const RawBank*>(mdf,0);
}

pair<MDFHeader*,const RawBank*> RawEvent2MBMMergerAlg::getHeader(bool with_hltbits) {
  switch(m_inputType)   {
  case MDFIO::MDF_NONE:
    return getHeaderFromRaw(with_hltbits);
  case MDFIO::MDF_BANKS:
    return getHeaderFromBanks(with_hltbits);
  case MDFIO::MDF_RECORDS:
    return getHeaderFromRecords(with_hltbits);
  default:
    return pair<MDFHeader*,const RawBank*>(0,0);
  }
}

/// MDFIO overload: Write MDF record from serialization buffer
StatusCode RawEvent2MBMMergerAlg::writeBuffer(void* const ioDesc, const void* data, size_t len)  {
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

      // If the buffer got compressed, the data are in a temporary buffer
      // and need to be copied into the MBM buffer
      if ( m_compress ) {
	::memcpy(e.data, data, len);
      }
      ::memcpy(e.mask,h->subHeader().H1->triggerMask(),sizeof(e.mask));

      if ( m_addFID )   {
	RawBank* fid_bank = 0;
	const FileIdInfo *src_info = 0;
	FileIdInfo *fid_info;
	std::pair<const char*,int> buff = getDataFromAddress();
	if ( buff.first )   {
	  MDFHeader* src_hdr = ((RawBank*)buff.first)->begin<MDFHeader>();
	  RawBank*   src_fid = (RawBank*)(((char*)src_hdr) + src_hdr->size2());
	  size_t rec_len = h->recordSize();
	  fid_bank = (RawBank*)((char*)e.data + len);
	  size_t fid_len = src_fid->totalSize();
	  fid_info = fid_bank->begin<FileIdInfo>();
	  src_info = src_fid->begin<FileIdInfo>();
	  ::memcpy((char*)e.data + len,src_fid,fid_len);
	  e.len += fid_len;
	  fid_info->checksum = h->checkSum();
	  h->setChecksum(0);
	  h->setSize(h->size()+fid_len);
	  h->setSize2(rec_len);
	}
	if ( 0 == fid_bank || 0 == src_info ) {
	  return error("Failed to access FID bank from input data!");
	}
      }

      int ret;
      if ( m_silent )   {
	ret = m_prod->declareEventTry();
	if ( MBM_NORMAL == ret ) {
	  ret = m_prod->sendSpace();
	}
	else {
	  setFilterPassed(false);
	  return StatusCode::SUCCESS;
	}
      }
      else {
	ret = m_prod->sendEvent();
      }
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
}

StatusCode RawEvent2MBMMergerAlg::writeEvent(unsigned int routingBits) {
  setupMDFIO(msgSvc(),eventSvc());
  pair<MDFHeader*,const RawBank*> h = getHeader(true);
  if ( h.first ) {
    const unsigned int* hmask = h.first->subHeader().H1->triggerMask();
    const unsigned int* tmask = h.second ? h.second->begin<unsigned int>() : hmask;
    unsigned int m[] = { tmask[0], tmask[1], tmask[2], routingBits != NO_ROUTING ? routingBits : hmask[3]};
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
  StatusCode sc = MDFWriter::execute();
  if ( m_silent && !sc.isSuccess() ) {
    sc.ignore();
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  return sc;
}

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEvent2MBMMergerAlg)
