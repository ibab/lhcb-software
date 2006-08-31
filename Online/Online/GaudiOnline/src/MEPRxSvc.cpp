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
//      Version   : $Id: MEPRxSvc.cpp,v 1.25 2006-08-31 09:55:24 niko Exp $
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
#include "GaudiOnline/MEPRxSvc.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "RTL/rtl.h"
#include "NET/IPHeader.h"
#include "WT/wt_facilities.h"
#include "MBM/MepProducer.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiOnline/MEPRxSys.h"

typedef RTL::IPHeader ip_hdr_t;
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

#define RAWBANKSIZE (sizeof(LHCb::RawBank) - sizeof(int)) /* f*** C99 */ 
#ifdef _WIN32
#define ERRMSG(x) do { \
  *m_log <<	MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(x) do { \
  *m_log << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __PRETTY_FUNCTION__ << ":" << __LINE__ << endmsg;} while(0);
#endif

#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define printnum(n, s) n << s << (n == 1 ? "" : "s")

typedef struct LHCb::MEPHdr mep_hdr_t;
typedef std::vector<LHCb::MEPRx *>::iterator RXIT;
#define HDR_LEN (IP_HEADER_LEN + sizeof(LHCb::MEPHdr)) 

static u_int8_t __hdr[HDR_LEN];
static MEPReq mepreq;

struct LHCb::MEPRx: public MEP::Producer {
  // parameters
  int m_refCount, m_spaceSize, m_age;
  LHCb::MEPRxSvc *m_parent;
  int m_nSrc, m_nrx;
  MsgStream *m_log;
  int m_r;
  // run-time
  u_int32_t m_l0ID, m_brx; 
  u_int16_t m_pf;
  int m_spaceRC;
  int m_eventType;
  MEPEVENT *m_e; 
  struct DAQErrorBankEntry m_eEntry[MAX_SRC];
  class  LHCb::RawBank *m_rawBufHdr, *m_MDFBankHdr;
  int m_seen[MAX_SRC]; 
  int m_maxMsForGetSpace;
  bool m_dry;

public:
  MEPRx(const std::string &nam, int partID, int refcnt, 
    size_t siz, LHCb::MEPRxSvc *parent, int nsrc, MsgStream *log, int r) 
    : MEP::Producer(nam, partID),  m_refCount(refcnt), m_spaceSize(siz),
    m_parent(parent), m_nSrc(nsrc), m_log(log), m_r(r)
  {

    m_flags = USE_MEP_BUFFER;
    include();
    m_bmid = m_mepID->mepBuffer;
    *m_log << MSG::DEBUG << std::hex << std::showbase << 
      "MEP buffer start: " 
      << m_mepID->mepStart << "\nEVENT  buffer start: " 
      << m_mepID->evtStart << "\nRESULT buffer start: " 
      << m_mepID->resStart << endmsg;     
    *m_log << MSG::DEBUG << "Buffer space: " << m_spaceSize << " bytes" << 
      endmsg;
    m_eventType = EVENT_TYPE_MEP;
    m_rawBufHdr = (class LHCb::RawBank *) 
      new u_int8_t[sizeof(LHCb::RawBank)]; /* don't ask me, ask Markus! */
    size_t len = sizeof(LHCb::RawBank) + MDFHeader::sizeOf(1);
    m_MDFBankHdr = (class LHCb::RawBank *) new u_int8_t[len];
    m_MDFBankHdr->setType(RawBank::DAQ);
    m_MDFBankHdr->setSize(MDFHeader::sizeOf(1));
    m_MDFBankHdr->setVersion(DAQ_STATUS_BANK);
    m_MDFBankHdr->setSourceID(1024);
    m_MDFBankHdr->setMagic();
  }
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
    m_brx = m_nrx = 0; memset(m_seen, 0, m_nSrc * sizeof(int));
    m_spaceRC = MEP::Producer::spaceRearm(m_spaceSize);
    return m_spaceRC;
  }
#define RAWBHDRSIZ (sizeof(LHCb::RawBank) - 4)
#define MEPHDRSIZ   sizeof(MEPHdr)
#define MEPFHDRSIZ  sizeof(MEPFrgHdr)  
#define DAQEERSIZ   sizeof(DAQErrorBankEntry)

  inline int createDAQErrorBankEntries() {
    int j = 0;
    /* we create a MEP fragment with the required error bank */
    for (int i = 0; i < m_nSrc; ++i) {
      if (m_seen[i] == 1) continue;
      m_eEntry[j].m_srcID = i;
      m_eEntry[j].m_srcIPAddr = m_parent->m_srcAddr[i];
      m_eEntry[j].m_errorType = MEPRxSvc::MissingSrc;
      m_eEntry[j].m_pktData = NULL;
      j++;
    }
    return j;
  }
  inline int setupDAQErrorBankHdr() {
    int n = createDAQErrorBankEntries();
    int size = n * DAQEERSIZ + RAWBHDRSIZ;
    int rem;

    m_rawBufHdr->setMagic();
    m_rawBufHdr->setSourceID(m_parent->m_sourceID);
    m_rawBufHdr->setVersion(DAQ_ERR_BANK_VERSION);
    m_rawBufHdr->setType(LHCb::RawBank::DAQ);
    m_rawBufHdr->setSize(size);
    if ((rem = (size % 4))) return (size + 4 - rem); else return size;
  } 
  inline int createDAQErrorMEP(u_int8_t *buf, int nEvt) {
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
  } 
  inline int setupMDFBank() {
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

  inline int createMDFMEP(u_int8_t *buf, int nEvt) {
    struct MEPHdr *meph = (struct MEPHdr *) buf;
    size_t banksize = setupMDFBank();
    meph->m_l0ID = m_l0ID;
    meph->m_totLen = MEPHDRSIZ +  nEvt * (MEPFHDRSIZ + banksize);
    meph->m_nEvt = nEvt;
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
  inline void addMDFMEP(void) {
    u_int8_t *buf = (u_int8_t *) m_e->data + m_brx + 4 + IP_HEADER_LEN; 
    m_brx += createMDFMEP(buf, m_pf) + IP_HEADER_LEN;
    return;
  } 
  inline void incompleteEvent() {

    if ((m_parent->m_incEvt)++ < 10) {
      *m_log << MSG::ERROR << "Incomplete Event" << endmsg;
    }
    u_int8_t *buf = (u_int8_t *) m_e->data + m_brx + 4 + IP_HEADER_LEN; 
    m_brx += createDAQErrorMEP(buf, m_pf) + IP_HEADER_LEN;
    return;
  }
  int spaceAction() { 
    static  int id = -1; 
    if (m_nrx != m_nSrc) incompleteEvent();
    addMDFMEP();
    m_e->evID        = ++id;
    m_event.len = m_brx + sizeof(MEPEVENT);
    m_event.mask[0] = 0xffffffff;
    m_event.mask[1] = 0xffffffff;
    m_event.mask[2] = 0xffffffff;
    m_event.mask[3] = 0xffffffff;
    m_event.type    = m_eventType;    

    *((int *) m_e->data) = m_brx;
    if (m_parent->m_RTTCCompat) LHCb::RTTC2Current((MEPEvent*)m_e->data);
    declareEvent();
    //std::cout << "bang " << id << " " << m_brx << " bytes " << m_eventType 
    //          << std::endl;
    return sendSpace();
  }
  // Run the application in synchonous mode
  int runSynchronous() {
    int status = spaceRearm(0);
    if ( status == MBM_NORMAL )  {
      status = spaceAction();
    }
    return status;
  }
  void multipleSrc() {
    m_eventType = EVENT_TYPE_ERROR;
    *m_log << MSG::ERROR << "Multiple event from source" << endmsg; 
  } 
  void badPkt(MEPRxSvc::DAQError /* type */ ) {
    m_eventType = EVENT_TYPE_ERROR;
    *m_log << MSG::ERROR << "Bad event from source" << endmsg; 
  }
  void warnSwap(void) {
    if ((m_parent->m_swappedMEP)++ < 10) {
      *m_log << MSG::WARNING << "MEP with swapped packing_factor / len"\
        " detected. Fixed!" << endmsg;
    }
  }
  int addMEP(const MEPHdr *hdr, int srcid) {
    int len = 0;

    if (m_seen[srcid]) multipleSrc();
    m_seen[srcid] = 1;		   
    /* here a lot of tests should be done */
    if (m_nrx == 0) {
      MBM::EventDesc& dsc = event();
      m_e = (MEPEVENT *)dsc.data;
      m_e->refCount    = m_refCount;
      m_e->evID        = hdr->m_l0ID;
      m_e->begin       = int(int(m_e)-m_mepID->mepStart);
      m_e->packing     = -1;
      m_e->valid       = 1;
      m_e->magic       = mep_magic_pattern();
      m_brx = 0;
      m_pf = hdr->m_nEvt;
    }
    len = MEPRxSys::recv_msg((u_int8_t *) m_e->data + m_brx + 4, MAX_R_PACKET, 0);
    if (len < 0) {
      ERRMSG("failed to receive message");
      return MEP_ADD_ERROR;
    }    
    MEPHdr *newhdr = (mep_hdr_t *) ((u_int8_t *) m_e->data + m_brx + 4 + IP_HEADER_LEN);
    m_parent->m_totRxOct += len;
    m_brx += len;

    /* check for swapped m_nEvt field */
    if (len != (hdr->m_totLen + IP_HEADER_LEN)) {
      if (len != (hdr->m_nEvt + IP_HEADER_LEN)) {
        badPkt(MEPRxSvc::ShortPkt);
      } else {
        u_int16_t tmp;
        tmp = newhdr->m_totLen; newhdr->m_totLen = newhdr->m_nEvt; 
        newhdr->m_nEvt = tmp;
        warnSwap();
        if (m_nrx == 0) m_pf = newhdr->m_nEvt;
      }
    }
    m_nrx++; 
    if (m_pf != newhdr->m_nEvt) badPkt(MEPRxSvc::WrongPackingFactor);    
    MEPEvent* e = (MEPEvent*)m_e->data;
    e->setSize(m_brx);
    //      info("\t\t\t nrx: %d", m_nrx);
    //      if (m_nrx == m_nSrc) info("complete\n");
    return (m_nrx == m_nSrc) ? spaceAction() : MEP_ADDED;
  }  

};

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPRxSvc)

// Standard Constructor
LHCb::MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc), m_handle(0), m_receiveEvents(false), m_incidentSvc(0)
{
  declareProperty("MEPBuffers", m_MEPBuffers = 4);
  declareProperty("ethInterface", m_ethInterface);
  declareProperty("IPNameOdin", m_IPNameOdin = "");
  declareProperty("maxBadPktRatio", m_maxBadPktRatio = 0.);
  declareProperty("pktSamplingCount", m_pktSamplingCount = 0);
  declareProperty("maxMsForGetSpace", m_maxMsForGetSpace = 10);
  declareProperty("IPSrc", m_IPSrc);
  declareProperty("IPProtoIn", m_IPProtoIn = IP_PROTO_HLT);
  declareProperty("sockBuf", m_sockBuf = 0x100000);
  declareProperty("partitionID", m_partitionID);
  declareProperty("refCount",m_refCount = 2);
  declareProperty("MEPBufSize", m_MEPBufSize = -1);
  declareProperty("bufName", m_bufName = "MEPRX");
  declareProperty("ownAddress", m_ownAddress = 0xFFFFFFFF);
  declareProperty("MEPManager",  m_mepMgrName="LHCb::MEPManager/MEPManager");
  declareProperty("RTTCCompat",  m_RTTCCompat = false);
  declareProperty("RxIPAddr", m_rxIPAddr = "127.0.0.1");
  m_trashCan  = new u_int8_t[MAX_R_PACKET];
}

// Standard Destructor
LHCb::MEPRxSvc::~MEPRxSvc(){
  delete((u_int8_t *) m_trashCan);
}

bool 
LHCb::MEPRxSvc::cmpL0ID(MEPRx *r, u_int32_t id) {
  //info("cmpL0ID: m_l0ID %d id %d\n", r->m_l0id, id);
  return r->m_l0ID < id;
}

void
LHCb::MEPRxSvc::removePkt()   {
  int len = MEPRxSys::recv_msg(m_trashCan, MAX_R_PACKET , 0);
  if (len < 0) {
    if (!MEPRxSys::rx_would_block()) 
      ERRMSG("recvmsg");
  }
}
/* age all workDsc and return the oldest */
RXIT
LHCb::MEPRxSvc::ageRx() {
  RXIT k, j;
  if (m_workDsc.empty()) return m_workDsc.end();
  for (k = j = m_workDsc.begin(); j != m_workDsc.end(); ++j) 
    if (--((*j)->m_age) <= 0) {
      k = j;
      (*j)->m_age = 0;
    } else if ((*k)->m_age > (*j)->m_age) k = j;
  return k;
}

int
LHCb::MEPRxSvc::setupMEPReq(std::string odinName) {
  u_int32_t addr;
  MsgStream log(msgSvc(), name());
  std::string msg;

  if (odinName.empty()) {
    log << MSG::INFO << "No address for ODIN. Dynamic MEP requests disabled!";
    m_dynamicMEPRequest = false;
    return 0;
  }
  if (MEPRxSys::parse_addr(odinName, addr) && MEPRxSys::addr_from_name(odinName, addr, msg)) {
    log << MSG::ERROR << "invalid address for ODIN: " <<  odinName << " " << msg << endmsg;
    return 1; 
  } 
  m_odinIPAddr = addr;
  log << MSG::INFO << "Dynamic MEP requests will be sent to " << 
    MEPRxSys::dotted_addr(m_odinIPAddr) << endmsg;
  m_dynamicMEPRequest = true;
  return 0;
}

int
LHCb::MEPRxSvc::sendMEPReq(int m) {
  int n;

  if (!m_dynamicMEPRequest) return 0;
  mepreq.nmep = m;
  if ((n = MEPRxSys::send_msg(m_odinIPAddr, &mepreq, MEP_REQ_LEN, 0)) == 
    MEP_REQ_LEN) return 0;
  if (n == -1) {
    ERRMSG("send MEP request");
    return 1;
  } else {
    *m_log << MSG::ERROR << "MEPRequest corrupted on send!" << endmsg;
    return 1;
  }
}

void 
LHCb::MEPRxSvc::freeRx() {
  MEPRx *rx;

  lib_rtl_lock(m_usedDscLock);
  if (m_usedDsc.empty()) {
    lib_rtl_unlock(m_usedDscLock);
    return;
  }
  rx = *(--m_usedDsc.end());
  m_usedDsc.pop_back();
  lib_rtl_unlock(m_usedDscLock);
  if (rx->spaceRearm(0) == MBM_NORMAL) {
    lib_rtl_lock(m_freeDscLock);
    m_freeDsc.push_back(rx);
    lib_rtl_unlock(m_freeDscLock);
    sendMEPReq(1);
    return; 
  }
  *m_log << MSG::ERROR << "timeout on getting space" << endmsg;
  return;
}

void
LHCb::MEPRxSvc::forceEvents() {
  for (RXIT i = m_workDsc.begin(); i != m_workDsc.end(); ++i) {
    forceEvent(i);
  }
}

void
LHCb::MEPRxSvc::forceEvent(RXIT &dsc) {
  (*dsc)->spaceAction();
  m_workDsc.erase(dsc);
  lib_rtl_lock(m_usedDscLock);
  m_usedDsc.push_back(*dsc);
  lib_rtl_unlock(m_usedDscLock);    
}

// IRunable implementation: Run the object
StatusCode 
LHCb::MEPRxSvc::run() {
  m_forceStop = false;
  RXIT rx; 
  const ip_hdr_t *iphdr = (ip_hdr_t *) __hdr;
  const struct MEPHdr *mephdr = (mep_hdr_t *) &__hdr[IP_HEADER_LEN];;
  int srcid;
  MsgStream log(msgSvc(), name());
  m_log = &log; // message stream is NOT thread-safe

  for (RXIT i = m_workDsc.begin(); i != m_workDsc.end(); ++i) {
    (*i)->m_log = &log;
  }
  while (!m_receiveEvents) {
    MEPRxSys::usleep(100000); // 100 ms
  }

  for (;;) {
    int n;  
    if ((n = MEPRxSys::rx_select(2)) ==  -1) {
      ERRMSG("select");
      continue;
    }
    if (n == MEPRX_WRONG_FD) {
      *m_log << MSG::ERROR << "wrong filedes on select" << endmsg;
      continue;
    }
    if (n == 0) {
      static int ncrh = 1;
      if (!m_receiveEvents) {
        forceEvents();
        return 0;
      } 
      if (--ncrh == 0) {
        *m_log << MSG::DEBUG << "crhhh..." <<  m_workDsc.size() << endmsg;
        ncrh = 10;
      }
      continue;
    }
    int len = MEPRxSys::recv_msg(__hdr, HDR_LEN, MEPRX_PEEK);
    if (len < 0) {
      if (!MEPRxSys::rx_would_block()) 
        ERRMSG("recvmsg");
      continue;
    }
    m_totRxPkt++;	
    if ((srcid = getSrcID(iphdr->saddr)) == - 1) {
      /* we do not expect nor want this */
      removePkt();
      continue;
    }
    //    ::fprintf(stdout, "\rpktin %d ", ++pktin);
    //        ::fflush(stdout);
    if (!m_workDsc.empty() && mephdr->m_l0ID == m_workDsc.back()->m_l0ID) {
      rx = --m_workDsc.end();
    } else {
      rx = lower_bound(m_workDsc.begin(), m_workDsc.end(), mephdr->m_l0ID, 
        cmpL0ID);
      if (rx == m_workDsc.end() || (*rx)->m_l0ID != mephdr->m_l0ID) {
        /* not found - get a new descriptor*/
        RXIT oldest = ageRx();
        try {
          if (m_freeDsc.empty()) {
            forceEvent(oldest);
            freeRx(); /* only if not in separate thread */
          }
          while (m_freeDsc.empty()) MEPRxSys::usleep(100) ; /* only necessary on 
                                                            multithreading */
          lib_rtl_lock(m_freeDscLock);
          rx = (--m_freeDsc.end());
          m_freeDsc.pop_back();
          lib_rtl_unlock(m_freeDscLock);
          RXIT j = lower_bound(m_workDsc.begin(), m_workDsc.end(), 
            mephdr->m_l0ID, cmpL0ID);
          m_workDsc.insert(j, *rx);
          rx = --m_workDsc.end();
          (*rx)->m_age = m_MEPBuffers;
          (*rx)->m_l0ID = mephdr->m_l0ID;
        }
        catch(std::exception& e) {
          *m_log << MSG::ERROR << "Exception " << e.what() << endmsg;
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
  //Incident incident(name(),"DAQ_ERROR");
  //m_incidentSvc->fireIncident(incident);
  return 1; 
}   

// IInterface implementation: Query interface
StatusCode 
LHCb::MEPRxSvc::queryInterface(const InterfaceID& riid,void** ppvInterface) {
  if ( riid == IID_IRunable )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS; 
  }
  // Interface is not directly availible: try a base class
  return Service::queryInterface(riid, ppvInterface);
}

int 
LHCb::MEPRxSvc::checkProperties() {
  MsgStream log(msgSvc(),name());

  if (m_ethInterface < 0) { 
    log << MSG::ERROR << "ethDevIn must be >= 0" << endmsg;
    return 1;
  }
  if (m_maxMsForGetSpace < 1) {
    log << MSG::ERROR << "maxMsForGetSpace must be > 0" << endmsg;
    return 1;
  }
  if (m_MEPBuffers < 1 || m_MEPBuffers > 256) {
    log << MSG::ERROR << "MEPBuffers must be > 0 and < 256" << endmsg;
    return 1;
  }
  if (m_pktSamplingCount < 0) {
    log << MSG::ERROR << "pktSamplingCount must be >= 0" << endmsg;
    return 1;
  }
  if (m_maxBadPktRatio <= 0) {
    log << MSG::ERROR << "maxBadPktRatio must be > 0" << endmsg;
    return 1;
  }
  if (m_sockBuf < 0x10000 || m_sockBuf > 0x4000000) {
    log << MSG::ERROR << "sockBuf must be >= 64 kB and less than 64 MB" << endmsg;
    return 1;
  }
  if (m_IPProtoIn < 0 || m_IPProtoIn > 255) {
    log << MSG::ERROR << "IPProtoIn is an unsigned 8 bit quantity" << endmsg;
    return 1;
  }
  if (m_MEPBufSize < 0) {
    log << MSG::ERROR << "Illegal value for MEPBufSize " << m_MEPBufSize << endmsg;
    return 1;
  }		  
  if (setupMEPReq(m_IPNameOdin)) {
    log << MSG::ERROR << "Bad address IPNameOdin " << m_IPNameOdin << endmsg;
    return 1;
  }
  if ((m_IPSrc.size() % 2) != 0) {
    log << MSG::ERROR << "Malformed source list (length is odd)" << endmsg;
    return 1;
  }
  for (unsigned int i = 0; i < m_IPSrc.size(); i += 2) {
    u_int32_t addr; std::string name;

    if (MEPRxSys::parse_addr(m_IPSrc[i], addr)) {
      std::string msg;
      if (MEPRxSys::addr_from_name(m_IPSrc[i + 1], addr, msg)) {
        log << MSG::ERROR << "No correct address for source " << i << " " \
          << msg << endmsg;
        return 1;
      }
      name = m_IPSrc[i + 1];
    } else {
      char tmp[16];
      std::string msg;
      if (m_IPSrc[i + 1].size() == 0) {
        if (MEPRxSys::name_from_addr(addr, name, msg)) name = snprintf(tmp, 15, "src-%d", 
          i / 2);
      } else {
        name = m_IPSrc[i + 1];
      }
    }
    m_srcAddr[addr] = i / 2; m_srcName.push_back(name);
  }
  m_nSrc = m_IPSrc.size() / 2; 
  return 0;
}

int LHCb::MEPRxSvc::allocRx() {
  std::string name = "MEPRx";
  // int partID = m_mepMgr->partitionID();
  // const std::string& proc = m_mepMgr->processName();                             
  //MEPID id = m_mepMgr->mepID();
  for (int i = 0; i < m_MEPBuffers; ++i) {
    LHCb::MEPRx *rx = new LHCb::MEPRx(name + char(i+'0'), m_partitionID, m_refCount, m_MEPBufSize,
      this, m_nSrc, m_log, m_r);
    if (rx->spaceRearm(0) != MBM_NORMAL) return 1;
    m_freeDsc.push_back(rx);
  }
  m_workDsc.clear();
  *m_log   << MSG::DEBUG << std::dec << printnum(m_MEPBuffers, "receive-buffer") << 
    " created" << endmsg;  
  return 0;
}

int LHCb::MEPRxSvc::openSock() {
  std::string msg;

  if (MEPRxSys::open_sock(m_IPProtoIn, m_sockBuf, m_ethInterface, \
    m_rxIPAddr, m_dynamicMEPRequest, msg)) {
      ERRMSG(msg);
      return 1;
    }
    return 0;
}

inline int
LHCb::MEPRxSvc::getSrcID(u_int32_t addr)
{
  std::map<u_int32_t, int>::iterator i;

  if ((i = m_srcAddr.find(addr)) == m_srcAddr.end()) {
    *m_log << MSG::ERROR << "received unexpected packet from " << \
      MEPRxSys::dotted_addr(addr) << endmsg;
    m_notReqPkt++;
    return -1;
  }
  return i->second;
}

void
LHCb::MEPRxSvc::publishCounters()
{
}

void
LHCb::MEPRxSvc::clearCounters()
{
  memset(m_rxOct,  0, m_nCnt * sizeof(u_int64_t));
  memset(m_rxPkt,  0, m_nCnt * sizeof(u_int64_t));
  memset(m_badPkt, 0, m_nCnt * sizeof(u_int32_t));
  memset(m_misPkt, 0, m_nCnt * sizeof(u_int32_t));
  m_notReqPkt = m_swappedMEP = 0;
} 

int
LHCb::MEPRxSvc::setupCounters(int n) {
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

void 
LHCb::MEPRxSvc::handle(const Incident& inc)    {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Got incident:" << inc.source()
    << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  {
    m_receiveEvents = false;
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_receiveEvents = true;
  }
}


StatusCode 
LHCb::MEPRxSvc::initialize()  {
  int sc;

  if ((sc = Service::initialize()) != StatusCode::SUCCESS) return sc;
  m_log = new MsgStream(msgSvc(),name());
  *m_log << MSG::DEBUG << "Entering initialize....." << endmsg;
  //if (!(service(m_mepMgrName, m_mepMgr).isSuccess())) return StatusCode::FAILURE;
  if (checkProperties() || openSock() || allocRx())
    return StatusCode::FAILURE;
  if (lib_rtl_create_lock(0, &m_usedDscLock) != 1 || 
    lib_rtl_create_lock(0, &m_freeDscLock) != 1) {
      *m_log << MSG::ERROR << "Failed to create locks." << endmsg;
      return StatusCode::FAILURE;
    }
    if (service("IncidentSvc", m_incidentSvc).isSuccess()) {
      m_incidentSvc->addListener(this, "DAQ_CANCEL");
      m_incidentSvc->addListener(this, "DAQ_ENABLE");      
    } else { 
      *m_log << MSG::ERROR << "Failed to access incident service." << endmsg;
      return StatusCode::FAILURE;
    }
    if (service("MonitorSvc", m_monSvc).isSuccess()) {
      setupCounters(m_MEPBuffers);
    } else {
      *m_log << MSG::ERROR << "Failed to access monitor service." << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}

StatusCode 
LHCb::MEPRxSvc::finalize()  {
  *m_log << MSG::DEBUG << "Entering finalize....." << endmsg;
  m_receiveEvents = false;
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
  if (m_swappedMEP) {
    *m_log << MSG::WARNING << m_swappedMEP << " MEPs with" << \
      " swapped header structure detected" << endmsg;
  }
  delete m_log;
  m_log = 0;
  return StatusCode::SUCCESS;
}
