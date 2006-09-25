//	============================================================
//
//	MEPRxSvc.cpp
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Niko Neufeld
//                  using code by B. Gaidioz and M. Frank
//
//      Version   : $Id: MEPRxSvc.cpp,v 1.32 2006-09-25 14:45:18 frankb Exp $
//
//	===========================================================
#ifdef _WIN32
#define snprintf _snprintf
#endif

#include <cstring>
#include <cstdlib>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "Event/RawBank.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "GaudiOnline/MEPRxSvc.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiOnline/MEPRxSys.h"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "NET/IPHeader.h"
#include "MBM/MepProducer.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"

namespace LHCb {
  class MEPEvent;
  StatusCode RTTC2Current( const MEPEvent* me );
}

#define MAX_R_PACKET (0x10000 + 20)
#define MEP_SENT  MBM_NORMAL
#define MEP_ADDED (MEP_SENT + 1)
#define IP_HEADER_LEN 20
#define IP_PROTO_HLT  0xF2

#define MEP_REQ_TOS 0xFD  /* the D is historic from the MDPs :-) */
#ifndef MEP_ADD_ERROR
#define MEP_ADD_ERROR -9999
#endif
/* maximum number of sources (TELL1, RS, UKL1) */
#ifndef MAX_SRC
#define MAX_SRC 320
#endif 

#define RAWBANKSIZE (sizeof(RawBank) - sizeof(int)) /* f*** C99 */ 
#ifdef _WIN32
#define ERRMSG(a,x) do {  \
  a <<	MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __PRETTY_FUNCTION__ << ":" << __LINE__ << endmsg;} while(0);
#endif

#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define printnum(n, s) n << s << (n == 1 ? "" : "s")

#define HDR_LEN (IP_HEADER_LEN + sizeof(LHCb::MEPHdr)) 

static MEPReq mepreq;

namespace LHCb  {
  struct MEPRx  : public MEP::Producer {
    MEPRxSvc*       m_parent;
    // parameters
    int             m_refCount;
    int             m_spaceSize;
    int             m_age;
    int             m_nSrc;
    int             m_nrx;
    // run-time
    u_int32_t       m_l0ID;
    u_int32_t       m_brx; 
    u_int16_t       m_pf;
    int             m_spaceRC;
    int             m_eventType;
    int             m_seen[MAX_SRC]; 
    int             m_maxMsForGetSpace;
    bool            m_dry;
    MsgStream       m_log;
    DAQErrorBankEntry m_eEntry[MAX_SRC];
    RawBank *m_rawBufHdr, *m_MDFBankHdr;

  public:
    MEPRx(const std::string &nam, MEPRxSvc *parent);
    ~MEPRx()  {
      delete[] (u_int8_t *) m_rawBufHdr;
      delete[] (u_int8_t *) m_MDFBankHdr;
      exclude();
    }    
    static int spaceTimeOut(void *p) {
      MEPRx *self = (MEPRx *) p;
      if (self->m_spaceRC != -100) return 0;
      mbm_cancel_request(self->m_bmid);
      return 1;
    }
    int spaceRearm(int) {
      m_eventType = EVENT_TYPE_MEP;
      m_brx = m_nrx = 0; 
      memset(m_seen, 0, m_nSrc * sizeof(int));
      m_spaceRC = MEP::Producer::spaceRearm(m_spaceSize);
      return m_spaceRC;
    }
    // Run the application in synchonous mode
    int runSynchronous() {
      int status = spaceRearm(0);
      return (status == MBM_NORMAL) ? spaceAction() : status;
    }
    void multipleSrc() {
      m_eventType = EVENT_TYPE_ERROR;
      m_log << MSG::ERROR << "Multiple event from source" << endmsg; 
    } 
    void badPkt(MEPRxSvc::DAQError /* type */ ) {
      m_eventType = EVENT_TYPE_ERROR;
      m_log << MSG::ERROR << "Bad event from source" << endmsg; 
    }
    void warnSwap(void) {
      if ((m_parent->m_swappedMEP)++ < 10) {
        m_log << MSG::WARNING << "MEP with swapped packing_factor / len"
          " detected. Fixed!" << endmsg;
      }
    }

#define RAWBHDRSIZ (sizeof(RawBank) - 4)
#define MEPHDRSIZ   sizeof(MEPHdr)
#define MEPFHDRSIZ  sizeof(MEPFrgHdr)  

    /// We create a MEP fragment with the required error bank
    int createDAQErrorBankEntries();
    int setupDAQErrorBankHdr();
    int createDAQErrorMEP(u_int8_t *buf, int nEvt);
    int setupMDFBank();
    int createMDFMEP(u_int8_t *buf, int nEvt);
    void incompleteEvent();
    int spaceAction();
    int addMEP(const MEPHdr *hdr, int srcid);
  };
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPRxSvc)

using namespace LHCb;

MEPRx::MEPRx(const std::string &nam, MEPRxSvc *parent) 
: MEP::Producer(nam, parent->partitionID()), m_parent(parent),  
  m_refCount(parent->mepRefcount()), 
  m_spaceSize(parent->spaceSize()),
  m_nSrc(parent->numberOfSources()), 
  m_log(parent->msgSvc(),nam)
{
  m_flags = USE_MEP_BUFFER;
  include();
  m_bmid = m_mepID->mepBuffer;
  m_log << MSG::DEBUG << std::hex << std::showbase 
    << "MEP buffer start: "       << m_mepID->mepStart 
    << "\nEVENT  buffer start: "  << m_mepID->evtStart
    << "\nRESULT buffer start: "  << m_mepID->resStart << endmsg
    << MSG::DEBUG << "Buffer space: " << m_spaceSize << " bytes" << endmsg;
  m_eventType  = EVENT_TYPE_MEP;
  m_rawBufHdr  = (class RawBank *)new u_int8_t[sizeof(RawBank)+16];
  size_t  len  =  sizeof(RawBank) + MDFHeader::sizeOf(1);
  m_MDFBankHdr = (class RawBank *) new u_int8_t[len];
  m_MDFBankHdr->setType(RawBank::DAQ);
  m_MDFBankHdr->setSize(MDFHeader::sizeOf(1));
  m_MDFBankHdr->setVersion(DAQ_STATUS_BANK);
  m_MDFBankHdr->setSourceID(1024);
  m_MDFBankHdr->setMagic();
}

int MEPRx::createDAQErrorBankEntries() {
  int j = 0;
  for (int i = 0; i < m_nSrc; ++i) {
    if (m_seen[i] == 1) continue;
    m_eEntry[j].m_srcID     = i;
    m_eEntry[j].m_srcIPAddr = m_parent->m_srcAddr[i];
    m_eEntry[j].m_errorType = MEPRxSvc::MissingSrc;
    m_eEntry[j].m_pktData   = NULL;
    j++;
  }
  return j;
}

int MEPRx::setupDAQErrorBankHdr() {
  int n = createDAQErrorBankEntries();
  int size = n * sizeof(DAQErrorBankEntry) + RAWBHDRSIZ;
  m_rawBufHdr->setMagic();
  m_rawBufHdr->setSourceID(m_parent->sourceID());
  m_rawBufHdr->setVersion(DAQ_ERR_BANK_VERSION);
  m_rawBufHdr->setType(RawBank::DAQ);
  m_rawBufHdr->setSize(size);
  return (size%4) ? size+4 - (size%4) : size;
} 

void MEPRx::incompleteEvent() {
  MEPEVENT* e = (MEPEVENT*)event().data;
  if ((m_parent->m_incEvt)++ < 10) {
    m_log << MSG::ERROR << "Incomplete Event" << endmsg;
  }
  return; // ????? Niko what's this ?
  u_int8_t *buf = (u_int8_t*)e->data + m_brx + 4 + IP_HEADER_LEN; 
  m_brx += createDAQErrorMEP(buf, m_pf) + IP_HEADER_LEN;
  return;
}

int MEPRx::setupMDFBank() {
  unsigned int mask[] = {~0,~0,~0,~0};
  MDFHeader* hdr = (MDFHeader *) m_MDFBankHdr->data();
  hdr->setSize(0);
  hdr->setChecksum(0);
  hdr->setCompression(0);
  hdr->setHeaderVersion(1);
  hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
  MDFHeader::SubHeader h = hdr->subHeader();
  h.H1->setTriggerMask(mask);
  h.H1->setRunNumber(0);
  h.H1->setOrbitNumber(0);
  h.H1->setBunchID(0);
  return m_MDFBankHdr->totalSize();
}

int MEPRx::createMDFMEP(u_int8_t *buf, int nEvt) {
  struct MEPHdr *meph = (struct MEPHdr *) buf;
  size_t banksize = setupMDFBank();
  meph->m_l0ID    = m_l0ID;
  meph->m_totLen  = MEPHDRSIZ +  nEvt * (MEPFHDRSIZ + banksize);
  meph->m_nEvt    = nEvt;
  buf += MEPHDRSIZ;
  for(int i = 0; i < nEvt; ++i) {
    struct MEPFrgHdr *frgh = (struct MEPFrgHdr *) buf;
    frgh->m_l0IDlow = 0xFFFF & (m_l0ID + i);
    frgh->m_len = banksize;
    buf += MEPFHDRSIZ;
    memcpy(buf, m_MDFBankHdr, banksize);
    buf += banksize;
  }
  return meph->m_totLen;
} 

int MEPRx::createDAQErrorMEP(u_int8_t *buf, int nEvt) {
  return 0;
  /*
  struct MEPHdr *meph = (struct MEPHdr *) buf;
  int banksize = setupDAQErrorBankHdr();
  meph->m_l0ID = m_l0ID;
  meph->m_totLen = MEPHDRSIZ +  nEvt * (MEPFHDRSIZ + banksize);
  meph->m_nEvt = nEvt;
  buf += MEPHDRSIZ;
  for (int i = 0; i < nEvt; ++i) {
  struct MEPFrgHdr *frgh = (struct MEPFrgHdr *) buf;
  frgh->m_l0IDlow = 0xFFFF & (m_l0ID + i);
  frgh->m_len = banksize;
  buf += MEPFHDRSIZ;
  memcpy(buf, m_rawBufHdr, RAWBHDRSIZ);
  buf += RAWBHDRSIZ;
  memcpy(buf, m_eEntry, banksize - RAWBHDRSIZ);
  buf += (banksize - RAWBHDRSIZ);
  }
  return meph->m_totLen;
  */
} 

int MEPRx::spaceAction() { 
  static  int id = -1; 
  MBM::EventDesc& dsc = event();
  MEPEVENT* e = (MEPEVENT*)dsc.data;
  MEPEvent* m = (MEPEvent*)e->data;
  if (m_nrx != m_nSrc) incompleteEvent();
  /// Add MDF bank
  u_int8_t *buf = (u_int8_t*)e->data + m_brx + 4 + IP_HEADER_LEN; 
  m_brx += createMDFMEP(buf, m_pf) + IP_HEADER_LEN;

  e->evID     = ++id;
  dsc.len     = m_brx + sizeof(MEPEVENT);
  dsc.mask[0] = dsc.mask[1] = dsc.mask[2] = dsc.mask[3] = 0xffffffff;
  dsc.type    = m_eventType;    
  m->setSize(m_brx);
  if (m_parent->m_RTTCCompat) RTTC2Current(m);
  declareEvent();
  return sendSpace();
}

int MEPRx::addMEP(const MEPHdr *hdr, int srcid) {
  MEPEVENT* e = (MEPEVENT*)event().data;
  if (m_seen[srcid]) multipleSrc();
  m_seen[srcid] = 1;		   
  // here a lot of tests should be done
  if (m_nrx == 0) {
    e->refCount    = m_refCount;
    e->evID        = hdr->m_l0ID;
    e->begin       = int(int(e)-m_mepID->mepStart);
    e->packing     = -1;
    e->valid       = 1;
    e->magic       = mep_magic_pattern();
    m_brx          = 0;
    m_pf           = hdr->m_nEvt;
  }
  int len = MEPRxSys::recv_msg((u_int8_t*)e->data + m_brx + 4, MAX_R_PACKET, 0);
  if (len < 0) {
    ERRMSG(m_log,"failed to receive message");
    return MEP_ADD_ERROR;
  }    
  MEPHdr *newhdr = (MEPHdr*) ((u_int8_t*)e->data + m_brx + 4 + IP_HEADER_LEN);
  m_parent->m_totRxOct += len;
  m_brx += len;

  // check for swapped m_nEvt field
  if (len != (hdr->m_totLen + IP_HEADER_LEN)) {
    if (len != (hdr->m_nEvt + IP_HEADER_LEN)) {
      badPkt(MEPRxSvc::ShortPkt);
    }
    else {
      std::swap(newhdr->m_totLen,newhdr->m_nEvt);
      if (m_nrx == 0) m_pf = newhdr->m_nEvt;
      warnSwap();
    }
  }
  m_nrx++; 
  if (m_pf != newhdr->m_nEvt) badPkt(MEPRxSvc::WrongPackingFactor);    
  return (m_nrx == m_nSrc) ? spaceAction() : MEP_ADDED;
}

// Standard Constructor
MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc), m_receiveEvents(false), m_incidentSvc(0)
{
  declareProperty("MEPBuffers",       m_MEPBuffers = 4);
  declareProperty("ethInterface",     m_ethInterface);
  declareProperty("IPNameOdin",       m_IPNameOdin = "");
  declareProperty("maxBadPktRatio",   m_maxBadPktRatio = 0.);
  declareProperty("pktSamplingCount", m_pktSamplingCount = 0);
  declareProperty("maxMsForGetSpace", m_maxMsForGetSpace = 10);
  declareProperty("IPSrc",            m_IPSrc);
  declareProperty("IPProtoIn",        m_IPProtoIn = IP_PROTO_HLT);
  declareProperty("sockBuf",          m_sockBuf = 0x100000);
  declareProperty("partitionID",      m_partitionID);
  declareProperty("refCount",         m_refCount = 2);
  declareProperty("MEPBufSize",       m_MEPBufSize = -1);
  declareProperty("bufName",          m_bufName = "MEPRX");
  declareProperty("ownAddress",       m_ownAddress = 0xFFFFFFFF);
  declareProperty("RTTCCompat",       m_RTTCCompat = false);
  declareProperty("RxIPAddr",         m_rxIPAddr = "127.0.0.1");
  m_trashCan  = new u_int8_t[MAX_R_PACKET];
}

// Standard Destructor
MEPRxSvc::~MEPRxSvc(){
  delete [] (u_int8_t*)m_trashCan;
}

bool MEPRxSvc::cmpL0ID(MEPRx *r, u_int32_t id) {
  return r->m_l0ID < id;
}

void MEPRxSvc::removePkt()   {
  int len = MEPRxSys::recv_msg(m_trashCan,MAX_R_PACKET,0);
  if (len < 0) {
    if (!MEPRxSys::rx_would_block())   {
      MsgStream log(msgSvc(),"MEPRx");
      ERRMSG(log,"recvmsg");
    }
  }
}

// age all workDsc and return the oldest
MEPRxSvc::RXIT MEPRxSvc::ageRx() {
  if ( !m_workDsc.empty() )  {
    RXIT k = m_workDsc.begin(), j = m_workDsc.begin();
    for (; j != m_workDsc.end(); ++j)  {
      if (--((*j)->m_age) <= 0) {
        k = j;
        (*j)->m_age = 0;
      } 
      else if ((*k)->m_age > (*j)->m_age)  {
        k = j;
      }
    }
    return k;
  }
  return m_workDsc.end();
}

StatusCode MEPRxSvc::setupMEPReq(std::string odinName) {
  MsgStream log(msgSvc(), "MEPRx");
  std::string msg;
  u_int32_t addr;

  if (odinName.empty()) {
    log << MSG::INFO << "No address for ODIN. Dynamic MEP requests disabled!";
    m_dynamicMEPRequest = false;
    return StatusCode::SUCCESS;
  }
  if (MEPRxSys::parse_addr(odinName, addr) && MEPRxSys::addr_from_name(odinName, addr, msg)) {
    log << MSG::ERROR << "Invalid address for ODIN: " <<  odinName << " " << msg << endmsg;
    return StatusCode::FAILURE; 
  } 
  m_odinIPAddr = addr;
  log << MSG::INFO << "Dynamic MEP requests will be sent to "
      << MEPRxSys::dotted_addr(m_odinIPAddr) << endmsg;
  m_dynamicMEPRequest = true;
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::sendMEPReq(int m) {
  if ( m_dynamicMEPRequest )   {
    mepreq.nmep = m;
    int n = MEPRxSys::send_msg(m_odinIPAddr, MEP_REQ_TOS, &mepreq, MEP_REQ_LEN, 0);
    if (n == MEP_REQ_LEN)   {
      return StatusCode::SUCCESS;
    }
    if (n == -1)  {
      MsgStream log(msgSvc(),"MEPRx");
      ERRMSG(log,"send MEP request");
      return StatusCode::FAILURE;
    }
    error("MEPRequest corrupted on send!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void MEPRxSvc::freeRx() {
  lib_rtl_lock(m_usedDscLock);
  if (m_usedDsc.empty()) {
    lib_rtl_unlock(m_usedDscLock);
    return;
  }
  MEPRx *rx = *(--m_usedDsc.end());
  m_usedDsc.pop_back();
  lib_rtl_unlock(m_usedDscLock);
  if (rx->spaceRearm(0) == MBM_NORMAL) {
    lib_rtl_lock(m_freeDscLock);
    m_freeDsc.push_back(rx);
    lib_rtl_unlock(m_freeDscLock);
    sendMEPReq(1);
    return; 
  }
  error("timeout on getting space.");
}

void MEPRxSvc::forceEvents() {
  for (RXIT i = m_workDsc.begin(); i != m_workDsc.end(); ++i) {
    forceEvent(i);
  }
}

void MEPRxSvc::forceEvent(RXIT &dsc) {
  (*dsc)->spaceAction();
  m_workDsc.erase(dsc);
  RTL::Lock lock(m_usedDscLock);
  m_usedDsc.push_back(*dsc);
}

// IRunable implementation: Run the object
StatusCode MEPRxSvc::run() {
  MsgStream log(msgSvc(), "MEPRx"); // message stream is NOT thread-safe
  RXIT rx; 
  u_int8_t hdr[HDR_LEN];
  const RTL::IPHeader *iphdr  = (RTL::IPHeader*)hdr;
  const MEPHdr        *mephdr = (MEPHdr*) &hdr[IP_HEADER_LEN];;
  int srcid;

  m_forceStop = false;
  while (!m_receiveEvents) {
    MEPRxSys::usleep(100000); // 100 ms
  }

  for (;;) {
    int n = MEPRxSys::rx_select(2);  
    if (n ==  -1) {
      ERRMSG(log,"select");
      continue;
    }
    if (n == MEPRX_WRONG_FD) {
      error("wrong filedes on select");
      continue;
    }
    if (n == 0) {
      static int ncrh = 1;
      if (!m_receiveEvents) {
        forceEvents();
        return 0;
      } 
      if (--ncrh == 0) {
        log << MSG::DEBUG << "crhhh..." <<  m_workDsc.size() << endmsg;
        ncrh = 10;
      }
      continue;
    }
    int len = MEPRxSys::recv_msg(hdr, HDR_LEN, MEPRX_PEEK);
    if (len < 0) {
      if (!MEPRxSys::rx_would_block()) 
        ERRMSG(log,"recvmsg");
      continue;
    }
    m_totRxPkt++;	
    if ((srcid = getSrcID(iphdr->saddr)) == - 1) {
      /* we do not expect nor want this */
      removePkt();
      continue;
    }
    if (!m_workDsc.empty() && mephdr->m_l0ID == m_workDsc.back()->m_l0ID) {
      rx = --m_workDsc.end();
    } 
    else {
      rx = lower_bound(m_workDsc.begin(), m_workDsc.end(), mephdr->m_l0ID, cmpL0ID);
      if (rx == m_workDsc.end() || (*rx)->m_l0ID != mephdr->m_l0ID) {
        // not found - get a new descriptor
        RXIT oldest = ageRx();
        try {
          if (m_freeDsc.empty()) {
            forceEvent(oldest);
            freeRx(); // only if not in separate thread
          }
          while (m_freeDsc.empty()) MEPRxSys::usleep(100) ; /* only necessary on 
                                                            multithreading */
          lib_rtl_lock(m_freeDscLock);
          rx = (--m_freeDsc.end());
          m_freeDsc.pop_back();
          lib_rtl_unlock(m_freeDscLock);
          RXIT j = lower_bound(m_workDsc.begin(),m_workDsc.end(),mephdr->m_l0ID,cmpL0ID);
          m_workDsc.insert(j, *rx);
          rx = --m_workDsc.end();
          (*rx)->m_age = m_MEPBuffers;
          (*rx)->m_l0ID = mephdr->m_l0ID;
        }
        catch(std::exception& e) {
          error(std::string("Exception ")+e.what());
        }
      } 
    }
    if ((*rx)->addMEP(mephdr, srcid) == MEP_SENT) {
      m_workDsc.erase(rx);
      lib_rtl_lock(m_usedDscLock);
      m_usedDsc.push_back(*rx);     
      lib_rtl_unlock(m_usedDscLock);
      freeRx();
    }		   		   
  }
  return 1; 
}   

// IInterface implementation: Query interface
StatusCode 
MEPRxSvc::queryInterface(const InterfaceID& riid,void** ppvInterface) {
  if ( riid == IID_IRunable )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS; 
  }
  // Interface is not directly availible: try a base class
  return Service::queryInterface(riid, ppvInterface);
}

StatusCode MEPRxSvc::error(const std::string& msg) {
  MsgStream log(msgSvc(),"MEPRx");
  log << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

StatusCode MEPRxSvc::checkProperties() {
  std::ostringstream log;
  if (m_ethInterface < 0) {
    return error("ethDevIn must be >= 0");
  }
  if (m_maxMsForGetSpace < 1) {
    return error("maxMsForGetSpace must be > 0");
  }
  if (m_MEPBuffers < 1 || m_MEPBuffers > 256) {
    return error("MEPBuffers must be > 0 and < 256");
  }
  if (m_pktSamplingCount < 0) {
    return error("pktSamplingCount must be >= 0");
  }
  if (m_maxBadPktRatio <= 0) {
    return error("maxBadPktRatio must be > 0");
  }
  if (m_sockBuf < 0x10000 || m_sockBuf > 0x4000000) {
    return error("sockBuf must be >= 64 kB and less than 64 MB");
  }
  if (m_IPProtoIn < 0 || m_IPProtoIn > 255) {
    return error("IPProtoIn is an unsigned 8 bit quantity");
  }
  if (m_MEPBufSize < 0) {
    log << "Illegal value for MEPBufSize " << m_MEPBufSize;
    return error(log.str());
  }		  
  if ( !setupMEPReq(m_IPNameOdin).isSuccess() ) {
    log << "Bad address IPNameOdin " << m_IPNameOdin;
    return error(log.str());
  }
  if ((m_IPSrc.size() % 2) != 0) {
    return error("Malformed source list (length is odd)");
  }
  for (unsigned int i = 0; i < m_IPSrc.size(); i += 2) {
    std::string nam, msg;
    u_int32_t addr; 
    if (MEPRxSys::parse_addr(m_IPSrc[i], addr)) {
      if (MEPRxSys::addr_from_name(m_IPSrc[i + 1], addr, msg)) {
        log << "No correct address for source " << i << " " << msg;
        return error(log.str());
      }
      nam = m_IPSrc[i + 1];
    }
    else {
      char tmp[16];
      if (m_IPSrc[i + 1].size() == 0) {
        if (MEPRxSys::name_from_addr(addr, nam, msg)) 
          nam = snprintf(tmp, 15, "src-%d", i/2);
      }
      else {
        nam = m_IPSrc[i + 1];
      }
    }
    m_srcAddr[addr] = i / 2; 
    m_srcName.push_back(nam);
  }
  m_nSrc = m_IPSrc.size() / 2; 
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::allocRx() {
  std::string nam = "MEPRx";
  MsgStream log(msgSvc(),nam);
  for(int i = 0; i < m_MEPBuffers; ++i) {
    MEPRx *rx = new MEPRx(nam + char(i+'0'), this);
    if (rx->spaceRearm(0) != MBM_NORMAL)  {
      return StatusCode::FAILURE;
    }
    m_freeDsc.push_back(rx);
  }
  m_workDsc.clear();
  log << MSG::DEBUG << std::dec 
      << printnum(m_MEPBuffers, " receive-buffer") << " created." << endmsg;  
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::releaseRx() {
  RTL::Lock l1(m_usedDscLock), l2(m_freeDscLock);
  for(Workers::iterator i=m_freeDsc.begin(); i != m_freeDsc.end(); ++i)
    delete (*i);
  m_freeDsc.clear();
  for(Workers::iterator j=m_usedDsc.begin(); j != m_usedDsc.end(); ++j)
    delete (*j);
  m_usedDsc.clear();
  for(Workers::iterator k=m_workDsc.begin(); k != m_workDsc.end(); ++k)
    delete (*k);
  m_workDsc.clear();
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::openSocket() {
  std::string msg;
  if (MEPRxSys::open_sock(m_IPProtoIn, 
                          m_sockBuf,
                          m_ethInterface,
                          m_rxIPAddr, 
                          m_dynamicMEPRequest, 
                          msg)) 
  {
    MsgStream log(msgSvc(),"MEPRx");
    ERRMSG(log,msg);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

int MEPRxSvc::getSrcID(u_int32_t addr)  {
  std::map<u_int32_t, int>::iterator i = m_srcAddr.find(addr);
  if(i == m_srcAddr.end()) {
    MsgStream log(msgSvc(),"MEPRx");
    log << MSG::ERROR << "received unexpected packet from "
        << MEPRxSys::dotted_addr(addr) << endmsg;
    m_notReqPkt++;
    return -1;
  }
  return i->second;
}

void MEPRxSvc::publishCounters()
{
}

void MEPRxSvc::clearCounters() {
  memset(m_rxOct,  0, m_nCnt * sizeof(u_int64_t));
  memset(m_rxPkt,  0, m_nCnt * sizeof(u_int64_t));
  memset(m_badPkt, 0, m_nCnt * sizeof(u_int32_t));
  memset(m_misPkt, 0, m_nCnt * sizeof(u_int32_t));
  m_notReqPkt = m_swappedMEP = 0;
} 

int MEPRxSvc::setupCounters(int n) {
  m_rxOct  = new u_int64_t[n];
  m_rxPkt  = new u_int64_t[n];
  m_badPkt = new u_int32_t[n];
  m_misPkt = new u_int32_t[n];
  PUBCNT(totRxOct, "Total received bytes");
  PUBCNT(totRxPkt, "Total received packets");
  PUBCNT(incEvt,   "Incomplete events");
  m_notReqPkt = m_swappedMEP = 0;
  m_nCnt = n;
  return 0;
}

void  MEPRxSvc::handle(const Incident& inc)    {
  MsgStream log(msgSvc(), "MEPRx");
  log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  {
    m_receiveEvents = false;
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_receiveEvents = true;
  }
}

StatusCode MEPRxSvc::initialize()  {
  StatusCode sc = Service::initialize();
  MsgStream log(msgSvc(),"MEPRx");
  if (sc != StatusCode::SUCCESS)  {
    log << MSG::DEBUG << "Failed to initialize service base class." << endmsg;
    return sc;
  }
  log << MSG::DEBUG << "Entering initialize....." << endmsg;
  if ( !checkProperties().isSuccess() )  return StatusCode::FAILURE;
  if ( !openSocket().isSuccess()      )  return StatusCode::FAILURE;
  if ( !allocRx().isSuccess()         )  return StatusCode::FAILURE;

  if (lib_rtl_create_lock(0, &m_usedDscLock) != 1 || 
    lib_rtl_create_lock(0, &m_freeDscLock) != 1) {
      return error("Failed to create locks.");
    }
    if (service("IncidentSvc", m_incidentSvc).isSuccess()) {
      m_incidentSvc->addListener(this, "DAQ_CANCEL");
      m_incidentSvc->addListener(this, "DAQ_ENABLE");      
    } 
    else { 
      return error("Failed to access incident service.");
    }
    if (service("MonitorSvc", m_monSvc).isSuccess()) {
      setupCounters(m_MEPBuffers);
    } 
    else {
      return error("Failed to access monitor service.");
    }
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::finalize()  {
  MsgStream log(msgSvc(),"MEPRx");
  log << MSG::DEBUG << "Entering finalize....." << endmsg;
  m_receiveEvents = false;
  releaseRx();
  if (m_incidentSvc) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = NULL;
  }
  if (m_monSvc) {
    m_monSvc->undeclareAll(this);
    m_monSvc->release();
    m_monSvc = NULL;
  }
  if ( m_swappedMEP ) {
    log << MSG::WARNING << m_swappedMEP << " MEPs with" <<
           " swapped header structure detected" << endmsg;
  }
  return Service::finalize();
}
