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
#include <cstdio>
#include <cerrno>
#include <ctime>
#include <sstream>
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
#include <sys/poll.h>
#endif // ifndef _WIN32
// Like the Windows and Unix syscalls
// *all* functions return 0 on success and > 0 on a failure
// this differs from OnlineKernel, which provides a (sentimental) mimickry
// of VMS syscalls (which IMHO is not a good idea).


namespace MEPRxSys {
    using namespace std;

#ifndef _WIN32
// wrapper classes for Unix System structs
struct MsgHdr: public msghdr {
    MsgHdr(struct iovec *v, int n ) {
        msg_iov = v;
        msg_iovlen = n;
        msg_control = NULL;
        msg_controllen = 0;
        msg_name = NULL;
        msg_namelen = 0;
    }
};
struct InAddr: public in_addr {
    InAddr(u_int32_t a) {
        s_addr = a;
    }
};
struct IOVec: public iovec {
    IOVec(void *base, size_t len) {
        iov_base = base;
        iov_len = len;
    }
};
#endif // _WIN32

std::string sys_err_msg(void)
{
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

// search string str in file, compare non-case sensitive
// string must be delimited by white-space and not contain whitespace itself
// anything after "#" is ignored
// file not there is not a problem (treated like not found)
bool is_in_file_no_case(const std::string & str, const std::string & filename, string & error)
{
    FILE *f;
    error = "";
    // file not there is not an error
    if (::access(filename.c_str(), F_OK) != 0) {
	return false;
    }
    // any other error on open is an error
    if (!(f = ::fopen(filename.c_str(), "r"))) {
        error =  sys_err_msg();
	return false;
    }
    // read all lines
    while (!::feof(f)) {
	char buf[80];
	if (!::fgets(buf, 80, f)) break;
	char *saveptr;
	if (buf[0] == '#') continue;
	char *tok = strtok_r(buf, " ", &saveptr);
	while (tok != NULL) {
	    if (tok[0] == '#') continue;
	    for (char *p = tok; *p != 0; *p = ::tolower(*p), ++p) {
		if (*p == '\n' || *p == '#') {
		    *p = 0; // newline/comment also ends a string for us
		    break;
		}
	    }
	    if (str.compare(tok) == 0) {
		fclose(f);
		return true;
	    }
	    tok = strtok_r(NULL, " ", &saveptr);
	}
    }
    fclose(f);
    return false;
}

int open_sock_udp(std::string &errmsg, int port)
{
    struct sockaddr_in si;
    int s;
    int on = 1;

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        errmsg = "socket";
        goto drop_out;
    }
    memset((char *) &si, 0, sizeof(si));
    si.sin_family = AF_INET;
    si.sin_port = htons(port);
    si.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (const sockaddr *) &si, sizeof(si)) == -1) {
        errmsg = "bind";
        goto drop_out;
    }
#ifdef linux
    if (setsockopt(s, SOL_SOCKET, SO_TIMESTAMP, &on, sizeof(on))) {
        errmsg = "setsockopt SO_TIMESTAMP";
        ::shutdown(s, SHUT_RD);
        goto drop_out;
    }
#endif
    return s;
drop_out:
    return -1;
}

int open_sock(int ipproto, int rxbufsiz, int netdev, std::string ifname,
              bool mepreq, std::string &errmsg)
{
    int retSockFd = -1;
#ifndef _WIN32
    char netdev_name[10];
    int fd;
    int on = 1;
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
    struct sockaddr_in saddr = {AF_INET, 0, addr,{0,}};
    if ((retSockFd = socket(AF_INET, SOCK_RAW, ipproto)) < 0) {
        errmsg = "socket";
        goto drop_out;
    }
    if (setsockopt(retSockFd, SOL_SOCKET, SO_RCVBUF, (const char *)
                   &rxbufsiz, sizeof(rxbufsiz))) {
        errmsg = "setsockopt SO_RCVBUF";
        goto shut_out;
    }
#ifdef linux
    if (setsockopt(retSockFd, SOL_SOCKET, SO_TIMESTAMP,
                   &on, sizeof(on))) {
        errmsg = "setsockopt SO_TIMESTAMP";
        goto shut_out;
    }
#endif
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

int recv_msg(int sockfd, void *buf, int len,  int flags, u_int64_t *whentsc,
             u_int64_t *when, std::string &errstr)
{
    int ioflags = 0;
#ifndef _WIN32
    struct IOVec bufs(buf, len);
#endif
    if (flags & MEPRX_PEEK) {
        ioflags |= MSG_PEEK;
    }
#ifdef _WIN32
    int rlen = recv(sockfd, (char *) buf, len, ioflags);
    *whentsc = *when = 0; // the X-Box knows no good time...
    return ((rlen == -1 && WSAGetLastError() == WSAEMSGSIZE) ? len : rlen);
#else
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct timeval))];
    } cmsg_un;
    struct cmsghdr *cmsg;
    struct timeval *tv;
    struct iovec vec[1];
    struct msghdr msg;

    *when = 0;
    vec[0].iov_base = buf;
    vec[0].iov_len = len;
    ::memset(&msg, 0, sizeof(msg));
    ::memset(&cmsg_un, 0, sizeof(cmsg_un));
    msg.msg_iov = vec;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_un.control;
    msg.msg_controllen = sizeof(cmsg_un.control);
    int s = ::recvmsg(sockfd, &msg, ioflags | MSG_DONTWAIT);
    *whentsc = rdtsc();
    if (s <= 0) {
        if (errno == EAGAIN || errno == EINTR)
            return 0;
        return s;
    }

    if (msg.msg_flags & MSG_TRUNC & !(flags & MEPRX_PEEK)) {
        errstr = "received truncated message";
        return s;
    }

    if (msg.msg_flags & MSG_CTRUNC) {
        errstr = "received truncated ancillary data";
        return s;
    }
    if (msg.msg_controllen < sizeof(cmsg_un.control)) {
        std::stringstream out;
        out << "received short ancillary data (" <<msg.msg_controllen << "/" <<
            sizeof(cmsg_un.control) << ")";
        errstr = out.str();
        return s;
    }
    tv = 0;
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
            cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMP)
            tv = (struct timeval *) CMSG_DATA(cmsg);
    }
    if (tv) {
        *when = 1000000 * tv->tv_sec + tv->tv_usec;
        if (*when == 0) *when = 1; // paranoid sanity check for wrap-around, because *when == 0 signals error
    } else
        errstr = "timevalue = NULL";

    return s;
#endif // ifndef _WIN32
}


#ifdef _WIN32
int send_msg(int sockfd, u_int32_t addr, u_int8_t protocol, void *buf, int len, int flags)
{
    int ioflags = 0;
    struct in_addr in;
    in.S_un.S_addr = addr;
    struct sockaddr_in sinaddr = {AF_INET, protocol, in, 0,};
    return (sendto(sockfd, (const char *) buf, len, ioflags,
                   (const struct sockaddr *) &sinaddr, sizeof(sinaddr)));

#else
//Sends only raw IP messages
int send_msg(int sockfd, u_int32_t addr, u_int8_t protocol, void *buf, int len, int /* flags */)
{
    struct IOVec bufs(buf, len);
    struct MsgHdr msg(&bufs, 1);
    struct in_addr in;
    in.s_addr = addr;
    static struct sockaddr_in _addr = { AF_INET, protocol, in, {0,}};
    msg.msg_name = &_addr;
    msg.msg_namelen = sizeof(_addr);
    return (sendmsg(sockfd, &msg, MSG_DONTWAIT | MSG_CONFIRM));
#endif
}

int
send_msg_arb_source(int raw_socket, u_int8_t proto, u_int32_t srcAddr, u_int32_t destAddr, void *buf, int len, u_int16_t datagramID)
{
#ifndef _WIN32
    struct iphdr *hdr = (struct iphdr *) buf;
    hdr->id = datagramID;

    return send_msg_arb_source(raw_socket, proto, srcAddr, destAddr, buf, len);
#else
    return 0;
#endif
}

/** Special function for sending messages with arbitrary source ip address.
 * Currently used by MEPInjector
 */

int
send_msg_arb_source(int raw_socket, u_int8_t proto, u_int32_t srcAddr, u_int32_t destAddr, void *buf, int len)
{
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

int
send_udp(int udp_socket, u_int16_t port, u_int32_t dstAddr, void* buf, int len)
{
    int n=0;
#ifndef _WIN32
    struct sockaddr_in dest;
    struct in_addr adr;
    adr.s_addr = dstAddr;
    dest.sin_family=AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr=adr;

    n = sendto(udp_socket, buf, len, 0, (const sockaddr *) &dest, sizeof(dest));

    //Let the user manage the errors
#endif
    return n;
}

//Also socket need some special treatment.
int
open_sock_arb_source(int ipproto, int rxbufsiz, std::string &errmsg)
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
    if (setsockopt(raw_socket, SOL_IP, IP_HDRINCL, (char*)&ipproto, 4)) { // any non-zero value is fine.
        errmsg = "setsockopt";
        return -1;
    }
    char netdev_name[10];
    int netdev = 1;
    sprintf(netdev_name, netdev < 0 ? "lo" : "eth%d", netdev);
#ifndef _WIN32
    if (setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, (void *) netdev_name,
                   1 + strlen(netdev_name))) {
        errmsg = "setsockopt SO_BINDTODEVICE";
        return -1;
    }
#endif

    int val;
    val = MEP_REQ_TTL;
    if (setsockopt(raw_socket, SOL_IP, IP_TTL, (const char *) &val,
                   sizeof(int))) {
        errmsg = "setsockopt SOL_IP TTL";
        return -1;
    }
    return raw_socket;
}


int
parse_addr(const std::string &straddr, u_int32_t &addr)
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
  get short hostname (without domain)
*/

void gethostname_short(std::string &hostname)
{
    char buf[80];
    
    ::gethostname(buf, 80);
    char *endp = buf;
    while (*endp != '\0' && *endp != '.') {
	*endp = ::tolower(*endp);
	++endp;
    }
    *endp = '\0';
    hostname.assign(buf);
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
    const unsigned long sec2000 = 365 * 24 * 3600;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec - sec2000) * 1000 + tv.tv_usec / 1000);
#endif
}

int cinet_addr(const std::string &addr)
{
    return inet_addr(addr.c_str());
}

/// poll the only socket in entry, so return only the number of socket which got the event we were waiting for, normal read
int rx_poll(int sockFd, int mSec)
{
#ifdef _WIN32
    return 0;
#else

    struct pollfd strPoll;

    /// Initialization of the pollfd structure,
    ///    * listen to odin socket
    ///    * wait for data to read
    ///    * clean the answer of the poll primitive (actually we don't care about the answer as we wait for only one event
    strPoll.fd = sockFd;
    strPoll.events = 0x0001;
    strPoll.revents = 0;

    int polRet = poll(&strPoll, 1, mSec);

    if (polRet == -1) { /// Error
        perror("poll");
        exit(errno);
    }
    if (polRet == 0) { /// Timeout
        return 0;
    }
    return polRet;
#endif
}


int rx_select(int sockfd, int millisec)
{
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    struct timeval timeout = {millisec / 1000, millisec % 1000};
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

u_int32_t IPStringToBits(std::string &StrIPAddr)
{
    u_int32_t BitIPAddr=0;
    if (MEPRxSys::parse_addr(StrIPAddr, BitIPAddr) != 0) {
        std::string msg;
        if(MEPRxSys::addr_from_name(StrIPAddr, BitIPAddr, msg) != 0) {
            return 0;
        }
    }
    return BitIPAddr;
}

} // namespace MEPRxSys
#if 0
;;;
Local Variables:
***
;;;
c-basic-offset:4 ***
;;;
End:
***
#endif
