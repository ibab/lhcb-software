#include <string>
#include <stdexcept>
#include <iostream>

extern "C" {
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "MDF/chunk_headers.h"
}

#include "MDF/Connection.h"
#include "MDF/MM.h"
#include "GaudiKernel/MsgStream.h"


using namespace LHCb;

/** Connects to a storage cluster node.
 * After that, obtains the addresses of all failover nodes.
 * @param serverAddr The hostname of the server to connect to. This
 *                   must not be an IP address to enable round-robin DNS.
 * @param serverPort The port on the server to connect to.
 * @param soTimeout The SO_TIMEOUT value to use for the socket.
 * @param sndRcvSizes The TCP send/receive buffer size.
 * @param msgSvc An IMessageSvc to m_log messages to.
 */
void Connection::connectAndNegotiate(std::string serverAddr, int serverPort,
    int soTimeout, int sndRcvSizes, MsgStream * log)
{
  int ret;
  int sockoptval;
  socklen_t optlen;
  struct hostent *hostname;
  int numRetries = 0;

  m_seqCounter = 0;
  m_state = Connection::STATE_CONN_CLOSED;
  m_serverAddr = serverAddr;
  m_serverPort = serverPort;
  m_ackHeaderReceived = 0;
  m_log = log;

  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(m_sockfd < 0) {
    throw std::runtime_error("Could not create socket.");
  }

  //Set options.
  optlen = sizeof(sockoptval);
  ret = setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, &soTimeout, optlen);
  if(ret < 0) {
    *m_log << MSG::WARNING << "Could not set SND_TIMEO size." << endmsg;
  }
  ret = setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &sndRcvSizes, optlen);
  if(ret < 0) {
    *m_log << MSG::WARNING << "Could not set SO_SNDBUF size." << endmsg;
  }
  ret = setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &sndRcvSizes, optlen);
  if(ret < 0) {
    *m_log << MSG::WARNING << "Could not set SO_RCVBUF size." << endmsg;
  }

  while(numRetries < MAX_RETRIES) {

    //Resolve the name.
    hostname = gethostbyname(m_serverAddr.c_str());
    if(!hostname) {
      throw std::runtime_error("Could not resolve host name.");
    }

    //Init the IP address.
    memset(&m_destAddr, 0, sizeof(m_destAddr));
    m_destAddr.sin_family = AF_INET;
    m_destAddr.sin_port = htons(m_serverPort);
    m_destAddr.sin_addr.s_addr = inet_addr(m_serverAddr.c_str());

    //TODO: Must be a name lookup.

    //Connect
    ret = connect(m_sockfd, (struct sockaddr*)&m_destAddr,
	(socklen_t)sizeof(struct sockaddr_in));

    if(ret == 0) {
      //TODO: Get the list of other nodes that can be connected to.
      m_state = Connection::STATE_CONN_OPEN;
      return;
    }

    if(errno == EINTR)
      continue;

    *m_log << MSG::WARNING << "Could not connect to server errno=" 
      << errno << ", attempt number " << numRetries << endmsg;

    numRetries++;
  }

  throw std::runtime_error(std::string("Could not connect to an initial host.") + hostname->h_addr_list[0]);
}


/** Closes the TCP connection.
  * This function blocks till all messages in the queue
  * are confirmed to have been sent.
  */
void Connection::closeConnection()
{
  while(m_mmObj.getQueueLength() != 0)
    processAcks(1);

  shutdown(m_sockfd, SHUT_RDWR);

  close(m_sockfd);
  m_sockfd = -1;
}

/** Fails over onto an alternative storage cluster server.
  * This entails connecting to a different server and 
  * re-sending all the queued requests.
  */
void Connection::failover()
{
	//TODO: Fail over
}


/** Processes acknowledgements received from the server.
  * All received acknowledgements are removed from the command
  * cache. Acknowledgements are read nonblocking from the
  * server as long as there are acks to be read.
  * @param blocking Block until all pending acknowledgements are
  * received if 1, do not block if 0.
  */
void Connection::processAcks(int blocking)
{
  int ret;
  unsigned char *ptr;

  ptr = (unsigned char*)&m_ackHeaderBuf;

  while(m_mmObj.getQueueLength() > 0) {

    ret = recv(m_sockfd, ptr + m_ackHeaderReceived,
	sizeof(struct ack_header) - m_ackHeaderReceived,
	blocking ? MSG_WAITALL : MSG_DONTWAIT);

    if(ret < 0 && (errno == EINTR || errno == EAGAIN)) {
      if(!blocking)
	return;
      else
	ret = 0;
    } else if(ret == 0) {

      *m_log << MSG::WARNING << "Disconnected should fail over." << endmsg;

      failover();

    } else if(ret < 0) {

      *m_log << MSG::WARNING << "Receive failed, should fail over." << endmsg;

      failover();
    }

    m_ackHeaderReceived += ret;

    if(m_ackHeaderReceived == sizeof(struct ack_header)) {
      m_ackHeaderReceived = 0;

      //Process the ack.
      unsigned int totalNumAcked = 0;
      if(m_ackHeaderBuf.min_seq_num > m_ackHeaderBuf.max_seq_num) {
	//Wraparound of sequence number has occurred.
	totalNumAcked = 0xffffffff - m_ackHeaderBuf.min_seq_num + 1;
	totalNumAcked += m_ackHeaderBuf.max_seq_num;
      } else {
	totalNumAcked = m_ackHeaderBuf.max_seq_num -
	  m_ackHeaderBuf.min_seq_num + 1;
      }

      //Go over the elements from the front, pop all that match.
      for(unsigned int i=0, seqNum=m_ackHeaderBuf.min_seq_num;
	  i<totalNumAcked;
	  i++,seqNum++) {

	std::list<struct cmd_header*>::iterator first, last;
	first = m_mmObj.getCommandIterator();
	last = m_mmObj.getLastCommand();

	while(first != last) {
	  if((*first)->data.start_data.seq_num == seqNum) {
	    struct cmd_header* del_this;
	    del_this = (struct cmd_header*)*first;

	    m_mmObj.dequeueCommand(first);
	    m_mmObj.freeCommand(del_this);
	    break;
	  }
	  first++;
	}
	if(first == last) {

          *m_log << MSG::ERROR << "FATAL, Received an unsolicited ack." << endmsg;
	  break;
	}
	if(m_mmObj.getQueueLength() == 0)
	  break;
      }
    } else {
      //Haven't read enough to process a full ack. Let's try later.
      break;
    }
  }

}


/**
  * Equivalent to sendCommand(header, NULL);
  */
void Connection::sendCommand(struct cmd_header *header)
{
  sendCommand(header, NULL);
}

/** Sends a command to the server.
  * This command can be a close request, an open request, or a
  * write request. The Connection class takes care of failing
  * over in case of a problem. In case the command could not be
  * sent even after a failover attempt, then the INotifyClient
  * interface is used to notify the calling class of the error.
  * @param header The command header which contains the command
  *               to send. The sequence number does not need to
  *               be set at this point. This is set automatically
  *               by this method.
  * @param data   In case of the WRITE_CHUNK command, this will
  *               be non-NULL. The sizeof this buffer will be
  *		  determined from header.
  */
void Connection::sendCommand(struct cmd_header *header, void *data)
{
  //Make a copy of the command first.
  struct cmd_header *newHeader;
  int totalSize = 0;
  int ret;
  char *ptr;
  int bytes_sent = 0;

  switch(header->cmd) {
    case CMD_WRITE_CHUNK:
      totalSize += header->data.chunk_data.size;
    
      case CMD_CLOSE_FILE:
      m_state = STATE_CONN_OPEN;
    case CMD_OPEN_FILE:
      m_state = STATE_FILE_OPEN;
      totalSize += sizeof(struct cmd_header);
      break;
  }

  header->data.chunk_data.seq_num = m_seqCounter++;
  newHeader = MM::allocAndCopyCommand(header, data);

  if(!newHeader) {
    throw std::runtime_error("Could not alloc memory for a new command.");
  }

  //Queue the command.
  m_mmObj.enqueueCommand(newHeader);

  ptr = (char *)newHeader;
  bytes_sent = 0;
  while(bytes_sent != totalSize) {
    ret = send(m_sockfd,
	ptr + bytes_sent,
	totalSize - bytes_sent, 0);
    if(ret < 0 && (errno != EAGAIN && errno != EINTR)) {
      *m_log << MSG::ERROR << "Could not send command: errno= " <<
	errno << ", failing over." << endmsg;
       failover();
    } else if(ret == 0) {
      *m_log << MSG::ERROR << "Could not send command: disconnected" <<
	errno << ", failing over." << endmsg;
       failover();
    } else if(ret > 0) {
      bytes_sent += ret;
    } else {
      continue;
    }
  }

  //Try to read some acknowledgements right away.
  processAcks(0);

}
