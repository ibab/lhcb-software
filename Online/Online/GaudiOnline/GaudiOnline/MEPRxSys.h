//	============================================================
//
//	MEPRxWin.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Niko Neufeld 
//                 
//  Windoze replacements for Unix system calls
//	===========================================================
#ifdef _WIN32
  #ifndef u_int64_t
  #define u_int64_t unsigned __int64
  #endif
  #ifndef u_int32_t
  #define u_int32_t unsigned __int32
  #endif
  #ifndef u_int16_t
  #define u_int16_t unsigned __int16
  #endif
  #ifndef u_int8_t
  #define u_int8_t  unsigned  __int8
  #endif
  #define __LITTLE_ENDIAN 0
  #define __BIG_ENDIAN 1
  #define __BYTE_ORDER __LITTLE_ENDIAN
#else
  #include <sys/types.h>
#endif

#include <string>
#define MEPRX_PEEK 1
#define MEPRX_WRONG_FD -2
#define MEP_REQ_TTL 10
#define MEP_REQ_TOS 0xFD  /* the D is historic from the MDPs :-) */

namespace MEPRxSys {
	std::string sys_err_msg(void);
	int open_sock(int ipproto, int rxbufsiz, int netdev, std::string ifname, bool mepreq, std::string &errmsg);
  int rx_select(int sec);
	int recv_msg(void *buf, int len,  int flags);
	int rx_would_block(void);
	std::string dotted_addr(u_int32_t addr);
	int parse_addr(const std::string &straddr, u_int32_t &addr);
	int addr_from_name(const std::string &hname, u_int32_t &addr, std::string &msg);
	int name_from_addr(u_int32_t addr, std::string &hname, std::string &msg);
	int send_msg(u_int32_t addr, void *buf, int len, int flags);
	void usleep(int us);
} // namespace MEPRxSys
