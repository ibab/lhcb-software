#ifndef __INTERNET_STUFF_H__
#define __INTERNET_STUFF_H__


#ifdef _VMS
#include </multinet_root/multinet/include/sys/types.h>
#include </multinet_root/multinet/include/sys/socket.h>
#include </multinet_root/multinet/include/netinet/in.h>
#include </multinet_root/multinet/include/arpa/inet.h>
#include </multinet_root/multinet/include/netdb.h>
#include </multinet_root/multinet/include/errno.h>
#include </multinet_root/multinet/include/vms/inetiodef.h>

#elif defined(_OSK)

#include <aos9def.h>
#include <socket_proto.h>

#elif defined(_WIN32)

#include <io.h>
#include <cerrno>
#include <winsock.h>
#define socket_close closesocket
#define ESOCK_INTR        WSAEINTR
#define ESOCK_TIMEDOUT    WSAETIMEDOUT
#define ESOCK_WOULDBLOCK  WSAEWOULDBLOCK
#define ESOCK_NOTCONN     WSAENOTCONN
#define ESOCK_CONNREFUSED WSAECONNREFUSED
#define ESOCK_NOTSOCK     WSAENOTSOCK
#define ESOCK_CONNABORTED WSAECONNABORTED
#define ESOCK_CONNRESET   WSAECONNRESET
#define ESOCK_NETRESET    WSAENETRESET
#define ESOCK_NETDOWN     WSAENETDOWN
#define ESOCK_SHUTDOWN    WSAESHUTDOWN
#define ESOCK_OPNOTSUPP   WSAEOPNOTSUPP
#define ESOCK_INVAL       WSAEINVAL
#define ESOCK_FAULT       WSAEFAULT
#define ESOCK_NOBUFS      WSAENOBUFS
#define ESOCK_AGAIN       EAGAIN
#define ESOCK_BADF        EBADF

#elif defined(__linux)

#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#define socket_close close

#define ESOCK_INTR        EINTR
#define ESOCK_WOULDBLOCK  EWOULDBLOCK
#define ESOCK_NOTCONN     ENOTCONN
#define ESOCK_CONNREFUSED ECONNREFUSED
#define ESOCK_CONNABORTED ECONNABORTED
#define ESOCK_CONNRESET   ECONNRESET
#define ESOCK_NETRESET    ENETRESET
#define ESOCK_NETDOWN     ENETDOWN
#define ESOCK_SHUTDOWN    ESHUTDOWN
#define ESOCK_TIMEDOUT    ETIMEDOUT
#define ESOCK_NOBUFS      ENOBUFS
#define ESOCK_OPNOTSUPP   EOPNOTSUPP
#define ESOCK_AGAIN       EAGAIN
#define ESOCK_NOTSOCK     ENOTSOCK
#define ESOCK_INVAL       EINVAL
#define ESOCK_FAULT       EFAULT
#define ESOCK_BADF        EBADF

#endif

#if defined(_VMS) || defined(VMS)                        /* _VMS   */
typedef unsigned short __NetworkChannel__;               /* _VMS   */
#else  // _OSK, WIN32 and linux
typedef int            __NetworkChannel__;
#ifdef _WIN32
typedef int socklen_t;
#endif
#endif

typedef unsigned short     __NetworkPort__;
typedef unsigned short     __NetworkFamily__;
typedef struct sockaddr_in __NetworkAddress__;
typedef struct in_addr     __NetworkSubAddress__;

#endif  /* __INTERNET_STUFF_H__ */
