#ifndef GAUDIONLINE_MEPRXSYS_H
#define GAUDIONLINE_MEPRXSYS_H
//	============================================================
//
//	MEPRxSys.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Niko Neufeld 
//                 
//  OS dependent part of the event-builder
//	===========================================================
#include <string>
#include "RTL/types.h"

#define MEPRX_PEEK		 1
#define MEPRX_WRONG_FD 	-2
#define MEP_REQ_TTL 	10
#define MEP_REQ_TOS 	0xFD  /* the D is historic from the MDPs :-) */

/*
 *  namespace MEPRxSys
 */
namespace MEPRxSys {
  /// Print error message
  std::string sys_err_msg(void);
  int open_sock(int ipproto, int rxbufsiz, int netdev, std::string ifname, 
		bool mepreq, std::string &errmsg);
  int rx_select(int sockfd, int msec);
  int recv_msg(int sockfd, void *buf, int len,  int flags);
  int recv_msg(int sockfd, void *buf, int len,  int flags, u_int64_t *whentsc,
	       u_int64_t *when, std::string &errmsg);
  int rx_would_block(void);
  std::string dotted_addr(u_int32_t addr);
  int parse_addr(const std::string &straddr, u_int32_t &addr);
  int cinet_addr(const std::string &addr);
  int addr_from_name(const std::string &hname, u_int32_t &addr, std::string &msg);
  int name_from_addr(u_int32_t addr, std::string &hname, std::string &msg);
  u_int32_t IPStringToBits(std::string &StrIPAddr);
  int send_msg(int sockfd, u_int32_t addr, u_int8_t, void *buf, int len, int flags);
  int open_sock_arb_source(int ipproto, int rxbufsize, std::string &errmsg);
  int send_msg_arb_source(int raw_socket, u_int8_t proto, u_int32_t srcAddr, u_int32_t destAddr, void *buf, int len);
  int send_msg_arb_source(int raw_socket, u_int8_t proto, u_int32_t srcAddr, u_int32_t destAddr, void *buf, int len, u_int16_t datagramID);
  int rx_poll(int sockfd, int msec);
  int open_sock_udp(std::string &errmsg, int port);
  int send_udp(int udp_socket, u_int16_t port, u_int32_t dstAddr, void* buf, int len);
  bool is_in_file_no_case(const std::string &str, const std::string &filename, std::string &error);
  void gethostname_short(std::string &hostname);
	
//Msf: usleep is a (unprotected!!!!!) define by DIM !!!!
  void microsleep(int us);
  unsigned long ms2k();

// high precision timing (not for the X-box)
// from  http://www.mcs.anl.gov/~kazutomo/rdtsc.html
#ifdef linux
#if defined(__i386__)

static __inline__ u_int64_t rdtsc(void)
{
  u_int64_t x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#elif defined(__x86_64)

static __inline__ u_int64_t rdtsc(void)
{
  u_int32_t hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#elif defined(__powerpc__)


static __inline__ unsigned long long rdtsc(void)
{
  u_int64_t result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}

#endif
#elif _WIN32
static inline unsigned long long int rdtsc()   {  return 0;   }
#endif // linux

} // namespace MEPRxSys

#endif // GAUDIONLINE_MEPRXSYS_H
