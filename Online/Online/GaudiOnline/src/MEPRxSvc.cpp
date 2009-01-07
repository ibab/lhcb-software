//  ============================================================
//
//  MEPRxSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Niko Neufeld
//                  using code by B. Gaidioz and M. Frank
//
//      Version   : $Id: MEPRxSvc.cpp,v 1.71 2009-01-07 10:31:52 garnierj Exp $
//
//  ===========================================================
#ifdef _WIN32
#define snprintf _snprintf
#endif
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPRxSvc.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiOnline/MEPRxSys.h"
#include "RTL/Lock.h"   // RTL funny VMS-like wrappers for Unix sys-calls :-)
#include "NET/IPHeader.h"
#include "MBM/MepProducer.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/OnlineRunInfo.h"

#include "MDF/MEPEvent.h" 
#include "Event/RawBank.h"


typedef unsigned int uint;

#define MAX_R_PACKET (0x10000 + 20)
#define MEP_SENT  MBM_NORMAL
#define MEP_ADDED (MEP_SENT + 1)
#define IP_HEADER_LEN 20
#define IP_PROTO_HLT  0xF2

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
  a <<  MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<<  ":(" << __LINE__ << ")" << endmsg;} while(0);
#endif

#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size() * sizeof(int), desc, this);} while(0);

#define printnum(n,s)     n << s << ((char*)(n == 1 ? "" : "s"))

#define HDR_LEN (IP_HEADER_LEN + sizeof(LHCb::MEPHdr)) 

static MEPReq mepreq;


// Event Source Flags 
#define DOUBLE_ZERO_BUG    1   // source will send two events with L0ID == 0 
#define ODIN               2   // this is the primary ODIN for MDF function 

template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
}
using std::string;

namespace LHCb  {
  struct MEPRx  : public MEP::Producer {
    MEPRxSvc*       m_parent;
    // parameters
    int             m_refCount;
    int             m_spaceSize;
    unsigned long   m_age;
    int             m_tick;
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
    bool            m_wasIncomplete;
    MsgStream       m_log;
    DAQErrorEntry   m_eEntry[MAX_SRC];
    RawBank *m_rawBufHdr, *m_MDFBankHdr, *m_DAQBankHdr;  
    u_int8_t        *m_odinMEP;
  public:
    MEPRx(const std::string &nam, MEPRxSvc *parent);
    virtual ~MEPRx();
    static void release(MEPRx* p)               {      delete p;            }
    static bool cmpL0ID(MEPRx *r, u_int32_t id) {  return r->m_l0ID < id;   }
    static int spaceTimeOut(void *p) {
      MEPRx *self = (MEPRx *) p;
      if (self->m_spaceRC != -100) return 0;
      mbm_cancel_request(self->m_bmid);
      return 1;
    }
#define FULLNAME(id) m_parent->m_srcName[id] + " (" + m_parent->m_srcDottedAddr[id] + ")"
#define HOSTNAME(id) m_parent->m_srcName[id]
// If any source is missing, which has already been missing last time,  no MEP request is send
// If *all* the sources which have been missing on the last event are there now, then the MEP request for the previous event is re-armed
// If no source has been missing we do not re-send
    bool rearmMEPReq() {
      bool hasShownAgain = false;
      for (int i = 0; i < m_nSrc; ++i)
	if (m_parent->m_noShow[i]) 
	  if (!m_seen[i]) return false;
	  else hasShownAgain = true;
      return hasShownAgain;
    }

    void updateNoshow() {
      for (int i = 0; i < m_nSrc; ++i)
	m_parent->m_noShow[i] = m_seen[i];
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
    void multipleSrc(int srcid ) {
      m_eventType = EVENT_TYPE_ERROR;
      m_log << MSG::ERROR << "Multiple event from " << 
      m_parent->m_srcName[srcid] << " (" << m_parent->m_srcDottedAddr[srcid]
      << ")" << endmsg;
      m_parent->m_multipleEvt[srcid]++;
    } 
    void badPkt(DAQErrorEntry::DAQErrorType type , int srcid ) {
      m_eventType = EVENT_TYPE_ERROR;
      m_log << MSG::ERROR << "Bad packet from " <<
	HOSTNAME(srcid) << " [";
      switch (type) {
      case DAQErrorEntry::EmptyMEP: m_parent->m_truncPkt[srcid]++; 
	m_log << "empty MEP"; break;
      case DAQErrorEntry::WrongPackingFactor: m_parent->m_badPckFktPkt[srcid]++;
	m_log << "mismatch in packing factor"; break;
      case DAQErrorEntry::ShortPkt: m_parent->m_badLenPkt[srcid]++;
	m_log << "mismatch between length on wire and in MEP header"; break;
      default: m_log << "Mysterious error: " << type;
      }
      m_log << "]" << endmsg;
      (m_parent->m_totBadMEP)++;
    }

#define RAWBHDRSIZ (sizeof(RawBank) - 4)
#define MEPHDRSIZ   sizeof(MEPHdr)
#define MEPFHDRSIZ  sizeof(MEPFrgHdr)  

    /// We create a MEP fragment with the required error bank
    int createDAQErrorBankEntries();
    int setupDAQErrorBankHdr();
    int createDAQErrorMEP(u_int8_t *buf, uint nEvt);
    void addODINInfo(int);
    void setupMDFBank(u_int32_t, u_int32_t, u_int32_t);
    int createMDFMEP(u_int8_t *buf, int nEvt);
    void incompleteEvent();
    int spaceAction();
    int addMEP(int sockfd, const MEPHdr *hdr, int srcid);
    int analyzeMEP(MEPHdr *mep, uint &nfrag);  
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
    << "MEP    buffer start: " << m_mepID->mepStart << endmsg
    << "Buffer space:        " << m_spaceSize << " bytes" << endmsg;
  m_eventType  = EVENT_TYPE_MEP;
  m_rawBufHdr  = (class RawBank*)new u_int8_t[sizeof(RawBank)+16];
  size_t  len  =  sizeof(RawBank) + MDFHeader::sizeOf(1);
  m_MDFBankHdr = (class RawBank*) new u_int8_t[len];
  m_MDFBankHdr->setType(RawBank::DAQ);
  m_MDFBankHdr->setSize(MDFHeader::sizeOf(1));
  m_MDFBankHdr->setVersion(DAQ_STATUS_BANK);
  m_MDFBankHdr->setSourceID(1024);
  m_MDFBankHdr->setMagic();
  m_MDFBankHdr = (class RawBank*) new u_int8_t[len];
  m_MDFBankHdr->setType(RawBank::DAQ);
  m_MDFBankHdr->setSize(MDFHeader::sizeOf(1));
  m_MDFBankHdr->setVersion(DAQ_STATUS_BANK);
  m_MDFBankHdr->setSourceID(1025);
  m_MDFBankHdr->setMagic();
  
  m_wasIncomplete = false;
}

MEPRx::~MEPRx()  {
  delete[] (u_int8_t *) m_rawBufHdr;
  delete[] (u_int8_t *) m_MDFBankHdr;
  exclude();
}    

int MEPRx::createDAQErrorBankEntries() {
  int j = 0;
  for (int i = 0; i < m_nSrc; ++i) {
    if (m_seen[i] == 1) continue;
    m_eEntry[j].m_srcID     = i;
    m_eEntry[j].m_srcIPAddr = m_parent->sourceAddr(i);
    m_eEntry[j].m_errorType = DAQErrorEntry::MissingSrc;
    m_eEntry[j].m_pktData   = NULL;
    j++;
  }
  return j;
}

int MEPRx::setupDAQErrorBankHdr() {
  int n = createDAQErrorBankEntries();
  int size = n * sizeof(DAQErrorEntry) + RAWBHDRSIZ;
  m_rawBufHdr->setMagic();
  m_rawBufHdr->setSourceID(m_parent->sourceID());
  m_rawBufHdr->setVersion(DAQ_ERR_BANK_VERSION);
  m_rawBufHdr->setType(RawBank::DAQ);
  m_rawBufHdr->setSize(size);
  return (size%4) ? size+4 - (size%4) : size;
} 

// 0 MEP syntactically correct
// 1 syntactically corrupted (pointers don't work out)
// 2 low word of l0_id's wrong
// assumes that length in MEP header has been verified 
int MEPRx::analyzeMEP(MEPHdr *mep, uint &nfrag){
  uint len = mep->m_totLen;
  u_int32_t l0id = mep->m_l0ID;
  uint n = sizeof(MEPHdr);
  const u_int8_t *buf = (u_int8_t *) mep;
  int err = 0;
  
  nfrag = 0;
  while (n + MEPFHDRSIZ < len) {
    MEPFrgHdr *fh = (MEPFrgHdr *) (buf + n);
    if (fh->m_l0IDlow - nfrag != l0id) {
        err |= 1;
    }
    nfrag++; n += fh->m_len + MEPFHDRSIZ;
  }
  if (n + MEPFHDRSIZ > len) {
      // warn about bad MEP
      err |= 2;
  }
 return err;
}

void MEPRx::incompleteEvent() {
  int i, nmiss = 0;
  MEPEVENT* e = (MEPEVENT*)event().data;
  m_parent->addIncompleteEvent();
  for (i = 0; i < m_nSrc; ++i) nmiss += (m_seen[i] ? 0 : 1); 
  if (nmiss <= m_nSrc/2) {
    m_log << MSG::ERROR << "Incomplete Event #" << e->evID 
	  << "  No packet from: ";
    for (int i = 0; i < m_nSrc; ++i) 
      if (!m_seen[i]) { 
	m_log << HOSTNAME(i) << " "; 
	m_parent->m_misPkt[i]++;
      }
  } else  {
    m_log << MSG::ERROR << "Incomplete Event #" << e->evID 
	  << "  Only packets from: ";
    for (int i = 0; i < m_nSrc; ++i) 
      if (m_seen[i]) { 
	m_log << HOSTNAME(i) << " "; 
	m_parent->m_misPkt[i]++;
      }
  } 
  m_log << endmsg;
  if (m_parent->m_dropIncompleteEvents) { // added on Kazu's demand
    m_eventType = EVENT_TYPE_ERROR;
  }  
  m_wasIncomplete = true;
  if (m_parent->m_createDAQErrorMEP) {
    u_int8_t *buf = (u_int8_t *) e->data + m_brx + 4 + IP_HEADER_LEN; 
    m_brx += createDAQErrorMEP(buf, m_pf) + IP_HEADER_LEN;
  }
  return;
}

void MEPRx::setupMDFBank(u_int32_t run, u_int32_t orbit, u_int32_t bunchID) {
  uint mask[] = {~0,~0,~0,~0};
  MDFHeader* hdr = (MDFHeader *) m_MDFBankHdr->data();
  hdr->setHdr(0);
  hdr->setSize(0);
  hdr->setHeaderVersion(3);
  hdr->setChecksum(0);
  hdr->setCompression(0);
  hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
  hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
  MDFHeader::SubHeader h = hdr->subHeader();
  h.H1->setTriggerMask(mask);
  h.H1->setRunNumber(run);
  h.H1->setOrbitNumber(orbit);
  h.H1->setBunchID(bunchID);
  return;
}

void MEPRx::addODINInfo(int n) {
    if (m_odinMEP == NULL) 
	setupMDFBank(0, 0, 0);
    else {
	LHCb::RawBank *bank = (LHCb::RawBank *) (((u_int8_t *) m_odinMEP) + 
						MEPHDRSIZ + MEPFHDRSIZ + 
						n * (sizeof(LHCb::RawBank) + 
						     sizeof(LHCb::OnlineRunInfo) +
						     MEPFHDRSIZ));
	LHCb::OnlineRunInfo *odin = bank->begin<LHCb::OnlineRunInfo>(); // raetselhaft
	setupMDFBank(odin->Run, odin->Orbit, odin->bunchID);
    }
}

int MEPRx::createMDFMEP(u_int8_t *buf, int nEvt) {
  struct MEPHdr *meph = (struct MEPHdr *) buf;
  size_t banksize = m_MDFBankHdr->totalSize();
  meph->m_l0ID    = m_l0ID;
  meph->m_totLen  = MEPHDRSIZ +  nEvt * (MEPFHDRSIZ + banksize);
  meph->m_nEvt    = nEvt;
  buf += MEPHDRSIZ;
  for(int i = 0; i < nEvt; ++i) {
    struct MEPFrgHdr *frgh = (struct MEPFrgHdr *) buf;
    frgh->m_l0IDlow = 0xFFFF & (m_l0ID + i);
    frgh->m_len = banksize;
    buf += MEPFHDRSIZ;
    addODINInfo(i);
    memcpy(buf, m_MDFBankHdr, banksize);
    buf += banksize;
  }
  return meph->m_totLen;
} 

int MEPRx::createDAQErrorMEP(u_int8_t *buf , uint  nEvt) {
  struct MEPHdr *meph = (struct MEPHdr *) buf;
  int banksize = setupDAQErrorBankHdr();
  meph->m_l0ID = m_l0ID;
  meph->m_totLen = MEPHDRSIZ +  nEvt * (MEPFHDRSIZ + banksize);
  meph->m_nEvt = nEvt;
  buf += MEPHDRSIZ;
  for (uint i = 0; i < nEvt; ++i) {
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

int MEPRx::spaceAction() { 
  static  int id = -1; 
  MBM::EventDesc& dsc = event();
  MEPEVENT* e = (MEPEVENT*)dsc.data;
  MEPEvent* m = (MEPEvent*)e->data;
  if (m_nrx != m_nSrc) incompleteEvent();
  /// Add MDF bank
  u_int8_t *buf = (u_int8_t *) e->data + m_brx + 4 + IP_HEADER_LEN; 
  m_brx += createMDFMEP(buf, m_pf) + IP_HEADER_LEN;

  e->evID     = ++id;
  dsc.len     = m_brx + sizeof(MEPEVENT);
  dsc.mask[0] = dsc.mask[1] = dsc.mask[2] = dsc.mask[3] = 0xffffffff;
  if (!m_odinMEP && m_parent->m_expectOdin) {
      m_eventType = EVENT_TYPE_ERROR;
  }
  dsc.type    = m_eventType;    
  m->setSize(m_brx);
  declareEvent();
  return sendSpace();
}

int MEPRx::addMEP(int sockfd, const MEPHdr *hdr, int srcid) {
  MEPEVENT *e = (MEPEVENT *)event().data;
  if (m_seen[srcid]) multipleSrc(srcid);
  m_seen[srcid] = 1;       
  if (m_nrx == 0) {
    e->refCount    = m_refCount;
    e->evID        = hdr->m_l0ID;
    e->begin       = long(long(e)-m_mepID->mepStart);
    e->packing     = -1;
    e->valid       = 1;
    e->magic       = mep_magic_pattern();
    m_brx          = 0;
    m_pf           = hdr->m_nEvt;
    m_age          = MEPRxSys::ms2k();
    m_odinMEP      = NULL;
  }
  int len = MEPRxSys::recv_msg(sockfd, (u_int8_t*)e->data + m_brx + 4, 
			       MAX_R_PACKET, 0);
  if (len < 0) {
    ERRMSG(m_log,"failed to receive message");
    return MEP_ADD_ERROR;
  }    
  MEPHdr *newhdr = (MEPHdr*) ((u_int8_t*)e->data + m_brx + 4 + IP_HEADER_LEN);
  m_parent->m_totRxOct += len;
  m_parent->m_rxOct[srcid];
  m_brx += len;
  m_nrx++; 

  if (len != (hdr->m_totLen + IP_HEADER_LEN)) 
    badPkt(DAQErrorEntry::ShortPkt, srcid);   
  if (m_pf == 0) 
    badPkt(DAQErrorEntry::EmptyMEP, srcid);
  if (m_pf != newhdr->m_nEvt) 
    badPkt(DAQErrorEntry::WrongPackingFactor, srcid);   
  if (m_parent->m_srcFlags[srcid] & ODIN)
      m_odinMEP = (u_int8_t *) newhdr;
  m_parent->m_rxEvt[srcid] += m_pf;
  return (m_nrx == m_nSrc) ? spaceAction() : MEP_ADDED;
}

void MEPRQCommand::commandHandler(void) {
  MsgStream log(m_msgSvc,getName());
  int numMEPs = getInt();
  log << MSG::INFO << "Received command, sending " << numMEPs << " MEP requests....";
  if(m_mepRxObj->sendMEPReq(numMEPs).isSuccess())
    log << MSG::INFO << "OK." << endmsg;
  else
    log << MSG::INFO << "FAILED." << endmsg;
}

// Standard Constructor
MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc)
: Service(nam, svc), m_ebState(NOT_READY), m_incidentSvc(0)
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
  declareProperty("RxIPAddr",         m_rxIPAddr = "0.0.0.0");
  declareProperty("InitialMEPReqs",    m_initialMEPReq = 1);
  declareProperty("MEPsPerMEPReq",     m_MEPsPerMEPReq = 1);
  declareProperty("MEPRecvTimeout",   m_MEPRecvTimeout = 10);
  declareProperty("maxEventAge",      m_maxEventAge = 1000); // ms
  declareProperty("checkPartitionID",      m_checkPartitionID = false);
  declareProperty("dropIncompleteEvents", m_dropIncompleteEvents = false);
  declareProperty("nCrh", m_nCrh = 10);
  declareProperty("maxErrors", m_maxErrors = 50);
  declareProperty("nErrorSamples", m_nErrorSamples = 10);
  declareProperty("errorCheckInterval", m_errorCheckInterval = -1); // ms
  declareProperty("RTTCCompat", m_RTTCCompat = false);
  declareProperty("createDAQErrorMEP", m_createDAQErrorMEP = false);
  m_trashCan  = new u_int8_t[MAX_R_PACKET];
  m_expectOdin = false;
  m_mepRQCommand = new MEPRQCommand(this, msgSvc(), RTL::processName());
}

// Standard Destructor
MEPRxSvc::~MEPRxSvc(){
  delete [] (u_int8_t*) m_trashCan;
  delete m_mepRQCommand;
}

void MEPRxSvc::cryError() {
  MsgStream log(msgSvc(),"MEPRx");
  log << MSG::DEBUG << "Excessive errors: Going to state DAQ_ERROR" << endmsg;
  Incident inc("MEPRx", "DAQ_ERROR");
  m_incidentSvc->fireIncident(inc); // note that name - this is very safely chosen
                                    // just in case you want to fire something 
                                    // else than an incident!
  m_cryError = false;
}

void MEPRxSvc::truncatedPkt(RTL::IPHeader *iphdr) 
{
  MsgStream log(msgSvc(), "MEPRx");
  std::string hname, mesg;
  log << MSG::ERROR << "received packet with truncated MEP-header from " << 
    (MEPRxSys::name_from_addr(iphdr->saddr, hname, mesg) ? mesg : hname) <<
    endmsg;
}
// 0 if partition ID matches 
// 1 if it does not match
int MEPRxSvc::checkPartitionID(u_int32_t addr, struct MEPHdr *mephdr)
{
  if (mephdr->m_partitionID != m_partitionID) {
    MsgStream log(msgSvc(),"MEPRx");
    std::string hname, mesg;
    log << MSG::ERROR << "received MEP from " << 
      (MEPRxSys::name_from_addr(addr, hname, mesg) ? mesg : hname) <<
      " with wrong partitionID: 0x" << 
      std::hex << mephdr->m_partitionID  << ". I want 0x" << 
      m_partitionID << endmsg;
    m_totWrongPartID++;
    return 1;
}
  return 0;
}

void MEPRxSvc::removePkt()   {
  int len = MEPRxSys::recv_msg(m_dataSock, m_trashCan,MAX_R_PACKET,0);
  if (len < 0) {
    if (!MEPRxSys::rx_would_block())   {
      MsgStream log(msgSvc(),"MEPRx");
      ERRMSG(log,"recvmsg");
    }
  }
}

static int errorCheck(void *context) {
  MEPRxSvc *svc = (MEPRxSvc *) context;
  static int n_high = 0;
  static int old_errors = 0;
  int new_errors;
   
  new_errors = svc->m_incEvt + svc->m_notReqPkt + svc->m_totBadMEP + 
    svc->totWrongPartID();
  if (new_errors - old_errors > svc->m_maxErrors) {
    n_high++;
    if (n_high > svc->m_nErrorSamples) svc->cryError();
  } else n_high = 0;
  old_errors = new_errors;
  /* rearm */
  lib_rtl_set_timer(svc->m_errorCheckInterval, errorCheck, svc, 
		    &(svc->m_timer));
  return 1; /* uaexxxx */
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
  return --m_workDsc.end();
}

StatusCode MEPRxSvc::setupMEPReq(const std::string& odinName) {
  MsgStream log(msgSvc(), "MEPRx");
  std::string msg;
  u_int32_t add;

  if (odinName.empty()) {
    m_dynamicMEPRequest = false;
    log << MSG::INFO << "No address for ODIN. Dynamic MEP requests disabled!" << endmsg;
    return StatusCode::SUCCESS;
  }
  if (MEPRxSys::parse_addr(odinName,add) && MEPRxSys::addr_from_name(odinName,add,msg)) {
    return error("Invalid address for ODIN: "+odinName+" "+msg);
  } 
  m_odinIPAddr = add;
  m_dynamicMEPRequest = true;
  log << MSG::INFO << "Dynamic MEP requests will be sent to " 
      << MEPRxSys::dotted_addr(m_odinIPAddr) << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::sendMEPReq(int m) {
  if ( m_dynamicMEPRequest )   {
    mepreq.nmep = m;
    int n = MEPRxSys::send_msg(m_mepSock, m_odinIPAddr, MEP_REQ_TOS, &mepreq, MEP_REQ_LEN, 0);
    if (n == MEP_REQ_LEN)   {
      m_totMEPReq += m;
      m_totMEPReqPkt++;
      return StatusCode::SUCCESS;
    }
    if (n == -1)  {
      MsgStream log(msgSvc(),"MEPRx");
      ERRMSG(log,"send MEP request");
      return StatusCode::FAILURE;
    }
    return error("MEPRequest corrupted on send!");
  }
  return StatusCode::SUCCESS;
}

void MEPRxSvc::freeRx() {
  RTL::Lock lock1(m_usedDscLock);
  if (!m_usedDsc.empty()) {
    MEPRx *rx = *(--m_usedDsc.end());
    m_usedDsc.pop_back();
    int rc = rx->spaceRearm(0);  
    if (rc == MBM_NORMAL) {
      RTL::Lock lock2(m_freeDscLock);
      if (rx->rearmMEPReq()) sendMEPReq(m_MEPsPerMEPReq);
      if (rx->m_wasIncomplete) {
	rx->updateNoshow();
	rx->m_wasIncomplete = false;
      } else { 
	resetCounters(m_noShow, m_nSrc);
	sendMEPReq(m_MEPsPerMEPReq);
      }
      m_freeDsc.push_back(rx);
    }
    else if (rc != MBM_REQ_CANCEL) {
      error("timeout on getting space.");
    }
  }
}

void MEPRxSvc::forceEvent(RXIT &dsc) {
  (*dsc)->spaceAction();
  RTL::Lock lock(m_usedDscLock);
  m_usedDsc.push_back(*dsc);
  m_workDsc.erase(dsc);
}

// IRunable implementation: Run the object
StatusCode MEPRxSvc::run() {
  MsgStream log(msgSvc(), "MEPRx"); // message stream is NOT thread-safe
  RXIT rxit;
  MEPRx *rx;
  u_int8_t hdr[HDR_LEN];
  RTL::IPHeader *iphdr  = (RTL::IPHeader *)hdr;
  MEPHdr        *mephdr = (MEPHdr*) &hdr[IP_HEADER_LEN];;
  int srcid;

  // we are ready - wait for start
  while (m_ebState != RUNNING) {
    switch(m_ebState) {
    case STOPPED:
    case NOT_READY:
      log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
      return StatusCode::SUCCESS;
    case READY: 
      MEPRxSys::microsleep(100000); // 100 ms
      break;
    default: continue;
    }
  }
  // we got a START command - send a few MEP requests
  if (!sendMEPReq(m_initialMEPReq).isSuccess()) {
    log << MSG::WARNING << "Could not send " << m_initialMEPReq
        << " initial MEP requests." << endmsg;
  }
     
  for (;;) {
    int n = MEPRxSys::rx_select(m_dataSock, m_MEPRecvTimeout);  
    if (n ==  -1) {
      ERRMSG(log,"select");
      continue;
    }
    if (n == MEPRX_WRONG_FD) {
      error("wrong file-descriptor in select");
      continue;
    }
    if (n == 0) {
      /* We haven't received a MEP for quite some time. Update counter.
       */
      m_numMEPRecvTimeouts++;
      
      static int ncrh = 1;
      if (m_ebState != RUNNING) {
        for(RXIT w=m_workDsc.begin(); w != m_workDsc.end(); ++w)
          forceEvent(w);
        log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
        return StatusCode::SUCCESS;
      }
      ageEvents();
      if (--ncrh == 0) {
        log << MSG::DEBUG << "crhhh..." << m_freeDsc.size() << " free buffers. ";
	log << MSG::DEBUG << endmsg;
        for(size_t i = 0; i < m_workDsc.size(); ++i) {
	  log << MSG::DEBUG << "Event L0ID# " << m_workDsc[i]->m_l0ID << " is missing ";
          log << MSG::DEBUG << printnum(m_workDsc[i]->m_nSrc - m_workDsc[i]->m_nrx," MEP");
	}
	log << endmsg;
	ncrh = m_nCrh;
      }
      continue;
    }
    ageEvents();
    int len = MEPRxSys::recv_msg(m_dataSock, hdr, HDR_LEN, MEPRX_PEEK);
    if (len < 0) {
      if (!MEPRxSys::rx_would_block()) 
        ERRMSG(log,"recvmsg");
      continue;
    }
    m_totRxPkt++; 
    	//Don't have sourceID yet, count per source later...

    m_numMEPRecvTimeouts = 0;
    if (len < (int) HDR_LEN) {
      // somebody sends us a badly truncated packet 
      truncatedPkt(iphdr);
      removePkt();
      continue;
    }	
    if ((srcid = getSrcID(iphdr->saddr)) == - 1) {
      // we do not expect nor want this 
      removePkt();
      continue;
    }
    if (m_checkPartitionID && checkPartitionID(iphdr->saddr, mephdr)) {
      // we do not want this either 
      removePkt();
      continue;
    }
    if (!m_workDsc.empty() && mephdr->m_l0ID == m_workDsc.back()->m_l0ID) {
      rxit = --m_workDsc.end();
    } 
    else {
      rxit = lower_bound(m_workDsc.begin(), m_workDsc.end(), 
			 mephdr->m_l0ID,MEPRx::cmpL0ID);
      if (rxit == m_workDsc.end() || (*rxit)->m_l0ID != mephdr->m_l0ID) {
        // not found - get a new descriptor
        RXIT oldest = ageRx();
        try {
          if (m_freeDsc.empty()) {
            forceEvent(oldest);
            freeRx(); // only if not in separate thread
	    
          }
          while (m_freeDsc.empty()) MEPRxSys::microsleep(100) ; /* only necessary on 
                                                            multithreading */
          lib_rtl_lock(m_freeDscLock);
          rx = m_freeDsc.back();
          m_freeDsc.pop_back();
          lib_rtl_unlock(m_freeDscLock);
          rx->m_age = MEPRxSys::ms2k();
          rx->m_l0ID = mephdr->m_l0ID;
          RXIT j = lower_bound(m_workDsc.begin(),m_workDsc.end(),mephdr->m_l0ID,MEPRx::cmpL0ID);
          m_workDsc.insert(j, rx);
          rxit = lower_bound(m_workDsc.begin(),m_workDsc.end(),mephdr->m_l0ID,MEPRx::cmpL0ID);
        }
        catch(std::exception& e) {
          error(std::string("Exception ")+e.what());
        }
      } 
    }
    m_rxPkt[srcid]++; //recieved packets per source
    if ((*rxit)->addMEP(m_dataSock, mephdr, srcid) == MEP_SENT) {
      rx = *rxit;
      m_workDsc.erase(rxit);
      lib_rtl_lock(m_usedDscLock);
      m_usedDsc.push_back(rx);     
      lib_rtl_unlock(m_usedDscLock);
      freeRx();
    }              
  } // for{;;}
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

void MEPRxSvc::srcSwap(int i, int j) {
  string name, addr; int flags;
  name = m_srcName[i]; m_srcName[i] = m_srcName[j]; m_srcName[j] = name;
  addr = m_srcDottedAddr[i]; m_srcDottedAddr[i] = m_srcDottedAddr[j]; m_srcDottedAddr[j] = addr;
  flags = m_srcFlags[i]; m_srcFlags[i] = m_srcFlags[j]; m_srcFlags[j] = flags;
}


int MEPRxSvc::srcFindMedianIndex(int left, int right, int shift) {
  int i, groups = (right - left)/shift + 1, k = left + groups/2*shift;
  for (i = left; i <= k; i+= shift) {
    int minIndex = i; string minValue = m_srcName[minIndex]; int j;
    for (j = i; j <= right; j += shift) 
      if (m_srcName[j] < minValue) {
	minIndex = j;
	minValue = m_srcName[minIndex];
      }
    srcSwap(i, minIndex);
  }
  return k;
}

int MEPRxSvc::srcFindMedianOfMedians(int left, int right) {
  
  if (left == right) return left;
  int i, shift = 1;
  while (shift <= (right - left)) {
    for (i = left; i <= left; i += shift * 5) {
      int endIndex = (i + shift * 5 - 1 < right) ? i + shift * 5 - 1 : right;
      int medianIndex = srcFindMedianIndex(i, endIndex, shift);
      srcSwap(i, medianIndex);
    }
    shift *= 5;
  }
  return left;
}
  
int MEPRxSvc::srcPart(int left, int right) {
  srcFindMedianOfMedians(left, right);
  int pivotIndex = left, index = left;
  string pivVal = m_srcName[pivotIndex];
  srcSwap(pivotIndex, right);
  for (int i = left; i < right; ++i) {
    if (m_srcName[i] <= pivVal) 
      srcSwap(i, index++);
  }
  srcSwap(right, index);
  return index;
}

void MEPRxSvc::srcSort(int left, int right) {
  if (m_srcName[left] >= m_srcName[right]) return;
  int index = srcPart(left, right);
  srcSort(left, index - 1);
  srcSort(index + 1, right);
}
   
StatusCode MEPRxSvc::checkProperties() {
  std::ostringstream log;
  if (m_ethInterface < 0)
    return error("ethDevIn must be >= 0");
  else if (m_maxMsForGetSpace < 1)
    return error("maxMsForGetSpace must be > 0");
  else if (m_MEPBuffers < 1 || m_MEPBuffers > 256)
    return error("MEPBuffers must be > 0 and < 256");
  else if (m_pktSamplingCount < 0)
    return error("pktSamplingCount must be >= 0");
  else if (m_maxBadPktRatio <= 0)
    return error("maxBadPktRatio must be > 0");
  else if (m_sockBuf < 0x10000 || m_sockBuf > 0x4000000)
    return error("sockBuf must be >= 64 kB and less than 64 MB");
  else if (m_IPProtoIn < 0 || m_IPProtoIn > 255)
    return error("IPProtoIn is an unsigned 8 bit quantity");
  else if ((m_IPSrc.size() % 3) != 0)
    return error("Malformed source list (length is odd)");
  else if (m_MEPBufSize < 0) {
    log << "Illegal value for MEPBufSize " << m_MEPBufSize;
    return error(log.str());
  }      
  else if ( !setupMEPReq(m_IPNameOdin).isSuccess() ) {
    return error("Bad address IPNameOdin "+m_IPNameOdin);
  }
  for (uint i = 0; i < m_IPSrc.size(); i += 3) {
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
          nam = snprintf(tmp, 15, "src-%d", i/3);
      }
      else {
        nam = m_IPSrc[i + 1];
      }
    }

    m_srcFlags.push_back(0);
    if (m_IPSrc[i + 2] == "DOUBLE_ZERO_BUG") 
      m_srcFlags[i/3] |= DOUBLE_ZERO_BUG;
    if (m_IPSrc[i + 2] == "ODIN") {
      m_srcFlags[i/3] |= ODIN;	
      m_expectOdin = true;
    }
    m_srcDottedAddr.push_back(MEPRxSys::dotted_addr(addr));
    m_srcName.push_back(nam);
  }
  m_nSrc = m_IPSrc.size() / 3;
  // sort sources alphabetically
  //c0, m_nSrc - 1);
  // create hash-map for finding addresses
  for (int i = 0; i < m_nSrc; ++i) 
    m_srcAddr[MEPRxSys::cinet_addr(m_srcDottedAddr[i])] = i;
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::allocRx() {
  char num[32];
  MsgStream log(msgSvc(),"MEPRx");
  for(int i = 0; i < m_MEPBuffers; ++i) {
    ::sprintf(num, "MEPRx%03d", i);      
    MEPRx *rx = new MEPRx(num, this);
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
  std::for_each(m_freeDsc.begin(), m_freeDsc.end(), MEPRx::release);
  m_freeDsc.clear();
  std::for_each(m_usedDsc.begin(), m_usedDsc.end(), MEPRx::release);
  m_usedDsc.clear();
  std::for_each(m_workDsc.begin(), m_workDsc.end(), MEPRx::release);
  m_workDsc.clear();
  return StatusCode::SUCCESS;
}

int MEPRxSvc::openSocket(int protocol) {
  std::string msg;
  int retSock;
  if ((retSock = MEPRxSys::open_sock(protocol, 
                          m_sockBuf,
                          m_ethInterface,
                          m_rxIPAddr, 
                          m_dynamicMEPRequest, 
                          msg)) < 0) 
  {
    MsgStream log(msgSvc(),"MEPRx");
    ERRMSG(log,msg);
  }
  return retSock;
}

int MEPRxSvc::getSrcID(u_int32_t addr)  {
  std::map<u_int32_t, int>::iterator i = m_srcAddr.find(addr);
  if (i == m_srcAddr.end()) {
    std::string hname, mesg;
    error("received unexpected packet from " + (
          MEPRxSys::name_from_addr(addr, hname, mesg) ? mesg : hname));
    m_notReqPkt++;
    return -1;
  }
  return i->second;
}

void MEPRxSvc::ageEvents() {
  if (m_maxEventAge == 0) return;	
  unsigned long ms = MEPRxSys::ms2k();
 ageloop:
  for (RXIT w=m_workDsc.begin(); w != m_workDsc.end(); ++w) {
    if ((ms - ((*w)->m_age)) > m_maxEventAge) { 
      forceEvent(w);
      freeRx(); // only if not in separate thread
      goto ageloop;
    }    
  }
}

void MEPRxSvc::publishCounters()
{
  PUBCNT(totRxOct,           "Total received bytes");
  PUBCNT(totRxPkt,           "Total received packets");
  PUBCNT(incEvt,             "Incomplete events");
  PUBCNT(totBadMEP,          "Total bad MEPs");
  PUBCNT(totMEPReq,          "Total requested MEPs");
  PUBCNT(totMEPReqPkt,       "Total Sent MEP-request packets");
  PUBCNT(numMEPRecvTimeouts, "MEP-receive Timeouts");
  PUBCNT(notReqPkt,          "Total unsolicited packets");
  PUBCNT(totWrongPartID,     "Packets with wrong partition-ID");
  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
  PUBARRAYCNT(misPkt,        "Missing MEPs");
  PUBARRAYCNT(badPckFktPkt,  "MEPs with wrong packing (MEP) factor");
  PUBARRAYCNT(truncPkt,      "Truncated (empty) MEPs");
  PUBARRAYCNT(multipleEvt,   "Duplicate Events");
  PUBARRAYCNT(rxOct,	     "Received bytes");
  PUBARRAYCNT(rxPkt,         "Received packets");
  PUBARRAYCNT(rxEvt,         "Received events");
}

void MEPRxSvc::clearCounters() {
  resetCounters(m_rxOct, m_nSrc);
  resetCounters(m_rxPkt, m_nSrc);
  resetCounters(m_rxEvt, m_nSrc);
  resetCounters(m_badPckFktPkt, m_nSrc);
  resetCounters(m_badLenPkt, m_nSrc);
  resetCounters(m_misPkt, m_nSrc);
  resetCounters(m_truncPkt, m_nSrc);
  resetCounters(m_multipleEvt, m_nSrc);
  resetCounters(m_noShow, m_nSrc);
  m_totMEPReq          = 0;
  m_totMEPReqPkt       = 0;
  m_numMEPRecvTimeouts = 0;
  m_notReqPkt          = 0;
  m_totRxOct           = 0;
  m_totRxPkt           = 0;
  m_incEvt             = 0;
  m_totMEPReqPkt       = 0;
  m_numMEPRecvTimeouts = 0;
  m_totMEPReq          = 0;
  m_totBadMEP          = 0;
  m_totWrongPartID     = 0;
} 

int MEPRxSvc::setupCounters() {
  MsgStream log(msgSvc(),"MEPRx");
  clearCounters();
  publishCounters();

  // create cstring for source names
  std::string all_names = "";
  for (unsigned i = 0; i < m_srcName.size(); ++i)  all_names = all_names + '\0' + m_srcName[i];
  if (!(m_allNames = new char[all_names.size()+1]))
    return 1;
   ::memcpy(m_allNames, (const char *) all_names.data(), all_names.size() +1);
  
  m_monSvc->declareInfo("srcName", "C:", m_allNames, sizeof(m_srcName), "Source IP names", this);
  log << MSG::INFO << all_names << all_names.size() << endmsg;
  return 0;
}

void  MEPRxSvc::handle(const Incident& inc)    {
  MsgStream log(msgSvc(),"MEPRx");
  log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  {
    m_ebState = STOPPED;
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_ebState = RUNNING;
  }
}

int MEPRxSvc::setupTimer(void) {
  unsigned long t;
  if (m_errorCheckInterval <= 0) return 0;
  if (lib_rtl_set_timer(m_errorCheckInterval, errorCheck, this, &t)) {
    m_timer = t;
    return 0; // in the weird world of the OnlineKernel 0 = 1 and 1 = 0 ...
  }
  return 1;
}

StatusCode MEPRxSvc::initialize()  {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess())
    return error("Failed to initialize service base class.");
  else if ( !checkProperties().isSuccess() )  
    return error("Failed to check properties.");
  else if ((m_mepSock = openSocket(MEP_REQ_TOS)) < 0)
    return error("Failed to open socket: MEP_REQ_TOS");
  else if ((m_dataSock = openSocket(m_IPProtoIn)) < 0)
    return error("Failed to open socket: IPProtoIn");
  else if ( !allocRx().isSuccess()         )
    return error("Failed to allocate RX");
  else if (lib_rtl_create_lock(0, &m_usedDscLock) != 1)
    return error("Failed to create lock m_usedDscLock.");
  else if (lib_rtl_create_lock(0, &m_freeDscLock) != 1)
    return error("Failed to create lock m_freeDscLock.");
  else if (!service("IncidentSvc", m_incidentSvc).isSuccess())
    return error("Failed to access incident service.");
  else {
    m_incidentSvc->addListener(this, "DAQ_CANCEL");
    m_incidentSvc->addListener(this, "DAQ_ENABLE");
  }
  if (service("MonitorSvc", m_monSvc).isSuccess()) {
    if (setupCounters()) {
      return error("Failed to setup counters");
    }
  }
  else {
    return error("Failed to access monitor service.");
  }
  if (setupTimer()) 
    return error("Failed to initialize timer.");
  m_ebState = READY;
  return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::finalize()  {
  MsgStream log(msgSvc(),"MEPRx");

  log << MSG::INFO << "Entering finalize....." << endmsg;
  if (m_errorCheckInterval > 0) 
    lib_rtl_kill_timer(m_timer);
  releaseRx();
  if (m_incidentSvc) {
    m_incidentSvc->removeListener(this);
    m_incidentSvc->release();
    m_incidentSvc = 0;
  }
  if (m_monSvc) {
    m_monSvc->undeclareAll(this);
    m_monSvc->release();
    m_monSvc = 0;
  }
  m_ebState = NOT_READY;
  return Service::finalize();
}
#if 0
;;; Local Variables: ***
;;; c-basic-offset:2 ***
;;; End: ***
#endif
