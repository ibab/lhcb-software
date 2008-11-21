//  ============================================================
//
//  MEPRxSys.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : Niko Neufeld 
//                 
//  System dependent parts of the eventbuilder MEPRxSvc
//  ===========================================================
#include "GaudiOnline/MEPRxSys.h"
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <ctime>

#ifdef _WIN32
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#define herrno 0
#define hstrerror(x) sys_err_msg()
#define SOL_IP IPPROTO_IP
#define SHUT_RD SD_RECEIVE
#else // UNIX 
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h> 
#endif // ifndef _WIN32

// Like the Windows and Unix syscalls
// *all* functions return 0 on success and > 0 on a failure 
// this differs from OnlineKernel, which provides a (sentimental) mimickry
// of VMS syscalls (which IMHO is not a good idea).


namespace MEPRxSys {
#ifndef _WIN32
// wrapper classes for Unix System structs
struct MsgHdr: public msghdr {
  MsgHdr(struct iovec *v, int n) {
    msg_iov = v; msg_iovlen = n; msg_control = NULL; msg_controllen = 0; msg_name = NULL; msg_namelen = 0; }
};
struct InAddr: public in_addr {
  InAddr(u_int32_t a) {
    s_addr = a;
  }
};
struct IOVec: public iovec {
  IOVec(void *base, size_t len) {
    iov_base = base; iov_len = len;
  }
};
#endif // _WIN32

std::string sys_err_msg(void) {
#ifdef _WIN32  
  char msg[512];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, msg, 512, NULL);
  std::string errstr(msg);
#else
  std::string errstr(strerror(errno));
#endif
  return errstr;
}

std::string dotted_addr(u_int32_t addr)
{
  struct in_addr in;

#ifdef _WIN32
  in.S_un.S_addr = addr;
#else
  in.s_addr = addr;
#endif
  return inet_ntoa(in);
}

int open_sock(int ipproto, int rxbufsiz, int netdev, std::string ifname, 
          bool mepreq, std::string &errmsg) 
{
  int retSockFd = -1;
#ifndef _WIN32
  char netdev_name[10];
  int fd;
  if ((fd = open("/proc/raw_cap_hack", O_RDONLY)) != -1) {
    ioctl(fd, 0, 0);  
    close(fd);
  } // if we can't open the raw_cap_hack we have to be root 
#endif
  u_int32_t myaddr = inet_addr(ifname.c_str());
  struct in_addr addr;
#ifdef _WIN32
  addr.S_un.S_addr = myaddr; 
#else
  addr.s_addr = myaddr;
#endif
  struct sockaddr_in saddr = {AF_INET, 0, addr,0, }; 
  if ((retSockFd = socket(AF_INET, SOCK_RAW, ipproto)) < 0) {
    errmsg = "socket";
    goto drop_out;
  }

  if (setsockopt(retSockFd, SOL_SOCKET, SO_RCVBUF, (const char *)
     &rxbufsiz, sizeof(rxbufsiz))) {
    errmsg = "setsockopt SO_RCVBUF";
    goto shut_out;
  }
  if (myaddr == INADDR_NONE) { 
#ifdef linux
    sprintf(netdev_name, netdev < 0 ? "lo" : "eth%d", netdev);           
    if (setsockopt(retSockFd, SOL_SOCKET, SO_BINDTODEVICE, (void *) netdev_name,
       1 + strlen(netdev_name))) {
      errmsg = "setsockopt SO_BINDTODEVICE";
      goto shut_out;
    }

#else
    errmsg = "inet_addr(" + ifname + ")";
    goto shut_out;
#endif
  } else {
    if (bind(retSockFd, (const struct sockaddr *) &saddr, sizeof(saddr))) {
      errmsg = "bind";
      goto shut_out;
    } 
  }
  if (mepreq) {
    int val;
    val = MEP_REQ_TTL;
    if (setsockopt(retSockFd, SOL_IP, IP_TTL, (const char *) &val, sizeof(int))) {
      errmsg = "setsockopt SOL_IP TTL";
      goto shut_out;
    }
  } 
  return retSockFd;
shut_out:
  shutdown(retSockFd, SHUT_RD);
drop_out:
  return -1;
}

int recv_msg(int sockfd, void *buf, int len,  int flags)
{
  int ioflags = 0;
#ifndef _WIN32
  struct IOVec bufs(buf, len);
  struct MsgHdr msg(&bufs, 1);
#endif
  if (flags & MEPRX_PEEK) {
   ioflags |= MSG_PEEK;
  }
#ifdef _WIN32
  int rlen = recv(sockfd, (char *) buf, len, ioflags);
  return ((rlen == -1 && WSAGetLastError() == WSAEMSGSIZE) ? len : rlen);  
#else 
  return (recvmsg(sockfd, &msg, ioflags | MSG_DONTWAIT));
#endif
}

#ifdef _WIN32
int send_msg(int sockfd, u_int32_t addr, u_int8_t protocol, void *buf, int len, int flags) {
  int ioflags = 0;
  struct in_addr in;
  in.S_un.S_addr = addr;
  struct sockaddr_in sinaddr = {AF_INET, protocol, in, 0,}; 
  return (sendto(sockfd, (const char *) buf, len, ioflags, 
           (const struct sockaddr *) &sinaddr, sizeof(sinaddr)));
#else
int send_msg(int sockfd, u_int32_t addr, u_int8_t protocol, void *buf, int len, int /* flags */) {
  struct IOVec bufs(buf, len);  
  struct MsgHdr msg(&bufs, 1);
  struct in_addr in;
  in.s_addr = addr;
  static struct sockaddr_in _addr = { AF_INET, protocol, in, 0,};
  msg.msg_name = &_addr;
  msg.msg_namelen = sizeof(_addr);
  return (sendmsg(sockfd, &msg, MSG_DONTWAIT | MSG_CONFIRM));
#endif
}

/** Special function for sending messages with arbitrary source ip address.
 * Currently used by MEPInjector
 */
int
send_msg_arb_source(int raw_socket, u_int8_t proto, u_int32_t srcAddr, u_int32_t destAddr, void *buf, int len) {
  int n = 0;
#ifndef _WIN32
  struct sockaddr_in in;
  struct iphdr *hdr = (struct iphdr *) buf;
  hdr->saddr = srcAddr; //inet_addr(srcAddr);
  hdr->daddr = destAddr; //inet_addr(destAddr);
  hdr->version = 4;
  hdr->ihl = 5;
  hdr->ttl = 4;
  hdr->protocol = proto;
  in.sin_family = AF_INET;
  in.sin_port = proto;
  memcpy(&(in.sin_addr), &(hdr->daddr), 4);

  if ((n = sendto(raw_socket, buf, len, 0, (struct sockaddr *) &in, sizeof(in))) != len) {
    perror("send");
    exit(errno);
  }
#endif
  return n;
}

//Also socket need some special treatment.
int
#ifdef _WIN32
  open_sock_arb_source(int ipproto, int rxbufsiz, std::string &errmsg)
#else
  open_sock_arb_source(int /* ipproto */, int rxbufsiz, std::string &errmsg)
#endif
{
  int raw_socket;
#ifndef _WIN32
  int fd;
  if ((fd = open("/proc/raw_cap_hack", O_RDONLY)) != -1) {
    ioctl(fd, 0, 0);
    close(fd);
  } // if we can't open the raw_cap_hack we have to be root
#endif
  if ((raw_socket = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
    errmsg = "socket";
    return -1;
  }

  if (setsockopt(raw_socket, SOL_SOCKET, SO_SNDBUF, (const char *)
     &rxbufsiz, sizeof(rxbufsiz))) {
     errmsg = "setsockopt SO_SNDBUF";
     return -1;
     //RCVBUF not needed
  }
#ifdef _WIN32
  if (setsockopt(raw_socket, SOL_IP, IP_HDRINCL, (char*)&ipproto, 4)) { // any non-zero value is fine.
    errmsg = "setsockopt";
    return -1;
  }
#else
  char netdev_name[10];
  int netdev = 1;
  sprintf(netdev_name, netdev < 0 ? "lo" : "eth%d", netdev);
  if (setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, (void *) netdev_name,
      1 + strlen(netdev_name))) {
    errmsg = "setsockopt SO_BINDTODEVICE";
    return -1;
  }
#endif

  int val;
  val = MEP_REQ_TTL;
  if (setsockopt(raw_socket, SOL_IP, IP_TTL, (const char *) &val, sizeof(int))) {
    errmsg = "setsockopt SOL_IP TTL";
    return -1;
  }

  return raw_socket;
}


int parse_addr(const std::string &straddr, u_int32_t &addr)
{
#ifdef _WIN32
  if ((addr = inet_addr(straddr.c_str())) == INADDR_NONE) return 1;
  else return 0;
#else
  struct in_addr a;
  int rc = inet_aton(straddr.c_str(), &a);
  addr = a.s_addr;
  return rc ? 0 : 1; // inet_aton returns 1 on success!
#endif
}
/**
 if successful hname will contain the trimmed (up to the first dot) hostname
 */
int 
name_from_addr(u_int32_t addr, std::string &hname, std::string &msg)
{
  struct hostent *h;

  if (!(h = gethostbyaddr((const char *) &addr, 4, AF_INET))) {
    msg = dotted_addr(addr) + " " + hstrerror(h_errno);
    return 1;
  } 
  hname = h->h_name;
  hname = hname.substr(0, hname.find_first_of("."));
  msg = "";
  return 0;
}

int 
addr_from_name(const std::string &hname, u_int32_t &addr, std::string &msg) 
{
  struct hostent *h;

  if (!(h = gethostbyname(hname.c_str()))) {
    msg = hname + " " + hstrerror(h_errno);
    return 1;
  }
  addr = *((u_int32_t *) h->h_addr_list[0]);    
  return 0;
}

void microsleep(int us) 
{
#ifdef _WIN32
  long millisecs = us/1000;
  ::Sleep( millisecs<=0 ? 1 : millisecs);
#else
  struct timespec t = { 0, 1000 * us }; // 0 s,  ns
  nanosleep(&t, &t);
#endif
}
/*
 * return the time in ms since (approx) Jan 1st 2000
 */
unsigned long ms2k(void)
{
#ifdef _WIN32
    static unsigned long ms = 0;
    return ms++; // Windows has neither future nor past...
#else
    struct timeval tv;
    const time_t sec2000 = 30 * 365 * 24 * 3600;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec - sec2000) * 1000 + tv.tv_usec / 1000);
#endif
}

int rx_select(int sockfd, int sec)
{
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(sockfd, &rfds);
  struct timeval timeout = {sec, 0}; /* seconds */
  int maxfd = sockfd + 1;
  int n;
  
  n = select(maxfd, &rfds, NULL, NULL, &timeout);
#ifdef _WIN32
  if (n == SOCKET_ERROR) return -1;
#else
  if (n == -1) return -1;
#endif
  if (n == 1 && !FD_ISSET(sockfd, &rfds)) return MEPRX_WRONG_FD;
  return n;
}

int rx_would_block() 
{
#ifdef _WIN32
  if (WSAGetLastError() == WSAEWOULDBLOCK) return 1;
#else
  if (errno == EAGAIN) return 1;
#endif
  return 0;
}
int cinet_addr(const std::string &addr) {
    return inet_addr(addr.c_str());
}

} // namespace MEPRxSys
