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
//      Version   : $Id: MEPRxSvc.cpp,v 1.94 2010-11-04 07:39:24 frankb Exp $
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
#include "GaudiKernel/IUpdateable.h"
#include "GaudiKernel/xtoa.h"
#include "EventBuilder/MEPRxSvc.h"
#include "EventBuilder/MEPHdr.h"
#include "EventBuilder/MEPRxSys.h"
#include "NET/IPHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MEPEvent.h"
#include "Event/RawBank.h"

typedef unsigned int uint;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPRxSvc)

#define MAX_R_PACKET (0x10000 + 20)
#define MEP_SENT  MBM_NORMAL
#define MEP_ADDED (MEP_SENT + 1)
#define IP_HEADER_LEN 20
#define IP_PROTO_HLT  0xF2

#ifndef MEP_ADD_ERROR
#define MEP_ADD_ERROR -9999
#endif
/* maximum number of sources (TELL1, RS, UKL1) */
//#ifndef MAX_SRC
// This must provoke an error!  No protection (M.Frank)
#define MAX_SRC 500
//#endif

#define RAWBHDRSIZ (sizeof(RawBank) - sizeof(int)) /* f*** C99 */
#define MEPHDRSIZ   sizeof(MEPHdr)
#define MEPFHDRSIZ  sizeof(MEPFrgHdr)

#ifdef _WIN32
#define ERRMSG(a,x) do {  \
  a <<  MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<<  ":(" << __LINE__ << ")" << endmsg;} while(0);
#endif

#define PUBCNT(name, desc)   do {m_ ## name = 0; m_monSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
// Sent as 64bit integers ("X")
#define PUB64CNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name,"X", & m_ ## name, sizeof(int64_t), desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "X", & m_ ## name [0], m_ ## name.size() * sizeof(int64_t), desc, this);} while(0);

#define printnum(n,s)     n << s << ((char*)(n == 1 ? "" : "s"))

#define HDR_LEN (IP_HEADER_LEN + sizeof(LHCb::MEPHdr))

static MEPReq mepreq;

#ifndef EVENT_TYPE_BROKEN
#define EVENT_TYPE_BROKEN 16
#endif

// Event Source Flags
#define DOUBLE_ZERO_BUG    1   // source will send two events with L0ID == 0
#define IS_ODIN            2   // this is the primary ODIN for MDF function
template<typename T> static void resetCounter(T& cnt)
{
    cnt = 0;
}
template<typename T> static void resetCounters(T& cnt, size_t len)
{
    cnt.resize(len, 0);
    std::for_each(cnt.begin(), cnt.end(), resetCounter<typename T::value_type> );
}
using std::string;
using std::hex;
using std::dec;

namespace LHCb {
struct MEPBuf: public MBM::Producer {
    static void cancelReq(MEPBuf *p) {
        if (p)
            ::mbm_cancel_request(p->m_bmid);
    }
    MEPBuf(const std::string &nam, const std::string &bufnam, int partitionID);
    virtual ~MEPBuf();
};

struct MEPRx {
    MEPRxSvc* m_parent;
    // parameters
    int m_refCount;
    int m_spaceSize;
    unsigned long m_age;
    int m_nSrc;
    int m_nrx;
    // run-time
    u_int32_t m_runNumber;
    u_int32_t m_l0ID;
    u_int32_t m_prevL0ID;
    u_int32_t m_partID;
    u_int32_t m_brx;
    u_int16_t m_pf;
    u_int64_t m_hdrtsc; // tsc after reception of MEP header
    u_int64_t m_tFirstFrag; // time of first MEP received (us)
    u_int64_t m_tCurFrag; // time of current MEP receved (us)
    int m_spaceRC;
    int m_eventType;
    int m_seen[MAX_SRC];
    int m_maxMsForGetSpace;
    bool m_dry;
    bool m_wasIncomplete;
    MsgStream m_log;
    DAQErrorEntry m_eEntry[MAX_SRC];
    u_int32_t m_daqError;
    RawBank *m_rawBufHdr, *m_MDFBankHdr, *m_DAQBankHdr, *m_ODINBank;
    u_int8_t *m_odinMEP;
    struct MEPBuf *m_buf, *m_ovflBuf, *m_liveBuf;
    MEPRx(const std::string &nam, MEPRxSvc *parent);
    virtual ~MEPRx();
    static void release(MEPRx* p) {
        delete p;
    }
    static void cancelReq(MEPRx* p) {
        //if (p) p->m_buf->cancelReq(p->m_buf);
        if (p) {
            if (p->m_ovflBuf)
                p->m_ovflBuf->cancelReq(p->m_ovflBuf);
            if (p->m_liveBuf)
                p->m_liveBuf->cancelReq(p->m_liveBuf);
        }
    }
    static bool cmpL0ID(MEPRx *r, u_int32_t id) {
        return r->m_l0ID < id;
    }
    int createDAQErrorBankEntries();
    int setupDAQErrorBankHdr();
    int createDAQErrorMEP(u_int8_t *buf, uint nEvt);
    void addODINInfo(int);
    void setupMDFBank(u_int32_t, u_int32_t, u_int32_t);
    void setupODINBank(int n, u_int32_t);
    int createMDFMEP(u_int8_t *buf, int nEvt);
    int createODINMEP(u_int8_t *buf, int nEvt);
    void incompleteEvent();
    int spaceAction();
    int spaceRearm(int);
    void updateNoshow(void);
    void badPkt(DAQErrorType, int);
    void multipleSrc(int);
    int addMEP(int sockfd, const MEPHdr *hdr, int srcid, u_int64_t tsc,
               u_int64_t rxtim);
    int analyzeMEP(MEPHdr *mep, uint &nfrag);
};
}

using namespace LHCb;

#define FULLNAME(id) m_parent->m_srcName[id] + " (" + m_parent->m_srcDottedAddr[id] + ")"
#define HOSTNAME(id) m_parent->m_srcName[id]

MEPBuf::MEPBuf(const std::string &bufnam, const std::string &nam, int partitionID) :
    MBM::Producer(bufnam, nam, partitionID)
{
}
MEPBuf::~MEPBuf()
{
}

MEPRx::MEPRx(const string &nam, MEPRxSvc *parent) :
    m_parent(parent), m_refCount(parent->mepRefcount()), m_spaceSize(
        parent->spaceSize()), m_nSrc(parent->numberOfSources()), m_log(
            parent->msgSvc(), nam), m_ovflBuf(0)
{
    std::string liveBufName = parent->m_nameLiveBuf;
    std::string overBufName = parent->m_nameOverflowBuf;
    if ( parent->m_partitionBuffers ) {
  liveBufName += "_";
  overBufName += "_";
  if ( !parent->m_partitionName.empty() )  {
    liveBufName += parent->m_partitionName;
    overBufName += parent->m_partitionName;
  }
  else {
          char txt[32];
    liveBufName += _itoa(parent->m_partitionID,txt,16);
    overBufName += _itoa(parent->m_partitionID,txt,16);
  }
    }
    m_liveBuf = new MEPBuf(liveBufName, nam, parent->partitionID());
    if ( !parent->m_nameOverflowBuf.empty() ) {
  m_ovflBuf = new MEPBuf(overBufName, nam, parent->partitionID());
    }
    m_buf = m_liveBuf;
    m_log << MSG::DEBUG << std::hex << std::showbase
          << "Buffer space:        " << m_spaceSize << " bytes" << endmsg;
    m_eventType = EVENT_TYPE_MEP;
    m_rawBufHdr = (class RawBank*) new u_int8_t[sizeof(RawBank) + 16];
    size_t len = sizeof(RawBank) + MDFHeader::sizeOf(1);
    m_MDFBankHdr = (class RawBank*) new u_int8_t[len];
    m_MDFBankHdr->setType(RawBank::DAQ);
    m_MDFBankHdr->setSize(MDFHeader::sizeOf(1));
    m_MDFBankHdr->setVersion(DAQ_STATUS_BANK);
    m_MDFBankHdr->setSourceID(1025);
    m_MDFBankHdr->setMagic();
    len = sizeof(RawBank) + sizeof(OnlineRunInfo);
    m_ODINBank = (class RawBank *) new u_int8_t[len];
    m_ODINBank->setType(RawBank::ODIN);
    m_ODINBank->setSize(sizeof(OnlineRunInfo));
    m_ODINBank->setVersion(6); // as seen on the wire 26/05/11
    m_ODINBank->setSourceID(16); // as seen on the wire 26/05/11
    m_ODINBank->setMagic();
    m_prevL0ID = 0;
    m_wasIncomplete = false;
}

MEPRx::~MEPRx()
{
    delete[] (u_int8_t *) m_rawBufHdr;
    delete[] (u_int8_t *) m_MDFBankHdr;
    if ( m_liveBuf ) delete m_liveBuf;
    if ( m_ovflBuf ) delete m_ovflBuf;
}

int MEPRx::spaceRearm(int)
{
    int sc;

    m_eventType = EVENT_TYPE_MEP;
    m_brx = m_nrx = 0;
    memset(m_seen, 0, m_nSrc * sizeof(int));
    m_parent->m_overflowActive = false;
    if (m_parent->m_overflow) {
        sc = m_ovflBuf ? m_liveBuf->getSpaceTry(m_spaceSize)
      : m_liveBuf->getSpace(m_spaceSize);
        if (sc == MBM_REQ_CANCEL)
            return sc;
        if (sc == MBM_NORMAL) {
            m_buf = m_liveBuf;
            return sc;
        }
        if (m_ovflBuf && sc == MBM_NO_ROOM) {
            // Assign variable first. Otherwise Cancel on stop
            // goes to the wrong buffer ....
            m_buf = m_ovflBuf;
            sc = m_ovflBuf->getSpace(m_spaceSize);
            m_parent->m_overflowActive = true;
            return sc;
        }
        // something went wrong
        return sc;
    }
    m_buf = m_liveBuf;
    return m_buf->getSpace(m_spaceSize);
}

void MEPRx::updateNoshow()
{
    for (int i = 0; i < m_nSrc; ++i)
        m_parent->m_noShow[i] = m_seen[i];
}

void MEPRx::multipleSrc(int srcid)
{
    m_eventType = EVENT_TYPE_ERROR;
    m_log << MSG::ERROR << "Multiple event from " << m_parent->m_srcName[srcid]
          << " (" << m_parent->m_srcDottedAddr[srcid] << ")" << endmsg;
    m_parent->m_multipleEvt[srcid]++;
}

void MEPRx::badPkt(DAQErrorType type, int srcid)
{
    m_eventType = EVENT_TYPE_ERROR;
    m_daqError |= type;
    m_log << MSG::ERROR << "Bad packet from " << HOSTNAME(srcid) << " [";
    switch (type) {
    case EmptyMEP:
        m_parent->m_truncPkt[srcid]++;
        m_log << "empty MEP";
        break;
    case WrongPackingFactor:
        m_parent->m_badPckFktPkt[srcid]++;
        m_log << "mismatch in packing factor";
        break;
    case ShortPkt:
        m_parent->m_badLenPkt[srcid]++;
        m_log << "mismatch between length on wire and in MEP header";
        break;
    default:
        m_log << "Mysterious error: " << type;
    }
    m_log << "]" << endmsg;
    (m_parent->m_totBadMEP)++;
}

int MEPRx::createDAQErrorBankEntries()
{
    int j = 0;
    for (int i = 0; i < m_nSrc; ++i) {
        if (m_seen[i] == 1)
            continue;
        m_eEntry[j].m_srcID = i;
        m_eEntry[j].m_srcIPAddr = m_parent->sourceAddr(i);
        m_eEntry[j].m_errorType = MissingSrc;
        m_eEntry[j].m_pktData = NULL;
        j++;
    }
    return j;
}

int MEPRx::setupDAQErrorBankHdr()
{
    int n = createDAQErrorBankEntries();
    int size = n * sizeof(DAQErrorEntry) + RAWBHDRSIZ;
    m_rawBufHdr->setMagic();
    m_rawBufHdr->setSourceID(m_parent->sourceID());
    m_rawBufHdr->setVersion(DAQ_ERR_BANK_VERSION);
    m_rawBufHdr->setType(RawBank::DAQ);
    m_rawBufHdr->setSize(size);
    return (size % 4) ? size + 4 - (size % 4) : size;
}

// 0 MEP syntactically correct
// 1 syntactically corrupted (pointers don't work out)
// 2 low word of l0_id's wrong
// assumes that length in MEP header has been verified
int MEPRx::analyzeMEP(MEPHdr *mep, uint &nfrag)
{
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
        nfrag++;
        n += fh->m_len + MEPFHDRSIZ;
    }
    if (n + MEPFHDRSIZ > len) {
        // warn about bad MEP
        err |= 2;
    }
    return err;
}

void MEPRx::incompleteEvent()
{
    int i, nmiss = 0;
    MSG::Level sev = m_parent->m_partitionName == "LHCb" ? MSG::INFO : MSG::ERROR;

    m_parent->addIncompleteEvent();
    for (i = 0; i < m_nSrc; ++i)
        nmiss += (m_seen[i] ? 0 : 1);
    if (nmiss <= m_nSrc / 2) {
        m_log << sev << std::dec << "Run # " << m_runNumber
              << " - Incomplete Event #" << m_l0ID << "  No packet from: ";
        for (int i = 0; i < m_nSrc; ++i)
            if (!m_seen[i]) {
                m_log << HOSTNAME(i) << " ";
                m_parent->m_misPkt[i]++;
            }
    } else {
        m_log << sev << std::dec << "Run # " << m_runNumber
              << " - Incomplete Event #" << m_l0ID << "  Only packets from: ";
        for (int i = 0; i < m_nSrc; ++i)
            if (m_seen[i]) {
                m_log << HOSTNAME(i) << " ";
                m_parent->m_misPkt[i]++;
            }
    }
    m_log << endmsg;
    m_eventType = EVENT_TYPE_ERROR;
    m_daqError |= MissingSrc;
    m_wasIncomplete = true;

    return;
}

void MEPRx::setupMDFBank(u_int32_t run, u_int32_t orbit, u_int32_t bunchID)
{
    uint mask[] = { ~0u, ~0u, ~0u, ~0u };
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

void MEPRx::setupODINBank(int n, u_int32_t orbit_or_timestamp)
{
    OnlineRunInfo *bank = (OnlineRunInfo *) m_ODINBank->data();
    bank->L0ID = m_l0ID + n;
    bank->Orbit = orbit_or_timestamp;
}

void MEPRx::addODINInfo(int n)
{
    if (m_odinMEP == NULL)
        setupMDFBank(0, 0, 0);
    else {
        LHCb::RawBank *bank = (LHCb::RawBank *) (((u_int8_t *) m_odinMEP)
                              + MEPHDRSIZ + MEPFHDRSIZ + n * (RAWBHDRSIZ
                                      + sizeof(LHCb::OnlineRunInfo) + MEPFHDRSIZ));
        LHCb::OnlineRunInfo *odin = bank->begin<LHCb::OnlineRunInfo> (); // raetselhaft
        setupMDFBank(odin->Run, odin->Orbit, odin->bunchID);
    }
}

int MEPRx::createODINMEP(u_int8_t *buf, int nEvt)
{
    struct MEPHdr *meph = (struct MEPHdr *) buf;
    size_t banksize = m_ODINBank->totalSize();
    meph->m_l0ID = m_l0ID;
    meph->m_totLen = MEPHDRSIZ + nEvt * (MEPFHDRSIZ + banksize);
    meph->m_nEvt = nEvt;
    buf += MEPHDRSIZ;
    for (int i = 0; i < nEvt; ++i) {
        struct MEPFrgHdr *frgh = (struct MEPFrgHdr *) buf;
        frgh->m_l0IDlow = 0xFFFF & (m_l0ID + i);
        frgh->m_len = banksize;
        buf += MEPFHDRSIZ;
        setupODINBank(i, m_partID);
        memcpy(buf, m_ODINBank, banksize);
        buf += banksize;
    }
    return meph->m_totLen;
}

int MEPRx::createMDFMEP(u_int8_t *buf, int nEvt)
{
    struct MEPHdr *meph = (struct MEPHdr *) buf;
    size_t banksize = m_MDFBankHdr->totalSize();
    meph->m_l0ID = m_l0ID;
    meph->m_totLen = MEPHDRSIZ + nEvt * (MEPFHDRSIZ + banksize);
    meph->m_nEvt = nEvt;
    buf += MEPHDRSIZ;
    for (int i = 0; i < nEvt; ++i) {
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

int MEPRx::createDAQErrorMEP(u_int8_t *buf, uint nEvt)
{
    struct MEPHdr *meph = (struct MEPHdr *) buf;
    int banksize = setupDAQErrorBankHdr();
    meph->m_l0ID = m_l0ID;
    meph->m_totLen = MEPHDRSIZ + nEvt * (MEPFHDRSIZ + banksize);
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

int MEPRx::spaceAction()
{
    static int id = -1;
    MBM::EventDesc& dsc = m_buf->event();
    MEPEVENT* e = (MEPEVENT *) dsc.data;
    MEPEvent* m = (MEPEvent*) e->data;

    if (m_nrx != m_nSrc)
        incompleteEvent();
    /// Add MDF bank
    u_int8_t *buf = (u_int8_t *) e->data + m_brx + 4 + IP_HEADER_LEN;
    m_brx += createMDFMEP(buf, m_pf) + IP_HEADER_LEN;
    if (m_parent->m_createODINMEP) {
        buf = (u_int8_t *) e->data + m_brx + 4 + IP_HEADER_LEN;
        m_brx += createODINMEP(buf, m_pf) + IP_HEADER_LEN;
    }
    e->evID = ++id;
    dsc.len = m_brx + sizeof(MEPEVENT);
    dsc.mask[0] = dsc.mask[1] = dsc.mask[2] = dsc.mask[3] = 0xffffffff;
    if (!m_odinMEP && m_parent->m_expectOdin) {
        m_eventType = EVENT_TYPE_ERROR;
        m_daqError |= MissingOdin;
    }
    if (m_eventType == EVENT_TYPE_ERROR) {
        dsc.mask[3] = m_daqError;
        if (m_parent->m_createDAQErrorMEP) {
            u_int8_t *buf = (u_int8_t *) e->data + m_brx + 4 + IP_HEADER_LEN;
            m_brx += createDAQErrorMEP(buf, m_pf) + IP_HEADER_LEN;
        }
    }
    dsc.type = m_eventType;
    m->setSize(m_brx);
    int sc;
    try
    {
      m_buf->declareEvent();
      sc = m_buf->sendSpace();
      if (m_eventType == EVENT_TYPE_MEP) {
          (m_parent->m_totMEPproduced)++;
          if (m_buf == m_liveBuf)
              (m_parent->m_totMEPproducedLive)++;
          else (m_parent->m_totMEPproducedOvfl)++;
      }
    }
    catch (std::runtime_error &e) {
      sc = MBM_ERROR;
    }
    m_parent->m_complTimeSock->fill(1.0 * (m_age), 1.0);
    m_parent->m_L0IDDiff->fill(1.0 * m_l0ID - m_prevL0ID, 1.0);
    m_parent->m_tLastComp = m_parent->m_tLastAdded;
    m_prevL0ID = m_l0ID;
    return sc;
}

int MEPRx::addMEP(int sockfd, const MEPHdr *hdr, int srcid, u_int64_t tsc,
                  u_int64_t rxtim)
{
    MEPEVENT *e = (MEPEVENT *) m_buf->event().data;
    if (m_seen[srcid])
        multipleSrc(srcid);
    m_seen[srcid] = 1;
    if (m_nrx == 0) {
        e->refCount = m_refCount;
        e->evID = hdr->m_l0ID;
        //    e->begin       = long(long(e) - m_mepID->mepStart);
        e->begin = long(long(e) - (long) m_buf->bufferAddress());
        e->packing = -1;
        e->valid = 1;
        e->magic = mep_magic_pattern();
        ::memset(e->events, 0, sizeof(e->events));
        m_brx = 0;
        m_pf = hdr->m_nEvt;
        m_age = 0; // us
        m_odinMEP = NULL;
        m_tFirstFrag = rxtim;
        m_daqError = 0;
        m_eventType = EVENT_TYPE_MEP;
    }
    m_hdrtsc = tsc;
    u_int64_t bodytsc;
    string errstr;

    if (abs(m_brx + 4 + MAX_R_PACKET) > abs(m_spaceSize)) {
        m_log << MSG::ERROR << "Buffer too small - event truncated!" << endmsg;
        return MEP_ADD_ERROR;
    }
    int len = MEPRxSys::recv_msg(sockfd, (u_int8_t *) e->data + m_brx + 4,
                                 MAX_R_PACKET, 0, &bodytsc, &m_tCurFrag, errstr);
    if (len < 0) {
        ERRMSG(m_log,"failed to receive message");
        return MEP_ADD_ERROR;
    }
    if (m_tCurFrag == 0) {
        ERRMSG(m_log, errstr);
    }
    MEPHdr *newhdr = (MEPHdr*) ((u_int8_t*) e->data + m_brx + 4 + IP_HEADER_LEN);
    m_parent->m_rxOct[srcid] += len;
    m_parent->m_totRxOct += len;
    m_brx += len;
    m_nrx++;
    if (len != (hdr->m_totLen + IP_HEADER_LEN))
        badPkt(ShortPkt, srcid);
    if (m_pf == 0)
        badPkt(EmptyMEP, srcid);
    if (m_pf != newhdr->m_nEvt)
        badPkt(WrongPackingFactor, srcid);
    if (m_parent->m_srcFlags[srcid] & IS_ODIN) {
        m_odinMEP = (u_int8_t *) newhdr;
        // Record the run number of this event
        MEPFrgHdr* mepfrghdr = (MEPFrgHdr *)  (((u_int8_t *) m_odinMEP) + MEPHDRSIZ);
        for (int i = 0; i < m_pf; ++i) {
          LHCb::RawBank *bank = (LHCb::RawBank *) (((u_int8_t *) mepfrghdr) + MEPFHDRSIZ);
          if (i == 0) {
            LHCb::OnlineRunInfo *odin = bank->begin<LHCb::OnlineRunInfo> (); // raetselhaft
            m_runNumber = odin->Run; // do this only for index 0 to preserve original semantic
          }
          if (bank->magic() != RawBank::MagicPattern) {
              m_log << MSG::ERROR << "Foul! Event " << m_l0ID << " subevent lsb" << mepfrghdr->m_l0IDlow << " fragment # " << i <<
                hex <<  " ODIN magic pattern is " << hex
                    << bank->magic() << endmsg;

          }
          if (bank->type() != RawBank::ODIN) {
              m_log << MSG::ERROR << "Foul! Event " << m_l0ID << " subevent lsb" << mepfrghdr->m_l0IDlow << " fragment # " << i <<
              "ODIN bank type is " << hex << bank->type() <<
                    " should be " << RawBank::ODIN << endmsg;
          }
          mepfrghdr = (MEPFrgHdr *) (((u_int8_t *) mepfrghdr) + mepfrghdr->m_len + MEPFHDRSIZ);
        }
    }
    m_parent->m_rxEvt[srcid] += m_pf;
    m_parent->m_totRxEvt += m_pf;
    m_parent->m_rxMEP[srcid]++;
    m_parent->m_totRxMEP++;
    m_parent->m_tLastAdded = rxtim;
    m_partID = hdr->m_partitionID;
    m_age = m_tCurFrag - m_tFirstFrag;
    // if (m_nrx == m_nSrc) m_log << MSG::ERROR << m_nrx <<  m_nSrc << endmsg;
    return (m_nrx == m_nSrc) ? spaceAction() : MEP_ADDED;
}

// DIM commands & services
void SetOverflowCmd::commandHandler(void)
{
    MsgStream log(m_msgSvc, getName());
    int what = getInt();
    if (m_permDisable) {
        return;
    }
    if (what == 2) {
        log << MSG::INFO << "Overflow buffer permanently disabled"  << endmsg;
        m_permDisable = true;
        m_mepRxObj->m_overflow = false;
    }
    m_mepRxObj->m_overflow = (what == 1);
    log << MSG::INFO << (m_mepRxObj->m_overflow ? "en" : "dis") << "abled overflow buffer" << endmsg;
}

void UpMonCommand::commandHandler(void)
{
    MsgStream log(m_msgSvc, getName());
    log << MSG::INFO << "Received command, updating counters and histograms"
        << endmsg;
    SmartIF<IUpdateableIF> upda(m_mepRxObj->getMonSvc());
    if (upda) {
        upda->update(0).ignore();
    }
}

void ClearMonCommand::commandHandler(void)
{
    MsgStream log(m_msgSvc, getName());
    log << MSG::INFO << "Received command, clearing counters and histograms."
        << endmsg;
    m_mepRxObj->clearCounters();
}

void MEPRQCommand::commandHandler(void)
{
    MsgStream log(m_msgSvc, getName());
    int numMEPs = getInt();
    log << MSG::INFO << "Received command, sending " << numMEPs
        << " MEP requests.";
    if (m_mepRxObj->sendMEPReq(numMEPs).isSuccess())
        log << MSG::INFO << "OK." << endmsg;
    else log << MSG::INFO << "FAILED." << endmsg;
}

void OverflowStatSvc::serviceHandler()
{
    if (!m_mepRxObj->m_overflow) {
        setData(const_cast<char *> ("Overflow disabled"));
        return;
    }
    if (m_mepRxObj->m_overflowActive) {
        setData(const_cast<char *> ("Overflow active"));
        return;
    }
    setData(const_cast<char *> ("Overflow enabled"));
}

// Standard Constructor
MEPRxSvc::MEPRxSvc(const std::string& nam, ISvcLocator* svc) :
    Service(nam, svc), m_ebState(NOT_READY), m_incidentSvc(0)
{
    declareProperty("LocalTest", m_LocalTest = false);
    declareProperty("DestTestPort", m_DestTestPort = 45199);
    declareProperty("SrcTestPort", m_SrcTestPort = 45198);
    declareProperty("MEPBuffers", m_MEPBuffers = 2);
    declareProperty("ethInterface", m_ethInterface = -1);
    declareProperty("IPNameOdin", m_IPNameOdin = "");
    declareProperty("maxMsForGetSpace", m_maxMsForGetSpace = 100);
    declareProperty("IPSrc", m_IPSrc);
    declareProperty("IPProtoIn", m_IPProtoIn = IP_PROTO_HLT);
    declareProperty("sockBuf", m_sockBuf = 0x100000);
    declareProperty("partitionID", m_partitionID);
    declareProperty("partitionName", m_partitionName);
    declareProperty("partitionBuffers", m_partitionBuffers = false);
    declareProperty("refCount", m_refCount = 2);
    declareProperty("MEPBufSize", m_MEPBufSize = 0x400000); // 4 MB
    declareProperty("ownAddress", m_ownAddress = 0xFFFFFFFF);
    declareProperty("RxIPAddr", m_rxIPAddr = "0.0.0.0");
    declareProperty("InitialMEPReqs", m_initialMEPReq = 1);
    declareProperty("MEPsPerMEPReq", m_MEPsPerMEPReq = 1);
    declareProperty("MEPRecvTimeout", m_MEPRecvTimeout = 100); // ms (!)
    declareProperty("maxEventAge", m_maxEventAge = 1000000); // us
    declareProperty("nCrh", m_nCrh = 10);
    declareProperty("createDAQErrorMEP", m_createDAQErrorMEP = false);
    declareProperty("createODINMEP", m_createODINMEP = false);
    declareProperty("resetCountersOnRunChange", m_resetCounterOnRunChange = true);
    declareProperty("alwaysSendMEPReq", m_alwaysSendMEPReq = true);
    declareProperty("overflowPath", m_overflowPath = "/localdisk/overflow");
    declareProperty("overflow", m_overflow = true);
    declareProperty("nameLiveBuf", m_nameLiveBuf = "Events");
    declareProperty("nameOverflowBuf", m_nameOverflowBuf = "Overflow");
    declareProperty("badDiskList", m_badDiskList = "/group/online/dataflow/brokendisks");
    m_trashCan = new u_int8_t[MAX_R_PACKET];
    m_expectOdin = false;
    m_overflowActive = false;

    m_runNumber = 0; // when this becomes a problem I have hopefully something more interesting to do...
    m_tLastAdded = 0; // us
    m_tLastRx = 0; // us
}

// Standard Destructor
MEPRxSvc::~MEPRxSvc()
{
    delete[] (u_int8_t*) m_trashCan;

}

void MEPRxSvc::truncatedPkt(RTL::IPHeader *iphdr)
{
    MsgStream log(msgSvc(), "MEPRx");
    std::string hname, mesg;
    log << MSG::ERROR << "received packet with truncated MEP-header from "
        << (MEPRxSys::name_from_addr(iphdr->saddr, hname, mesg) ? mesg : hname)
        << endmsg;
}

void MEPRxSvc::removePkt()
{
    u_int64_t tsc, rxtim;
    std::string errstr;
    int len = MEPRxSys::recv_msg(m_dataSock, m_trashCan, MAX_R_PACKET, 0, &tsc,
                                 &rxtim, errstr);
    if (len < 0) {
        if (!MEPRxSys::rx_would_block()) {
            MsgStream log(msgSvc(), "MEPRx");
            ERRMSG(log,"recvmsg");
        }
    }
}

// age all workDsc and return the oldest
MEPRxSvc::RXIT MEPRxSvc::oldestRx()
{
    RXIT k = m_workDsc.begin(), j = m_workDsc.begin();
    for (; j != m_workDsc.end(); ++j) {
        if ((*j)->m_tFirstFrag < (*k)->m_tFirstFrag)
            k = j;
    }
    return k;
}

StatusCode MEPRxSvc::setupMEPReq(const std::string& odinName)
{
    MsgStream log(msgSvc(), "MEPRx");
    std::string msg;
    u_int32_t add;

    if (odinName.empty()) {
        m_dynamicMEPRequest = false;
        log << MSG::INFO << "No address for ODIN. Dynamic MEP requests disabled!"
            << endmsg;
        return StatusCode::SUCCESS;
    }
    if (MEPRxSys::parse_addr(odinName, add) && MEPRxSys::addr_from_name(odinName,
            add, msg)) {
        return error("Invalid address for ODIN: " + odinName + " " + msg);
    }
    m_odinIPAddr = add;
    m_dynamicMEPRequest = true;
    log << MSG::INFO << "Dynamic MEP requests will be sent to "
        << MEPRxSys::dotted_addr(m_odinIPAddr) << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::sendMEPReq(int m)
{
    static u_int32_t seqno = 0;
    if (!m_overflow && (m != 0xff)) return StatusCode::SUCCESS;
    if (m_dynamicMEPRequest) {
        mepreq.nmep = m;
        mepreq.seqno = seqno++;
        mepreq.runno = m_runNumber;
        clock_gettime(CLOCK_REALTIME, &mepreq.txtime);
        int n = MEPRxSys::send_msg(m_mepSock, m_odinIPAddr, MEP_REQ_TOS, &mepreq,
                                   MEP_REQ_LEN, 0);
        if (n == MEP_REQ_LEN) {
            m_totMEPReq += m;
            m_totMEPReqPkt++;
            return StatusCode::SUCCESS;
        }
        if (n == -1) {
            MsgStream log(msgSvc(), "MEPRx");
            ERRMSG(log,"send MEP request");
            return StatusCode::FAILURE;
        }
        return error("MEPRequest corrupted on send!");
    }
    return StatusCode::SUCCESS;
}

void MEPRxSvc::freeRx()
{
    if (!m_usedDsc.empty()) {
        MEPRx *rx = *(--m_usedDsc.end());
        m_usedDsc.pop_back();
        int rc = rx->spaceRearm(0);
        if (rc == MBM_NORMAL) {
            if (rx->m_wasIncomplete) {
                rx->updateNoshow();
                rx->m_wasIncomplete = false;
                // send mep requests only if the incomplete event contains the odin mep or
                // we are instructed to always re-charge
                if (!(rx->m_daqError & MissingOdin) || m_alwaysSendMEPReq) {
                    sendMEPReq(m_MEPsPerMEPReq);
                }
            } else {
                resetCounters(m_noShow, m_nSrc);
                sendMEPReq(m_MEPsPerMEPReq);
            }
            m_freeDsc.push_back(rx);
        } else if (rc != MBM_REQ_CANCEL) {
            error("timeout on getting space.");
        }
    }
}

void MEPRxSvc::forceEvent(RXIT dsc, ForceReason reason)
{
    MsgStream log(msgSvc(), "MEPRx"); // message stream is NOT thread-safe
    MSG::Level sev = m_partitionName == "LHCb" ? MSG::INFO : MSG::ERROR;
    switch (reason) {
    case TIME_OUT:
        log << sev << "Time-out for event " << (*dsc)->m_l0ID
            << " started at " << (*dsc)->m_tFirstFrag - m_tzero
            << " last fragment received " << m_tLastRx - m_tzero << " age "
            << (m_tLastRx - (*dsc)->m_tFirstFrag) << " us > limit" << endmsg;
        break;
    case NO_BUFFER:
        log << sev << "Flush oldest event " << (*dsc)->m_l0ID
            << " started at " << (*dsc)->m_tFirstFrag - m_tzero
            << " last fragment received " << m_tLastRx - m_tzero << " age "
            << (m_tLastRx - (*dsc)->m_tFirstFrag) << " us " << endmsg;
        break;
    case END_OF_RUN:
        log << MSG::INFO << "Flush remaining events because of end of run"
            << endmsg;
        break;
    }
    (*dsc)->spaceAction();
    m_usedDsc.push_back(*dsc);
    m_workDsc.erase(dsc);
}

// IRunable implementation: Run the object
StatusCode MEPRxSvc::run()
{
    MsgStream log(msgSvc(), "MEPRx"); // message stream is NOT thread-safe

    RXIT rxit;
    MEPRx *rx;
    u_int8_t hdr[HDR_LEN];
    RTL::IPHeader *iphdr = (RTL::IPHeader *) hdr;
    MEPHdr *mephdr = (MEPHdr*) &hdr[IP_HEADER_LEN];
    ;
    int srcid;

    // we are ready - wait for start
    while (m_ebState != RUNNING) {
        switch (m_ebState) {
        case STOPPED:
        case NOT_READY:
            log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
            return StatusCode::SUCCESS;
        case READY:
            MEPRxSys::microsleep(100000); // 100 ms
            break;
        default:
            continue;
        }
    }
    // we got a START command - send a clear-command first
    if (!sendMEPReq(0xff).isSuccess()) {
  log << MSG::WARNING << "Could not send MEP clear command" <<  endmsg;
    }
    MEPRxSys::microsleep(rand() | 0xffff); // back-off a bit
    // now send initial MEP requests
    if (!sendMEPReq(m_initialMEPReq).isSuccess()) {
        log << MSG::WARNING << "Could not send " << m_initialMEPReq
            << " initial MEP requests." << endmsg;
    }
    for (;;) {
        int n = 0;
        if (m_ebState == RUNNING) {
            checkTimeOut();
            n = MEPRxSys::rx_select(m_dataSock, m_MEPRecvTimeout);
        }
        if (n == -1) {
            ERRMSG(log, "select");
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
                SmartIF<IUpdateableIF> upda(m_monSvc);
                if (upda) {
                    upda->update(0).ignore();
                }
                while (!(m_workDsc.empty())) {
                    forceEvent(m_workDsc.begin(), END_OF_RUN);
                }
                log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
                return StatusCode::SUCCESS;
            }
            // we had a timeout - all events go up by the select timeoutvalue (which is in seconds!)
            //for (RXIT w = m_workDsc.begin(); w != m_workDsc.end(); ++w)
            //  (*w)->m_age += 1000000 * m_MEPRecvTimeout;
            m_tLastRx += 1000 * m_MEPRecvTimeout;
            checkTimeOut();
            if (--ncrh == 0) {
                log << MSG::DEBUG << "crhhh..." << m_freeDsc.size()
                    << " free buffers. ";
                log << MSG::DEBUG << endmsg;
                for (size_t i = 0; i < m_workDsc.size(); ++i) {
                    log << MSG::DEBUG << "Run # " << m_workDsc[i]->m_runNumber
                        << " - Event L0ID# " << m_workDsc[i]->m_l0ID << " is missing ";
                    log << MSG::DEBUG << printnum(m_workDsc[i]->m_nSrc -
                                                  m_workDsc[i]->m_nrx," MEP");
                }
                log << endmsg;
                ncrh = m_nCrh;
            }
            continue;
        }
        u_int64_t tsc, _tmp;
        std::string errstr;
        int len = MEPRxSys::recv_msg(m_dataSock, hdr, HDR_LEN, MEPRX_PEEK, &tsc,
                                     &_tmp, errstr);
        if (m_tzero == 0)
            m_tzero = _tmp;
        if (_tmp > m_tLastRx)
            m_tLastRx = _tmp;
        if (len < 0) {
            if (!MEPRxSys::rx_would_block())
                ERRMSG(log,"recvmsg");
            continue;
        }
        if (m_tLastRx == 0)
            ERRMSG(log, errstr);

        m_totRxPkt++;
        // Don't have sourceID yet, count per source later...
        m_numMEPRecvTimeouts = 0;
        if (len < (int) HDR_LEN) {
            // somebody sends us a badly truncated packet
            truncatedPkt(iphdr);
            removePkt();
            continue;
        }
        if ((srcid = getSrcID(iphdr->saddr)) == -1) {
            // we do not expect nor want this
            removePkt();
            continue;
        }
        if (!m_workDsc.empty() && mephdr->m_l0ID == m_workDsc.back()->m_l0ID) {
            rxit = --m_workDsc.end();
        } else {
            rxit = lower_bound(m_workDsc.begin(), m_workDsc.end(), mephdr->m_l0ID,
                               MEPRx::cmpL0ID);
            if (rxit == m_workDsc.end() || (*rxit)->m_l0ID != mephdr->m_l0ID) {

                // not found - get a new descriptor
                RXIT oldest = oldestRx();
                // how long since we completed the last event?
                m_idleTimeSock->fill(double(m_tLastRx - m_tLastComp), 1.0);
                try {
                    if (m_freeDsc.empty()) {
                        forceEvent(oldest, NO_BUFFER);
                        freeRx(); // only if not in separate thread
                    }
                    rx = m_freeDsc.back();
                    m_freeDsc.pop_back();
                    rx->m_l0ID = mephdr->m_l0ID;
                    RXIT j = lower_bound(m_workDsc.begin(), m_workDsc.end(),
                                         mephdr->m_l0ID, MEPRx::cmpL0ID);
                    m_workDsc.insert(j, rx);
                    rxit = lower_bound(m_workDsc.begin(), m_workDsc.end(),
                                       mephdr->m_l0ID, MEPRx::cmpL0ID);
                } catch (std::exception& e) {
                    error(std::string("Exception ") + e.what());
                }
            }
        }
        m_rxPkt[srcid]++; // recieved packets per source
        int rc = (*rxit)->addMEP(m_dataSock, mephdr, srcid, tsc, m_tLastRx);
        if ((*rxit)->m_runNumber > m_runNumber) {
            if (m_runNumber != 0)
            {
              int runo;
              runo = int(m_runNumber);
              log << MSG::INFO << "run-change detected - updating EOR services for run# " << m_runNumber
                  << endmsg;
              m_monSvc->updateSvc("this",runo,this);
              m_monSvc->resetHistos(0);
            }
            m_runNumber = (*rxit)->m_runNumber;
            log << MSG::DEBUG << "run-change detected - new run# " << m_runNumber
                << endmsg;
            if (m_resetCounterOnRunChange)
                clearCounters();
        }
        else if ((*rxit)->m_runNumber < m_runNumber)
        {
          log << MSG::INFO << "Received Run Number (" << (*rxit)->m_runNumber
              << ") inferior to current Run number (" << m_runNumber << ")"
              << endmsg;
        }
        if (rc == MEP_SENT) {
            rx = *rxit;
            m_workDsc.erase(rxit);
            m_usedDsc.push_back(rx);
            freeRx();
        }
    } // for{;;}
    return 1; // never reached
}

// IInterface implementation: Query interface
StatusCode MEPRxSvc::queryInterface(const InterfaceID& riid,
                                    void** ppvInterface)
{
    if (IRunable::interfaceID().versionMatch(riid)) {
        *ppvInterface = (IRunable*) this;
    } else if (IIncidentListener::interfaceID().versionMatch(riid)) {
        *ppvInterface = (IIncidentListener*) this;
    } else {
        // Interface is not directly availible: try a base class
        return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::error(const std::string& msg)
{
    MsgStream log(msgSvc(), "MEPRx");
    log << MSG::ERROR << msg << endmsg;
    return StatusCode::FAILURE;
}

StatusCode MEPRxSvc::info(const std::string& msg)
{
    MsgStream log(msgSvc(), "MEPRx");
    log << MSG::INFO << msg << endmsg;
    return StatusCode::SUCCESS;
}

void MEPRxSvc::srcSwap(int i, int j)
{
    string name, addr;
    int flags;
    name = m_srcName[i];
    m_srcName[i] = m_srcName[j];
    m_srcName[j] = name;
    addr = m_srcDottedAddr[i];
    m_srcDottedAddr[i] = m_srcDottedAddr[j];
    m_srcDottedAddr[j] = addr;
    flags = m_srcFlags[i];
    m_srcFlags[i] = m_srcFlags[j];
    m_srcFlags[j] = flags;
}

int MEPRxSvc::srcFindMedianIndex(int left, int right, int shift)
{
    int i, groups = (right - left) / shift + 1, k = left + groups / 2 * shift;
    for (i = left; i <= k; i += shift) {
        int minIndex = i;
        string minValue = m_srcName[minIndex];
        int j;
        for (j = i; j <= right; j += shift)
            if (m_srcName[j] < minValue) {
                minIndex = j;
                minValue = m_srcName[minIndex];
            }
        srcSwap(i, minIndex);
    }
    return k;
}

int MEPRxSvc::srcFindMedianOfMedians(int left, int right)
{
    if (left == right)
        return left;
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

int MEPRxSvc::srcPart(int left, int right)
{
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

void MEPRxSvc::srcSort(int left, int right)
{
    if (m_srcName[left] >= m_srcName[right])
        return;
    int index = srcPart(left, right);
    srcSort(left, index - 1);
    srcSort(index + 1, right);
}

StatusCode MEPRxSvc::checkProperties()
{
    std::ostringstream log;
    if (m_ethInterface < -1)
        return error("ethDevIn must be >= 0 (or -1 if rxIPAddr is set)");
    else if (m_maxMsForGetSpace < 1)
        return error("maxMsForGetSpace must be > 0");
    else if (m_MEPBuffers < 1 || m_MEPBuffers > 256)
        return error("MEPBuffers must be > 0 and < 256");
    else if (m_sockBuf < 0x10000 || m_sockBuf > 0x4000000)
        return error("sockBuf must be >= 64 kB and less than 64 MB");
    else if (m_IPProtoIn < 0 || m_IPProtoIn > 255)
        return error("IPProtoIn is an unsigned 8 bit quantity");
    else if ((m_IPSrc.size() % 3) != 0)
        return error("Malformed source list (length is odd)");
    else if (m_MEPBufSize < 0) {
        log << "Illegal value for MEPBufSize " << m_MEPBufSize;
        return error(log.str());
    } else if (!setupMEPReq(m_IPNameOdin).isSuccess()) {
        return error("Bad address IPNameOdin " + m_IPNameOdin);
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
        } else {
            char tmp[16];
            if (m_IPSrc[i + 1].size() == 0) {
                if (MEPRxSys::name_from_addr(addr, nam, msg))
                    nam = snprintf(tmp, 15, "src-%d", i / 3);
            } else {
                nam = m_IPSrc[i + 1];
            }
        }

        m_srcFlags.push_back(0);
        if (m_IPSrc[i + 2] == "DOUBLE_ZERO_BUG")
            m_srcFlags[i / 3] |= DOUBLE_ZERO_BUG;
        if (m_IPSrc[i + 2] == "ODIN") {
            m_srcFlags[i / 3] |= IS_ODIN;
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
    if (m_maxEventAge != 0) {
        if (m_maxEventAge < 1000 * m_MEPRecvTimeout)
            m_MEPRecvTimeout = m_maxEventAge / 1000;
    }
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::allocRx()
{
    char num[32];
    MsgStream log(msgSvc(), "MEPRx");
    for (int i = 0; i < m_MEPBuffers; ++i) {
        ::sprintf(num, "MEPRx%03d", i);
        MEPRx *rx = new MEPRx(num, this);
        if (rx->spaceRearm(0) != MBM_NORMAL) {
            return StatusCode::FAILURE;
        }
        m_freeDsc.push_back(rx);
    }
    m_workDsc.clear();
    log << MSG::DEBUG << std::dec << printnum(m_MEPBuffers, " receive-buffer")
        << " created." << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::releaseRx()
{
    std::for_each(m_freeDsc.begin(), m_freeDsc.end(), MEPRx::release);
    m_freeDsc.clear();
    std::for_each(m_usedDsc.begin(), m_usedDsc.end(), MEPRx::release);
    m_usedDsc.clear();
    std::for_each(m_workDsc.begin(), m_workDsc.end(), MEPRx::release);
    m_workDsc.clear();
    return StatusCode::SUCCESS;
}

int MEPRxSvc::openSocket(int protocol)
{
    std::string msg;
    int retSock;

    if (m_LocalTest) {
        retSock = MEPRxSys::open_sock_udp(msg, m_DestTestPort);
    } else {
        retSock = MEPRxSys::open_sock(protocol, m_sockBuf, m_ethInterface,
                                      m_rxIPAddr, m_dynamicMEPRequest, msg);
    }
    if (retSock < 0) {
        MsgStream log(msgSvc(), "MEPRx");
        ERRMSG(log,msg);
    }
    return retSock;
}

int MEPRxSvc::getSrcID(u_int32_t addr)
{
    std::map<u_int32_t, int>::iterator i = m_srcAddr.find(addr);
    if (i == m_srcAddr.end()) {
        std::string hname, mesg;
        error("received unexpected packet from " + (MEPRxSys::name_from_addr(addr,
                hname, mesg) ? mesg : hname));
        m_notReqPkt++;
        return -1;
    }
    return i->second;
}

void MEPRxSvc::checkTimeOut()
{
    if (m_maxEventAge == 0 || m_tLastAdded == 0)
        return;
ageloop:
    for (RXIT w = m_workDsc.begin(); w != m_workDsc.end(); ++w) {
        if ((m_tLastRx - (*w)->m_tFirstFrag) > m_maxEventAge) {
            forceEvent(w, TIME_OUT);
            freeRx(); // only if not in separate thread
            goto ageloop;
        }
    }
}

void MEPRxSvc::publishHists()
{
    m_monSvc->declareInfo("MEPcomplTime", m_complTimeSock,
                          "dt(last - first MEP for given L0ID) us", this);
    m_monSvc->declareInfo("InterMEPTime", m_idleTimeSock,
                          "dt(prev MEP - current MEP) us", this);
    m_monSvc->declareInfo("L0IDDiff", m_L0IDDiff,
                          "L0ID difference between 2 consecutive events", this);
}

void MEPRxSvc::publishCounters()
{
    PUB64CNT(totRxOct, "Total received bytes");
    PUB64CNT(totRxPkt, "Total received packets");
    PUB64CNT(totRxEvt, "Total received events");
    PUB64CNT(totRxMEP, "Total received MEPs");
    PUB64CNT(incEvt, "Incomplete events");
    PUB64CNT(totBadMEP, "Total bad MEPs");
    PUB64CNT(totMEPReq, "Total requested MEPs");
    PUB64CNT(totMEPReqPkt, "Total Sent MEP-request packets");
    PUB64CNT(numMEPRecvTimeouts, "MEP-receive Timeouts");
    PUB64CNT(notReqPkt, "Total unsolicited packets");
    PUBCNT(totMEPproduced, "Total produced complete MEPs");
    PUBCNT(totMEPproducedOvfl, "Total produced complete MEPs into Overflow buf");
    PUBCNT(totMEPproducedLive, "Total produced complete MEPs into MEP buf");
    PUBARRAYCNT(badLenPkt, "MEPs with mismatched length");
    PUBARRAYCNT(misPkt, "Missing MEPs");
    PUBARRAYCNT(badPckFktPkt, "MEPs with wrong packing (MEP) factor");
    PUBARRAYCNT(truncPkt, "Truncated (empty) MEPs");
    PUBARRAYCNT(multipleEvt, "Duplicate Events");
    PUBARRAYCNT(rxOct, "Received bytes");
    PUBARRAYCNT(rxPkt, "Received packets");
    PUBARRAYCNT(rxEvt, "Received events");
    PUBARRAYCNT(rxMEP, "Received MEPs");
}

void MEPRxSvc::clearCounters()
{
    resetCounters(m_rxOct, m_nSrc);
    resetCounters(m_rxPkt, m_nSrc);
    resetCounters(m_rxEvt, m_nSrc);
    resetCounters(m_rxMEP, m_nSrc);
    resetCounters(m_badPckFktPkt, m_nSrc);
    resetCounters(m_badLenPkt, m_nSrc);
    resetCounters(m_misPkt, m_nSrc);
    resetCounters(m_truncPkt, m_nSrc);
    resetCounters(m_multipleEvt, m_nSrc);
    resetCounters(m_noShow, m_nSrc);
    m_totMEPReq = 0;
    m_totMEPReqPkt = 0;
    m_totMEPproduced = 0;
    m_totMEPproducedOvfl = 0;
    m_totMEPproducedLive = 0;
    m_numMEPRecvTimeouts = 0;
    m_notReqPkt = 0;
    m_totRxOct = 0;
    m_totRxPkt = 0;
    m_totRxEvt = 0;
    m_totRxMEP = 0;
    m_incEvt = 0;
    m_totMEPReqPkt = 0;
    m_numMEPRecvTimeouts = 0;
    m_totMEPReq = 0;
    m_totBadMEP = 0;
    m_complTimeSock->reset();
    m_idleTimeSock->reset();
    m_L0IDDiff->reset();
}

int MEPRxSvc::setupCounters()
{
    MsgStream log(msgSvc(), "MEPRx");

    m_complTimeSock = m_histSvc->book("complTimeSock",
                                      "dt(last - first MEP for given L0ID) us", 500, 0., 100000.);
    m_idleTimeSock = m_histSvc->book("idleTimeSock",
                                     "dt(prev MEP current MEP) us", 100, 0., 1000000);
    m_L0IDDiff = m_histSvc->book("L0IDDiff",
                                 "L0ID difference between 2 consecutive events", 50, -30000., 30000.);

    publishHists();
    if (m_complTimeSock == NULL || m_idleTimeSock == NULL) {
        log << MSG::ERROR << "hist pointers were not booked !!!" << endmsg;
        // if(m_complTimeTSC == NULL) log << MSG::ERROR << "complTimeTSC" << endmsg;
        if (m_complTimeSock == NULL)
            log << MSG::ERROR << "complTimeSock" << endmsg;
        // if(m_idleTimeTSC == NULL) log << MSG::ERROR << "idleTimeTSC" << endmsg;
        if (m_idleTimeSock == NULL)
            log << MSG::ERROR << "idleTTimeSock" << endmsg;
        if (m_L0IDDiff == NULL)
            log << MSG::ERROR << "L0IDDiff" << endmsg;
        return 1;
    }
    clearCounters();
    publishCounters();
    // create cstring for source names
    std::string all_names = "";
    if (m_srcName.size() > 0)
        all_names = m_srcName[0];
    for (unsigned i = 1; i < m_srcName.size(); ++i)
        all_names = all_names + '\0' + m_srcName[i];
    if (!(m_allNames = new char[all_names.size() + 1]))
        return 1;
    ::memcpy(m_allNames, (const char *) all_names.data(), all_names.size() + 1);
    m_monSvc->declareInfo("srcName", "C", m_allNames, all_names.size() + 1,
                          "Source IP names", this);
    log << MSG::INFO << all_names << all_names.size() << endmsg;
    return 0;
}

void MEPRxSvc::handle(const Incident& inc)
{
    MsgStream log(msgSvc(), "MEPRx");
    log << MSG::INFO << "Got incident:" << inc.source() << " of type "
        << inc.type() << endmsg;
    if (inc.type() == "DAQ_CANCEL") {
        m_ebState = STOPPED;
        std::for_each(m_freeDsc.begin(), m_freeDsc.end(), MEPRx::cancelReq);
        std::for_each(m_usedDsc.begin(), m_usedDsc.end(), MEPRx::cancelReq);
        std::for_each(m_workDsc.begin(), m_workDsc.end(), MEPRx::cancelReq);
    } else if (inc.type() == "DAQ_ENABLE") {
        m_ebState = RUNNING;
    }
}
StatusCode MEPRxSvc::stop()
{
   StatusCode sc;
   if (m_monSvc)
   {
     MsgStream log(msgSvc(), "MEPRx"); // message stream is NOT thread-safe
     log << MSG::INFO << "executing Stop - updating EOR services for run# " << m_runNumber
         << endmsg;
     int runo;
     runo = int(m_runNumber);
     m_monSvc->updateSvc("this",runo,this);
     m_monSvc->resetHistos(0);
   }
   sc = Service::stop();
   return sc;
}
StatusCode MEPRxSvc::initialize()
{
    StatusCode sc = Service::initialize();
    if (!sc.isSuccess())
        return error("Failed to initialize service base class.");
    else if (!checkProperties().isSuccess())
        return error("Failed to check properties.");
    else if (!m_LocalTest && ((m_mepSock = openSocket(MEP_REQ_TOS)) < 0))
        return error("Failed to open socket: MEP_REQ_TOS");
    else if ((m_dataSock = openSocket(m_IPProtoIn)) < 0)
        return error("Failed to open socket: IPProtoIn");
    else if (!allocRx().isSuccess())
        return error("Failed to allocate RX");
    else if (!service("IncidentSvc", m_incidentSvc).isSuccess())
        return error("Failed to access incident service.");
    else if (!service("HistogramSvc", m_histSvc).isSuccess())
        return error("Failed to access histogram service.");
    else {
        m_incidentSvc->addListener(this, "DAQ_CANCEL");
        m_incidentSvc->addListener(this, "DAQ_ENABLE");
    }

    m_mepRQCommand = new MEPRQCommand(this, msgSvc(), RTL::processName());
    m_clearMonCommand = new ClearMonCommand(this, msgSvc(), RTL::processName());
    m_upMonCommand = new UpMonCommand(this, msgSvc(), RTL::processName());
    m_setOverflowCmd = new SetOverflowCmd(this, msgSvc(), RTL::processName());
    m_overflow = true;
    if (service("MonitorSvc", m_monSvc).isSuccess()) {
        if (setupCounters()) {
            return error("Failed to setup counters");
        }
    } else {
        return error("Failed to access monitor service.");
    }
#if 0 // obsolete verification of existence and write-ability of /localdisk/overflow
    if (lib_rtl_access(m_overflowPath.c_str(), 0x6) != 1) {
        info(std::string("Cannot write to ") + m_overflowPath + std::string(
                 " - disabling overflow"));
        m_overflow = false;
        m_setOverflowCmd->m_permDisable = true;
    }
#endif
    std::string hostname;
    MEPRxSys::gethostname_short(hostname); // get lowercased short hostname
    std::string errmsg;
    if (MEPRxSys::is_in_file_no_case(hostname, m_badDiskList, errmsg)) {
  info(std::string("Disk disabled on ") + hostname + std::string(" - disabling overflow"));
  m_overflow = false;
  m_setOverflowCmd->m_permDisable = true;
    } else {
  if (errmsg.compare("") != 0) {
      error(errmsg);
  }
    }

    m_overflowStatSvc = new OverflowStatSvc(this, msgSvc(), RTL::processName());
    m_ebState = READY;
    return StatusCode::SUCCESS;
}

StatusCode MEPRxSvc::finalize()
{
    MsgStream log(msgSvc(), "MEPRx");
    log << MSG::INFO << "Entering finalize....." << endmsg;
    if (m_mepRQCommand) {
      delete m_mepRQCommand;
      m_mepRQCommand = 0;
    }
    if (m_upMonCommand) {
      delete m_upMonCommand;
      m_upMonCommand = 0;
    }
    if (m_clearMonCommand) {
      delete m_clearMonCommand;
      m_clearMonCommand = 0;
    }
    if (m_setOverflowCmd) {
        delete m_setOverflowCmd;
        m_setOverflowCmd = 0;
    }
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
    if (m_histSvc)
        m_histSvc->release();
    m_ebState = NOT_READY;
    if (m_overflowStatSvc)
        delete m_overflowStatSvc;
    return Service::finalize();
}
#if 0
// ;;; Local Variables: ***
// ;;; c-default-style:"linux" ***
// ;;; c-basic-offset:4 ***
// ;;; End: ***
#endif
