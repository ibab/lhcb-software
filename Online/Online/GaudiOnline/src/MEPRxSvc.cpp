#if 0
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
//	===========================================================
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <map>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiOnline/MEPRxSvc.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "RTL/rtl.h"
#include "MBM/MepProducer.h"

#define MAX_R_PACKET (0x10000 + 20)
#define MEP_SENT  MBM_NORMAL
#define MEP_ADDED (MEP_SENT + 1)
#define IP_HEADER_LEN 20
#define IP_PROTO_HLT  0xF2
#define HDR_LEN (IP_HEADER_LEN + sizeof(mep_hdr_t)) 

#define errmsg(x) {char errstr[64]; strerror_r(errno, errstr, 64); \
log << MSG::ERROR << x << errstr << " " << __FILE__ << ":" << __LINE__ << endmsg;}
#define printnum(n, s) n << s << (n == 1 ? "" : "s")
typedef struct iphdr ip_hdr_t;
typedef std::vector<MEP::MEPRx *>::iterator RXIT;


static struct LHCb::MEPRx: public MEP::Producer {
  int m_spaceSize, m_refCount, m_age;
  u_int32_t m_l0id, m_brx, m_nrx, m_nsrc;
  u_int16_t m_pf;
  int m_spaceRC;
  LHCb::MEPRxSvc *m_parent;
  MEPEVENT *m_e; 
  MsgStream m_log(msgSvc(), name());

  MEPRx(const std::string& nam, int partitionID, int refcnt, 
	size_t siz, LHCb::MEPRxSvc *parent, int nsrc) 
    : MEP::Producer(nam, partitionID), m_spaceSize(siz), m_refCount(refcnt),
      m_parent(parent), m_nSrc(nsrc);
    {
      m_flags = USE_MEP_BUFFER;
      include();
      m_bmid = m_mepID->mepBuffer;
      m_log << MSG::DEBUG << hex << showbase << "MEP buffer start: " 
	    << m_mepID->mepStart << "\nEVENT  buffer start: " 
	    << m_mepID->evtStart << "\nRESULT buffer start: " 
	    << m_mepID->resStart << endmsg;     
      m_log << MSG::DEBUG << "Buffer space: " << m_spaceSize << " bytes" << 
	endmsg;
      m_seen = new int[m_nSrc];
    }
  ~MEPRx()  {
    delete m_seen;
  }    
  int spaceTimeOut(void *dummy) {
    if (m_spaceRC != -100) return 0;
    mbm_cancel_request(m_bmid);
    return 1;
  }
  int spaceRearm(int) {
    int timerID;

    if (m_maxMsForGetSpace) {
      m_spaceRC = -100;
      if (lib_rtl_set_timer(m_maxMsForGetSpace, this->spaceTimeOut, NULL, 
			    &timerID)) {
	m_log << MSG::ERROR << "Could not initialise timer!" << endmsg;
	return -1;
      }
    }
    m_spaceRC = MEP::Producer::spaceRearm(m_spaceSize);
    if (m_spaceRC == MBM_NORMAL) lib_rtl_kill_timer(timerID);
    return m_spaceRC;
  }

  int spaceAction() {
    static  int id = -1;
    int status;
    
    m_e->evID        = ++id;
    m_event.len = m_brx + sizeof(MEPEVENT);
    m_event.mask[0] = partitionID();
    m_event.mask[1] = 0;
    m_event.mask[2] = 0;
    m_event.mask[3] = 0;
    m_event.type    = EVENT_TYPE_MEP;    
    if (m_nrx != m_nSrc) incompleteEvent();
    *((int *) m_e->data) = m_brx;
    if (!dry) { 
      declareEvent();
      status = sendSpace();
    }
    //info("bang %d %d bytes\n", m_e->evID, m_brx);
    m_brx = m_nrx = 0; memset(seen, 0, m_nSrc);
    return status;
  }
  // Run the application in synchonous mode
  int runSynchronous() {
    int status = spaceRearm(0);
    if ( status == MBM_NORMAL )  {
      status = spaceAction();
    }
    return status;
  }
  int addMEP(mep_hdr_t *hdr, int srcid) {
    int len = 0;
 
    if (seen[i]) multipleSrc();
    seen[i] = 1;		   
    /* here a lot of tests should be done */
    if (m_nrx == 0) {
      MBM::EventDesc& dsc = event();
      m_e = (MEPEVENT *)dsc.data;
      m_e->refCount    = m_refCount;
      m_e->evID        = hdr->l01_id;
      m_e->begin       = int(int(m_e)-m_mepID->mepStart);
      m_e->packing     = -1;
      m_e->valid       = 1;
      m_e->magic       = mep_magic_pattern();
      m_event.len = sizeof(MEPEVENT) - sizeof(m_e->data);
      /* m_e->data  ab da laenge und dann MEPs */	
      m_brx = 0;
      m_pf = hdr->number_of_events;
    }
    if (m_pf != hdr->number_of_events) {
      info("Packing factor %d differs from expected %d\n",
	   m_pf, hdr->number_of_events);
    }
    //info("event 0x%08x src %d\n", hdr->l01_id, m_nrx);
    struct iovec mep_recv_vec[1];
    mep_recv_vec[0].iov_base = (char *) m_e->data + m_brx + 4; 
    mep_recv_vec[0].iov_len = MAX_R_PACKET; 	
    struct msghdr mep_recv_msg; 
    mep_recv_msg.msg_name = 0;
    mep_recv_msg.msg_namelen = 0; // sizeof(struct sockaddr_in),
    mep_recv_msg.msg_iov = mep_recv_vec;
    mep_recv_msg.msg_iovlen = 1;
    mep_recv_msg.msg_flags = 0;
    mep_recv_msg.msg_control = NULL;
    mep_recv_msg.msg_controllen = 0;
    len = recvmsg(r, &mep_recv_msg, MSG_DONTWAIT);
    if (len < 0) {
      fatal("recvmsg");
    }
    m_brx += len;
    m_nrx++; 
    *(int*)m_e->data = m_brx;
//      info("\t\t\t nrx: %d", m_nrx);
//      if (m_nrx == m_nsrc) info("complete\n");
    return (m_nrx == m_nsrc) ? spaceAction() : MEP_ADDED;
  }  
  void multipleSrc() {
    
};

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPRxSvc)

// Standard Constructor
LHCb::MEPRxSvc::MEPRxSvc(const std::string& nasm, ISvcLocator* svc)
  : Service(nam, svc), m_handle(0), m_receiveEvents(false), m_incidentSvc(0)
{
  declareProperty("MEPBuffers", m_MEPBuffers);
  declareProperty("ethInterface", m_ethInterface);
  declareProperty("IPOdin", m_IPOdin);
  declareProperty("maxBadPktRatio", m_maxBadPktRatio);
  declareProperty("pktSamplingCount", m_pktSamplingCount);
  declareProperty("maxMsForGetSpace", m_maxMsForGetSpace);
  declareProperty("IPSrc", m_IPSrc);
  declareProperty("IPProtoIn", m_IPProtoIn = IP_PROTO_HLT);
  declareProperty("sockBuf", m_sockBuf = 0x1000000);
  declareProperty("partitionID", m_partitionID);
  declareProperty("refCount",m_refCount = 2);
  declareProperty("MEPBufSize", m_MEPBufSize = -1);
  declareProperty("bufName", m_bufName = "MEPRX");
  m_trashVec.iov_base = new u_int8_t[MAX_R_PKT];
  m_trashVec.iov_len = MAX_R_PKT;
}

// Standard Destructor
LHCb::MEPRxSvc::~MEPRxSvc(){
  delete(m_trashVec.iov_base);
}

int 
LHCb::MEPRxSvc::exec(void* arg)  {
  LHCb::MEPRxSvc* s = (LHCb::MEPRxSvc*)arg;
  return s->execute();
}

static inline bool 
LHCb::MEPRxSvc::cmpL0ID(MEPRx *r, u_int32_t id) {
  //info("cmpL0ID: m_l0ID %d id %d\n", r->m_l0id, id);
  return r->m_l0id < id;
}

inline void
LHCb::MEPRxSvc::removePkt()
{
  struct msghdr msg = { 0, 0, m_trashVec, 1, 0, NULL, 0 };
  int len = recvmsg(r, &msg, MSG_DONTWAIT);
  if (len < 0) {
    if (errno != EAGAIN) 
      errmsg("recvmsg");
  }
}

int 
LHCb::MEPRxSvc::execute() {
  MsgStream log(msgSvc(), name());
c  fd_set fds;
  struct iovec mep_recv_vec[1];
  RXIT rx;
  struct msghdr msg; 
  bool exe = true;
  int maxfd = m_r + 1
  FD_ZERO(fds);
  ip_hdr_t *iphdr = (ip_hdr_t *) hdr;
  mep_hdr_t *mephdr = (mep_hdr_t *) &hdr[IP_HEADER_LEN];
  mep_recv_vec->iov_base = hdr;
  mep_recv_vec->iov_len = HDR_LEN;

  for (;;) {
    int n;
    struct timeval timeout = {2, 0}; /* seconds */
    
    FD_SET(m_r, &fds);
    if ((n = select(maxfd, &fds, NULL, NULL, &timeout)) < 0) {
      errmsg("select");
      continue;
    }
    if (n == 0) {
      if (!m_receiveEvents) {
	forceEvents();
	return 0;
      } 
      log << MSG::DEBUG << "crhhh..." <<  workdsc.size() << endmsg;
      continue;
    }
    if (FD_ISSET(r, &fds)) {
//      echohdr(hdr);
      struct msghdr msg = { .msg_name = 0, .msg_namelen = 0, .msg_iov = mep_recv_vec; .msg_iovlen = 1, .msg_flags = 0, .msg_control = NULL, .msg_controllen = 0};
      int len = recvmsg(r, &msg, MSG_DONTWAIT | MSG_PEEK);
      if (len < 0) {
	if (errno != EAGAIN) 
	  errmsg("recvmsg");
	continue;
      }
    } 
    if ((srcid = getSrcID(iphdr->saddr)) == - 1) {
      /* we do not expect nor want this */
      removePkt();
      continue;
    }
    //::fprintf(stdout, "\rpktin %d ", ++pktin);
    //    ::fflush(stdout);
    if (!workdsc.empty() && mephdr->l01_id == workdsc.back()->m_l0id) {
      rx = --workdsc.end();
    } else {
      rx = lower_bound(workdsc.begin(), workdsc.end(), mephdr->l01_id, 
		       cmpL0ID);
      if (rx == workdsc.end() || (*rx)->m_l0id != mephdr->l01_id) {
	/* not found */
	RXIT k, j;
	for (j = workdsc.begin(); j != workdsc.end(); ++j) 
	  if (--((*j)->m_age) <= 0) {
	    k = j;
	    (*j)->m_age = 0;
	  }
	try {
	  if (freedsc.empty()) {
	    (*k)->spaceAction();
	    freedsc.push_back(*k);
	    workdsc.erase(k);
	  }
	  j = lower_bound(workdsc.begin(), workdsc.end(), mephdr->l01_id, 
			  MEP::cmpL0ID);
	  rx = --freedsc.end(); 
	  freedsc.pop_back();
	  if ((*rx)->spaceRearm(0) != MBM_NORMAL) {
	    fatal("spaceRearm(0)");
	  }
	  workdsc.insert(j, *rx);
	  (*rx)->m_age = nmax;
	  (*rx)->m_l0id = mephdr->l01_id;
	}
	catch(std::exception& e) {
	  log << MSG:ERROR << "Exeption " << e.what() << endmsg;
	}
      } 
    }
    if ((*rx)->addMEP(mephdr, srcid) == MEP_SENT) {
      m_workDsc.erase(rx);
      m_freeDsc.push_back(*rx);
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
LHCb::MEPRxSvc::parseAddr(const std::string &straddr, u_int32_t &addr) {
  struct in_addr a;

  int rc = inet_aton(straddr.c_string(), &a);
  addr = a.s_addr;
  return rc;
}
int 
LHCb::MEPRxSvc::addrFromName(const std::string &name, u_int32_t &addr) {
  struct hostent h;

  if (!(h = gethostbyname(name.c_string()))) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << name << " " << hstrerror(h_errno) << endmsg;
    return 1;
  }
  addr = *((u_int32_t *) h.h_addr_list[0]);    
  return 0;
}
int 
LHCb::MEPRxSvc::nameFromAddr(u_int32_t addr, std::string &name) {
  struct hostent h;

  if (!(h = gethostbyaddr(&addr, 4, AF_INET))) {
    MsgStream log(msgSvc(), name());
    struct in_addr in = { .s_addr = addr };
    
   log << MSG::ERROR << inet_ntoa(in) << " " << hstrerror(h_errno) << endmsg;
    return 1;
  } 
  name = h.h_name;
  return 0;
}
int 
LHCb::MEPRxSvc::checkProperties() {
  MsgStream log(msgSvc(),name());
   
  if (m_ethDevIn < 0) { 
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
  if (m_sockBuf < 0x10000 || m_sockBuf > 0x40000000) {
    log << MSG::ERROR << "sockBuf must be >= 64 kB and less than 1 GB" << endmsg;
    return 1;
  }
  if (m_IPProtoIn < 0 || m_IPProtoIn > 255) {
    log << MSG::ERROR << "IPProtoIn is an unsigned 8 bit quantity" << endmsg;
    return 1;
  }
  if (parse_addr(m_IPOdin, m_iprs)) {
    log << MSG::ERROR << "Bad address IPOdin " << m_IPOdin << endmsg;
    return 1;
  }
  if ((m_IPSrc.size() % 2) != 0) {
    log << MSG::ERROR << "Malformed source list (length is odd)" << endmsg;
    return 1;
  }
  for (int i = 0; i < m_IPSrc.size(); i += 2) {
    u_int32_t addr; std::string name;

    if (parseAddr(m_IPSrc[i], addr)) {
      if (addrFromName(m_IPSrc[i + 1], addr)) {
	log << MSG::ERROR << "No correct address for source " << i << endmsg;
	return 1;
      }
      name = m_IPSrc[i + 1];
    } else {
      char tmp[16];
      if (m_IPSrc[i + 1].size() == 0) {
	if (nameFromAddr(addr, name)) name = snprintf(tmp, 15, "src-%d", 
							 i / 2);
      }
    }
    m_srcAddr[addr] = i / 2; m_srcName.push_back(name);
  }
  m_nSrc = m_IPSrc.size() / 2;
  return 0;
}

int LHCb::MEPRxSvc::allocRx() {
  MsgStream log(msgSvc(),name());

  for (int i = 0; i < m_MEPBuffers; ++i) {
    LHCb::MEPRx *rx = new LHCb::MEPRx(name + char(i+'0'), m_partitionID, 
				      m_refCount, m_MEPBufSize);
    rx->setNonBlocking(WT_FACILITY_DAQ_SPACE, false);
    m_freeDsc.push_back(rx);
    rx->m_nSrc = m_nSrc;
  }
  m_workDsc.clear();
  log   << MSG::DEBUG << printnum(m_MEPBuffers, "receive-buffer") << 
    "created" << endmsg;  
  return 0;
}
	
int LHCb::MEPRxSvc::openSock() {
  char netdev_name[10];
  MsgStream log(msgSvc(),name());
  uid_t uid;

  /* get root privs - needed until SYS_CAP_RAW available */
  uid = getuid();
  if (setuid(0)) {
    errmsg("setuid");
    return 1;
  }
  if ((m_r = socket(PF_INET, SOCK_RAW, m_IPProtoIn)) < 0) {
    errmsg("socket");
    goto drop_out;
  }
  if (setsockopt(m_r, SOL_SOCKET, SO_RCVBUF, (void *)
		 &m_sockBuf, sizeof(m_sockBuf))) {
    errmsg("setsockopt SO_RCVBUF");
    goto shut_out;
  }
  sprintf(netdev_name, m_ethDevIn < 0 ? "lo" : "eth%d", m_ethDevIn);           
  if (setsockopt(m_r, SOL_SOCKET, SO_BINDTODEVICE, (void *) netdev_name,
		 1 + strlen(netdev_name))) {
    errmsg("setsockopt SO_BINDTODEVICE");
    goto shut_out;
  }
  log << MSG::INFO << "listening on " << netdev_name << "for IP #" << 
    m_IPProtoIn <<  " socket buffer is " << m_sockBuf / 0x400 << 
    " kB" << endmsg;
  setuid(uid);
  return 0;
shut_out:
  shutdown(m_r);
drop_out:
  setuid(uid);
  return 1;
}

inline int
LHCb::MEPRxSvc::getSrcID(u_int32_t addr)
{
 std::map<u_int32_t, int>::iterator i;

  if ((i = m_srcAddr.find(addr)) == m_srcAddr.end()) {
    struct in_addr in = { .s_addr = addr };
    log << MSG::ERROR << "received unexpected packet from " << inet_ntoa(&in) \
	<< endmsg;
    m_notReqPkt++;
    return -1;
  }
  return *i;
}
	
void
LHCb::MEPRxSvc::publishCounters()
{
}
       
void
LHCb::MEPRxSvc::clearCounters(int n)
{
  memset(m_rxOct,  0, n * sizeof(u_int64_t));
  memset(m_rxPkt,  0, n * sizeof(u_int64_t));
  memset(m_badPkt, 0, n * sizeof(u_int32_t));
  memset(m_misPkt, 0, n * sizeof(u_int32_t));
  m_notReqPkt = 0;
} 

int
LHCb::MEPRxSvc::setupCounters(int n) {
  m_rxOct  = new u_int64_t[n];
  m_rxPkt  = new u_int64_t[n];
  m_badPkt = new u_int32_t[n];
  m_misPkt = new u_int32_t[n];
  m_notReqPkt = 0;
  return 0;
}

StatusCode 
LHCb::MEPRxSvc::initialize()  {
  StatusCode sc;

  if ((sc = Service::initialize()) != SatusCode::SUCCESS) return sc;
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering initialize....." << endmsg;
  if (checkProperties() || openSock() || allocRx()) return StatusCode::FAILURE;
  
  if (service("IncidentSvc", m_incidentSvc).isSuccess())  {
    int sc = lib_rtl_start_thread(MEPRxSvc::exec, this, &m_handle);
    if (lib_rtl_is_success(sc)) {
      return StatusCode::SUCCESS;
    }
    log << MSG::ERROR << "Failed to spawn main thread." << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::ERROR << "Failed to access incident service." << endmsg;
  return StatusCode::FAILURE;
}
StatusCode 
LHCb::MEPRxSvc::finalize()  {
  m_receiveEvents = false;
  int sc = lib_rtl_join_thread(m_handle);
  if ( m_incidentSvc ) m_incidentSvc->release();
  m_incidentSvc = 0;
  return StatusCode::SUCCESS;
}

// IRunable implementation: Run the object
StatusCode LHCb::MEPRxSvc::run() {
  m_receiveEvents = true;
  return StatusCode::SUCCESS;
}
#endif /* if 0 */
