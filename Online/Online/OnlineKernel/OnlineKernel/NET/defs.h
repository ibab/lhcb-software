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

#elif defined(linux)

#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define socket_close close
#endif

#endif  /* __INTERNET_STUFF_H__ */
