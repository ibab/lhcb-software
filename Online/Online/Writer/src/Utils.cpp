#ifdef BUILD_WRITER

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/signal.h>

#include <cerrno>
#include <sstream>
#include <stdexcept>

#include "GaudiKernel/MsgStream.h"
#include "Writer/Utils.h"

using namespace LHCb;

/**
 * Blocks some signals for threads that don't want to process them.
 */
void Utils::blockSignals(void)
{
  sigset_t signals;
  sigemptyset(&signals);
  sigaddset(&signals, SIGPIPE);
  pthread_sigmask(SIG_BLOCK, &signals, NULL);
}

int Utils::nameLookup(const char *serverAddr,
    struct sockaddr_in *destAddr, MsgStream * /*log*/) {

  struct hostent* hostname = NULL;
  hostname = gethostbyname(serverAddr);
  if(!hostname || hostname->h_length == 0 || hostname->h_addr_list[0] == NULL) {
    return -1;
  }
  memcpy(&destAddr->sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
  return 0;
}

int Utils::nameLookup(std::string &serverAddr,
    struct sockaddr_in *destAddr, MsgStream * log) {
  return Utils::nameLookup(serverAddr.c_str(), destAddr, log);
}

int Utils::setupSocket(int sndBufSize, int rcvBufSize, MsgStream *log) {
  socklen_t optlen;
  int ret, retSocket = socket(AF_INET, SOCK_STREAM, 0);

  if(retSocket < 0) {
    throw std::runtime_error("Could not create socket.");
  }
  //Set options.
  optlen = sizeof(sndBufSize);
  ret = setsockopt(retSocket, SOL_SOCKET, SO_SNDBUF, &sndBufSize, optlen);
  if(ret < 0 && log) {
    *log << MSG::WARNING << "Could not set SO_SNDBUF size." << endmsg;
  }
  ret = setsockopt(retSocket, SOL_SOCKET, SO_RCVBUF, &rcvBufSize, optlen);
  if(ret < 0 && log) {
    *log << MSG::WARNING << "Could not set SO_RCVBUF size." << endmsg;
  }
  return retSocket;
}

int Utils::connectToAddress(struct sockaddr_in *destAddr,
    int sndSize, int rcvSize, MsgStream *log)
{
  int ret;
  int sock = setupSocket(sndSize, rcvSize, log);

  if(sock < 0)
    return -1;

  ret = ::connect(sock, (struct sockaddr*)destAddr,
                (socklen_t)sizeof(struct sockaddr_in));

  if(ret != 0) {
    std::stringstream str;
    str << "Failed to open socket to addr: "
        << ::inet_ntoa(destAddr->sin_addr) << "  ["
        << std::hex << ntohl(destAddr->sin_addr.s_addr)
        << "] Errno: " << std::dec << errno 
        << "  [" << strerror(errno)  << "]" << std::endl;
    ::close(sock);
    throw std::runtime_error(str.str());
    return -1;
  }
  return sock;
}

int Utils::closeSocket(int *sock, MsgStream * /*log*/) {
  ::shutdown(*sock, SHUT_RDWR);
  ::close(*sock);
  *sock = -1;
  return 0;
}

/**
 * Receives data into the buffer and sleeps till pol
 */
int BIF::nbRecv(MsgStream *log)
{
  int ret;

  struct pollfd fds[1];
  fds[0].fd = m_sockFd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  ret = poll(fds, 1, POLL_TIMEOUT_MSEC);
  int errout = (int)errno;
  if(ret == 0) {
    return AGAIN;
  } else if(ret < 0 && (errno == EAGAIN || errno == EINTR || errno == ENOENT)) {
    return AGAIN;
  }else if(ret < 0) {
	  *log << MSG::WARNING << "ret <0 on poll(), ERRNO is: "<< errout << endmsg;
    return DISCONNECTED;
  } else if(fds[0].revents & POLLIN) {
    ret = ::recv(m_sockFd, m_data+m_bytesRead, m_bufLen-m_bytesRead, MSG_DONTWAIT);
    if(ret < 0 && (errno == EAGAIN || errno == EINTR || errno == ENOENT)) {
      return AGAIN;
    } else if(ret == 0 || ret < 0) {
    	errout = (int)errno;
    	*log << MSG::WARNING << " ret = " << ret <<" on recv(), m_bufLen = " << m_bufLen << " m_bytesRead = " << m_bytesRead << " , ERRNO is: "<< errout << endmsg;
      return DISCONNECTED;
    } else {
      m_bytesRead += ret;
    }
  }
  if(m_bytesRead == m_bufLen) {
    return m_bytesRead;
  }
  return AGAIN;
}

/**
 * Sends data from the buffer and returns only when the data is completely
 * sent, except in case of a timeout.
 */
int BIF::nbSendTimeout()
{
  int ret;
  struct pollfd fds[1];
  time_t endTime;
  fds[0].fd = m_sockFd;
  fds[0].events = POLLOUT|POLLERR;
  fds[0].revents = 0;

  endTime = time(NULL) + FAILURE_TIMEOUT_SEC;

  while(time(NULL) < endTime) {
    ret = ::poll(fds, 1, POLL_TIMEOUT_MSEC);
    if((ret < 0 && (errno == EAGAIN || errno == EINTR)) || ret == 0)
      continue;
    else if(ret < 0)
      return DISCONNECTED;

    int ret = ::send(m_sockFd, m_data+m_bytesRead, m_bufLen-m_bytesRead, MSG_DONTWAIT);
    if(ret < 0 && (errno == EAGAIN || errno == EINTR))
      continue;
    else if(ret <= 0)
      return DISCONNECTED;
    else
      m_bytesRead += ret;

    if(m_bytesRead == m_bufLen)
      return m_bytesRead;
  }
  return TIMEDOUT;
}

/**
 * Sends data from the buffer and returns only when the data is completely
 * sent, except in case the thread is stopped.
 */
int BIF::nbSend(MsgStream *log)
{
  int ret = ::send(m_sockFd, m_data+m_bytesRead, m_bufLen-m_bytesRead, MSG_DONTWAIT);
  int errout = (int)errno;
  if(ret < 0 && (errno == EAGAIN || errno == EINTR))
    return AGAIN;
  else if(ret <= 0)
  {
//	  std::stringstream str;
//	  str << "ret <=0 on send(), ERRNO is: "<< errout <<std::endl;
	  *log << MSG::WARNING << "ret <=0 on send(), ERRNO is: "<< errout << endmsg;
//	*log << MSG::WARNING << "ret <=0, ERRNO is: " << errno << endmsg;
    return DISCONNECTED;
  }
  else
    m_bytesRead += ret;

  if(m_bytesRead == m_bufLen)
    return m_bytesRead;
  else
    return AGAIN;
}

/**
 * Receives data from the buffer and returns only when the data is completely
 * sent, except in case the thread is stopped.
 */
int BIF::nbRecvTimeout()
{
  int ret;
  struct pollfd fds[1];
  time_t endTime;
  fds[0].fd = m_sockFd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  endTime = time(NULL) + FAILURE_TIMEOUT_SEC;

  while(time(NULL) < endTime) {
    ret = ::poll(fds, 1, POLL_TIMEOUT_MSEC);
    if((ret < 0 && (errno == EAGAIN || errno == EINTR)) || ret == 0)
      continue;
    else if(ret < 0)
      return DISCONNECTED;

    int ret = ::recv(m_sockFd, m_data+m_bytesRead, m_bufLen-m_bytesRead, MSG_DONTWAIT);
    if(ret < 0 && (errno == EAGAIN || errno == EINTR))
      continue;
    else if(ret <= 0)
      return DISCONNECTED;
    else
      m_bytesRead += ret;

    if(m_bytesRead == m_bufLen)
      return m_bytesRead;
  }
  return TIMEDOUT;
}

#endif
