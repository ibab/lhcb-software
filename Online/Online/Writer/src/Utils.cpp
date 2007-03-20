#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <stdexcept>

#include "GaudiKernel/MsgStream.h"
#include "Writer/Utils.h"

using namespace LHCb;

int Utils::nameLookup(std::string &serverAddr,
	struct sockaddr_in *destAddr, MsgStream * /*log*/) {

  //Resolve the name.
	struct hostent* hostname = NULL;
  hostname = gethostbyname(serverAddr.c_str());
  if(!hostname || hostname->h_length == 0 || hostname->h_addr_list[0] == NULL) {
  	return -1;
  }
  memcpy(&destAddr->sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
  return 0;
}

int Utils::setupSocket(int sndBufSize, int rcvBufSize, MsgStream *log) {

  socklen_t optlen;
  int retSocket;
  int ret;

	retSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(retSocket < 0) {
    throw std::runtime_error("Could not create socket.");
  }

  //Set options.
  optlen = sizeof(sndBufSize);
  ret = setsockopt(retSocket, SOL_SOCKET, SO_SNDBUF, &sndBufSize, optlen);
  if(ret < 0) {
    *log << MSG::WARNING << "Could not set SO_SNDBUF size." << endmsg;
  }
  ret = setsockopt(retSocket, SOL_SOCKET, SO_RCVBUF, &rcvBufSize, optlen);
  if(ret < 0) {
    *log << MSG::WARNING << "Could not set SO_RCVBUF size." << endmsg;
  }

  return retSocket;
}

int Utils::connectToAddress(struct sockaddr_in *destAddr,
	int sndSize, int rcvSize, MsgStream *log)
{
	int ret;
	int sock;

	sock = setupSocket(sndSize, rcvSize, log);
	if(sock < 0)
		return -1;

	*log << MSG::INFO << "Addr: port = " <<
		((destAddr->sin_addr.s_addr & 0xff)) << "." <<
		((destAddr->sin_addr.s_addr & 0xff00) >> 8) << "." <<
		((destAddr->sin_addr.s_addr & 0xff0000) >> 16) << "." <<
		((destAddr->sin_addr.s_addr & 0xff000000) >> 24) << ":" << 
		ntohs(destAddr->sin_port) << endmsg;

	*log << MSG::DEBUG << endmsg;
 
	ret = connect(sock, (struct sockaddr*)destAddr,
      (socklen_t)sizeof(struct sockaddr_in));


	if(ret != 0) {
	   *log << MSG::ERROR << "Could not connect, errno = " << errno << endmsg;
		return -1;
	}
  return sock;
}

int Utils::closeSocket(int *sock, MsgStream * /*log*/) {
	shutdown(*sock, SHUT_RDWR);
  close(*sock);
  *sock = -1;
  return 0;
}

int Utils::send(int sock, void *data, size_t datalen, MsgStream * /*log*/) {
	int ret = 0;
	while(1) {
		ret = ::send(sock, data, datalen, MSG_WAITALL);
		if(ret < 0) {
			if(errno == EINTR || errno == EAGAIN) {
				continue;
			} else {
				return ret;
			}
		} else {
			return ret;
		}
	}
	return ret;
}

/**
 * Performs a receive.
 * @return The number of bytes received, or -1 in case of an error.
 */
int Utils::brecv(int sock, void *data, size_t datalen, MsgStream * /*log*/) {
	int ret;
	
	while(1) {
		ret = recv(sock, data, datalen, MSG_WAITALL);
		if(ret < 0 && (errno == EAGAIN || errno == EINTR))
			continue;
		else if(ret == 0)
			return -1;
		else
			return ret;
	}
}
