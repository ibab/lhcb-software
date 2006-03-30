#ifndef _WIN32
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineKernel/src/MBM/MEPRx.cpp,v 1.2 2006-03-30 08:45:26 frankb Exp $
//	====================================================================
//  MEPRx.cpp
//	--------------------------------------------------------------------
//
//	Author    : Niko Neufeld
//      using code by Benjamin Gaidioz and Markus Frank
//
//	====================================================================
//#include "MDF/RawEventHelpers.h"
//#include "MDF/MEPEvent.h"
#include "MBM/MepProducer.h"
#include "WT/wt_facilities.h"
#include "mep.h"
#include <fcntl.h>
#ifdef _WIN32
#warning This program is not guaranteed to run on game-consoles.
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <functional>
#include <vector>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#define info if(1) ::printf
#define hard_stop() do{info("abort %s:%d\n",__FILE__,__LINE__);abort();}while(0)
#define fatal(m) do {perror(m);hard_stop();} while(0)
#define MAX_R_PACKET (0x10000 + 20)
#define MEP_SENT  MBM_NORMAL
#define MEP_ADDED (MEP_SENT + 1)
#define IP_HEADER_LEN 20
#define IP_PROTO_HLT  0xF2

static int r; /* receive socket */
static int dry = 0;

namespace {
  int setup_socket(int buf_size, int netdev_in, int ip_proto_in) {
    char netdev_name[10];
    
    if ((r = socket(PF_INET, SOCK_RAW, ip_proto_in)) < 0) 
      fatal("socket");
    info("  listening for IP %02x set receive buffer size to %dB (%dMB)\n", ip_proto_in, buf_size, 
	 buf_size / 0x100000);
    if (setsockopt(r, SOL_SOCKET, SO_RCVBUF, (void *)
		   &buf_size, sizeof(buf_size))) fatal("setsockopt");
    sprintf(netdev_name, netdev_in < 0 ? "lo" : "eth%d", netdev_in);           
    info("  binding socket to device \"%s\"\n", netdev_name);
    if (setsockopt(r, SOL_SOCKET, SO_BINDTODEVICE, (void *) netdev_name,
		   1 + strlen(netdev_name))) fatal("setsockopt");
    return r;
  }
}
static void 
help() {
    ::printf("mep_prod_a -opt [-opt]\n");
    ::printf("    -n(ame)=<name>         buffer member name\n");
    ::printf("    -a(synchronous)        Asynchonous mode (default is synchronous)\n");
    ::printf("    -s(pace)=<number>      Default space allocation in kBytes\n");
    ::printf("    -p(artition)=<number>  Partition ID\n");
    ::printf("    -r(efcount)=<number>   Initial MEP reference count\n");
  }

namespace MEP {
  struct MEPRx: public MEP::Producer  {
    int m_spaceSize, m_refCount, m_age;
    u_int32_t m_l0id, m_brx, m_nrx, m_nsrc;
      u_int16_t m_pf;
    MEPEVENT *m_e;  
    MEPRx(const std::string& nam, int partitionID, int refcnt, 
		size_t siz) 
    : MEP::Producer(nam, partitionID), m_spaceSize(siz), m_refCount(refcnt)
    {
      m_spaceSize *= 1024;  // Space size is in kBytes
      m_flags = USE_MEP_BUFFER;
      if (!dry) { 
	  include();
	  m_bmid = m_mepID->mepBuffer;
	  ::printf(" MEP    buffer start: %08X\n",m_mepID->mepStart);
	  ::printf(" EVENT  buffer start: %08X\n",m_mepID->evtStart);
	  ::printf(" RESULT buffer start: %08X\n",m_mepID->resStart);
      }
      ::printf(" Buffer space: %d bytes\n",m_spaceSize);
    }
    ~MEPRx()  {
    }    
    int spaceRearm(int) {
      return dry ? 0 : MEP::Producer::spaceRearm(m_spaceSize);
    }
    int spaceAction() {
      static int id = -1;
      int status;

      m_e->evID        = ++id;
      m_event.len = m_brx + sizeof(MEPEVENT);
      m_event.mask[0] = partitionID();
      m_event.mask[1] = 0;
      m_event.mask[2] = 0;
      m_event.mask[3] = 0;
      m_event.type    = EVENT_TYPE_MEP;

      if (m_nrx != m_nsrc) {
	  info("incomplete %d!\n", m_nrx);
      }
      *((int *) m_e->data) = m_brx;
      if (!dry) { 
	  declareEvent();
	  status = sendSpace();
      }
      //info("bang %d %d bytes\n", m_e->evID, m_brx);
      m_brx = m_nrx = 0;
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
    int addMEP(mep_hdr_t *hdr) {
      int len = 0;

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
			     
  };
  static inline bool 
  cmpL0ID(MEPRx *r, u_int32_t id) {
      //info("cmpL0ID: m_l0ID %d id %d\n", r->m_l0id, id);
    return r->m_l0id < id;
  }
} 
typedef struct iphdr ip_hdr_t;
#define hdrlen (IP_HEADER_LEN +sizeof(mep_hdr_t)) 
static u_int8_t hdr[hdrlen];
static ip_hdr_t *iphdr = (ip_hdr_t *) hdr;
static mep_hdr_t *mephdr = (mep_hdr_t *) &hdr[IP_HEADER_LEN];
typedef std::vector<MEP::MEPRx *>::iterator RXIT;
#define echohdr(hdr) info("src %08x evt %08x\n", ((ip_hdr_t *) hdr) ->saddr, \
			  ((mep_hdr_t *) ((char *) hdr + sizeof(ip_hdr_t))) ->l01_id);

extern "C" int meprx(int argc, char **argv) {
  RTL::CLI cli(argc, argv, help);
  int size = 1024;             // default 64 kB + 256 byte event builder
  int partID = 0x103;              // default is LHCb partition id
  int refCount = 2;
  int nmax = 1;                    // maximum number of concurrent events
  int netdev_in = 1, ip_proto_in = IP_PROTO_HLT, bufsiz = 0x200000;
  int maxfd = 2;                   
  std::vector<MEP::MEPRx *> freedsc, workdsc;
  std::string name = "MEPRX";
  bool async = 0;
  fd_set fds;
  int nsrc = 247, space;
  struct iovec mep_recv_vec[1];
  RXIT rx;
  struct msghdr mep_recv_msg; 
  int harrumph = 00;

  cli.getopt("name", 2, name);
  cli.getopt("space", 1, space);
  cli.getopt("partitionid", 1, partID);
  cli.getopt("refcount", 1, refCount);
  cli.getopt("maxevent", 2, nmax); 
  cli.getopt("netdev_in", 2, netdev_in);
  cli.getopt("ip_proto_in", 1, ip_proto_in);
  cli.getopt("bufsiz", 1, bufsiz);
  cli.getopt("nsrc", 2, nsrc);
  cli.getopt("dry", 1, dry);
  cli.getopt("harrumph", 2, harrumph);

  info("%synchronous MEP Receiver \"%s\" Partition:%d (pid:%d)"\
	   "included in buffers.\n",
	   async ? "As" : "S", name.c_str(), partID, MEP::MEPRx::pid());
  
  if (dry) info("running in drymode\n");
  for (int i = 0; i < nmax; ++i) {
    MEP::MEPRx *rx = new MEP::MEPRx(name + char(i+'0'), partID, refCount, size);
    if ( async ) rx->setNonBlocking(WT_FACILITY_DAQ_SPACE, true);
    freedsc.push_back(rx);
    rx->m_nsrc = nsrc;
  }
  info("%d receiver%s created\n", nmax, nmax == 1 ? "" : "s");
  workdsc.clear();
  setup_socket(bufsiz, netdev_in, ip_proto_in);
  FD_ZERO(&fds); 
  maxfd = r + 1;
  int pktin = 0;
 
  for (;;) {
    int n;
    struct timeval timeout = {2, 0}; /* seconds */
    
    FD_SET(r, &fds);
    if ((n = select(maxfd, &fds, NULL, NULL, &timeout)) < 0) fatal("select");
    if (n == 0) {
      time_t t = time(NULL);
      char tbuf[32];
//      info("crhhh...%s", ctime_r(&t, tbuf));
      continue;
    }
    if (FD_ISSET(r, &fds)) {
//      echohdr(hdr);
#if 0 /* need a state machine for this to work */
	if (harrumph--) {
	     recvmsg(r, &mep_recv_msg, MSG_DONTWAIT);
	     continue;
	}
#endif
      mep_recv_vec->iov_base = hdr;
      mep_recv_vec->iov_len = hdrlen;
      mep_recv_msg.msg_name = 0;
      mep_recv_msg.msg_namelen = 0; // sizeof(struct sockaddr_in),
      mep_recv_msg.msg_iov = mep_recv_vec;
      mep_recv_msg.msg_iovlen = 1;
      mep_recv_msg.msg_flags = 0;
      mep_recv_msg.msg_control = NULL;
      mep_recv_msg.msg_controllen = 0;     
      int len = recvmsg(r, &mep_recv_msg, MSG_DONTWAIT | MSG_PEEK);
      if (len < 0) {
	if (errno == EAGAIN) continue;
	else fatal("recvmsg"); 
      } 
      //::fprintf(stdout, "\rpktin %d ", ++pktin);
      //    ::fflush(stdout);
      if (!workdsc.empty() && mephdr->l01_id == workdsc.back()->m_l0id) {
	rx = --workdsc.end();
      } else {
	rx = lower_bound(workdsc.begin(), workdsc.end(), mephdr->l01_id, 
			     MEP::cmpL0ID);
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
	      std::cout << "Exeption " << e.what() << std::endl;
	  }
	} 
      }
      if ((*rx)->addMEP(mephdr) == MEP_SENT) {
	workdsc.erase(rx);
	freedsc.push_back(*rx);
      }
    }
  }
}

      
/*
 * check if event matches last -- yes handle
 * if not perform binary search in list if match handle
 * if no match run through list enter at correct position, while running through age all events by one, if find a 0 one and extra space is needed, force a send, * handle event at now correct position
    
 */
 
#endif
