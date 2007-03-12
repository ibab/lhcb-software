#ifndef _WIN32

#include <string>
#include <cerrno>
#include <stdexcept>
#include <iostream>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
extern "C" {
#include "Writer/chunk_headers.h"
}

#include "Writer/Connection.h"
#include "Writer/MM.h"
#include "GaudiKernel/MsgStream.h"

#define POLL_INTERVAL         5000   /*5 Seconds.*/
#define MAX_EMPTY_POLLS        6        /*Meaning 30 seconds of empty polls.*/
#define FAILOVER_RETRY_SLEEP  20      /*Seconds between  retries.*/

/**Used to tell a thread to stop _now_, regardless the queue elements left to purge.*/
#define STOP_URGENT				0x01

/**Used to tell a thread to stop after it has purged all remaining queue elements.*/
#define STOP_AFTER_PURGE	0x02

/*Thread identifiers.*/

/**The MDF Writer GaudiAlgorithm thread. */
#define MDFWRITER_THREAD 0x01
/**The thread that processes acknowledgements. */
#define ACK_THREAD   0x02
/**The thread that sends queued messages to the server. */
#define SEND_THREAD   0x03

/**
 * Returned by Connection::failover() to tell a thread that failover is already done,
 * and that it must die to be respawned.
 */
#define KILL_THREAD		(-1)

/**A thread local variable to keep track of which thread we're in.*/
__thread int currThread;

using namespace LHCb;

/**
 * Constructor.
 * Does nothing except initialise member variables.
 */
Connection::Connection(std::string serverAddr, int serverPort, int sndRcvSizes,
    MsgStream * log, INotifyClient *nClient) {

  m_seqCounter = 0;
  m_state = Connection::STATE_CONN_CLOSED;
  m_serverAddr = serverAddr;
  m_serverPort = serverPort;
  m_ackHeaderReceived = 0;
  m_log = log;
  m_notifyClient = nClient;
  m_sndRcvSizes = sndRcvSizes;

  pthread_mutex_init(&m_failoverLock, NULL);
  currThread = MDFWRITER_THREAD;
}

/**
 * Connects to a storage cluster node.
 */
void Connection::connectToServer(void)
{
  connectAndNegotiate(0);
}

/**
 * Connects to a storage cluster node and starts the processing threads.
 */
void Connection::connectAndStartThreads(void)
{
  connectAndNegotiate(1);
}

/** Connects to a storage cluster node.
 * After that, obtains the addresses of all failover nodes.
 */
void Connection::connectAndNegotiate(int startThreads)
{
  int ret;
  int sockoptval;
  socklen_t optlen;
  int numRetries = 0;
  struct hostent* hostent = 0;

  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(m_sockfd < 0) {
    throw std::runtime_error("Could not create socket.");
  }

  //Set options.
  optlen = sizeof(sockoptval);
  ret = setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &m_sndRcvSizes, optlen);
  if(ret < 0) {
    *m_log << MSG::WARNING << "Could not set SO_SNDBUF size." << endmsg;
  }
  ret = setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &m_sndRcvSizes, optlen);
  if(ret < 0) {
    *m_log << MSG::WARNING << "Could not set SO_RCVBUF size." << endmsg;
  }

  while(numRetries < MAX_RETRIES) {

    //Resolve the name.
    hostname = gethostbyname(m_serverAddr.c_str());
    if(!hostname || hostname->h_length == 0 || hostname->h_addr_list[0] == NULL) {
    	*m_log << MSG::WARNING << "Could not resolve name : " <<
    		m_serverAddr.c_str() << ", will retry. . ." << endmsg;
    	sleep(3);
    	continue;
    }

    //Init the IP address.
    memset(&m_destAddr, 0, sizeof(m_destAddr));
    m_destAddr.sin_family = AF_INET;
    m_destAddr.sin_port = htons(m_serverPort);
    memcpy(&m_destAddr.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);

    //Connect
    ret = connect(m_sockfd, (struct sockaddr*)&m_destAddr,
        (socklen_t)sizeof(struct sockaddr_in));

    if(ret == 0) {
      //TODO: Get the list of other nodes that can be connected to.
      *m_log << MSG::INFO << "Connected to a host." << endmsg;
      m_state = Connection::STATE_CONN_OPEN;
      if(startThreads) {
	      startAckThread();
	      startSendThread();
      }
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
  // Will get out only after _everything_ has been sent.
  stopSendThread(STOP_AFTER_PURGE);

  // Will get out only after everything has been acked.
  stopAckThread(STOP_AFTER_PURGE);

  shutdown(m_sockfd, SHUT_RDWR);
  close(m_sockfd);
  m_sockfd = -1;
}

/** Fails over onto an alternative storage cluster server.
 * This entails connecting to a different server and
 * re-sending all the queued requests.
 * @param stopAckThread If 1, then the ackThread needs
 * to be stopped. If 0, then it means that the function
 * is being called by the ack thread, and the thread
 * will stop on its own after function returns.
 */
int Connection::failover()
{
	int ret = 0;


	/* So that only one thread does failover recovery. */
	ret = pthread_mutex_trylock(&m_failoverLock);
	if(ret == EBUSY) {
		/* Someone else is already in the failover routine, so let this thread
		 * die and be respawned later.
		 */
		return KILL_THREAD;
	}

	int oldSendStopLevel = m_stopSending;
	int oldAckStopLevel = m_stopAcking;

	/*
	 * This is called only by the sender thread or the
	 * ack thread. Either way, the other guy must be stopped.
	 */
	if(currThread == ACK_THREAD) {
		*m_log << MSG::INFO << "In Failover from ACK_THREAD, stopping SEND_THREAD. . .";
		stopSendThread(STOP_URGENT);
		*m_log << "Stopped." << endmsg;
	} else if(currThread == SEND_THREAD) {
		*m_log << MSG::INFO << "In Failover from SEND_THREAD, stopping ACK_THREAD. . .";
		stopAckThread(STOP_URGENT);
		*m_log << "Stopped." << endmsg;
	}

  //Reinit stuff that the ack thread works with.
  m_ackHeaderReceived = 0;
  memset(&m_ackHeaderBuf, 0, sizeof(m_ackHeaderBuf));
  *m_log << MSG::INFO << "Reinited ack thread data." << endmsg;

  //Reinit stuff that the sender thread works with.
  //The send pointer is where the sender thread starts sending from.
  m_mmObj.resetSendPointer();
  *m_log << MSG::INFO << "Reinited send thread data." << endmsg;

  //Close the socket, shut it down.
  shutdown(m_sockfd, SHUT_RDWR);
  close(m_sockfd);
  m_sockfd = -1;

  *m_log << MSG::INFO << "Reconnecting socket now..." << endmsg;

  //Retry till the end of time.
  while(1) {
    //Now connect and negotiate.
    try{
      connectToServer();
      break;
    }catch(std::exception& ex) {
      *m_log << MSG::FATAL
        << "Could not failover to a different storage cluster, will try again."
        << endmsg;
    }
    sleep(FAILOVER_RETRY_SLEEP);
  }

  /* One of the two threads has been killed. So we have to
   * revive that one.
   */
  if(currThread == SEND_THREAD) {
  	*m_log << MSG::INFO << "Reviving ACK_THREAD. . .";
  	startAckThread();
  	*m_log << "Done" << endmsg;
  } else if(currThread == ACK_THREAD) {
  	*m_log << MSG::INFO << "Reviving SEND_THREAD. . .";
  	startSendThread();
  	*m_log << "Done" << endmsg;
  }

  /* It could be possible that the failover is taking place
   * during a finalize()/Connection::closeConnection(), where the
   * threads were in STOP_AFTER_PURGE state. So for consistency,
   * we just restore them to whatever state they were in.
   */
  m_stopAcking = oldAckStopLevel;
  m_stopSending = oldSendStopLevel;

  *m_log << MSG::INFO << "Successfully failed over. " << endmsg;
  pthread_mutex_unlock(&m_failoverLock);

  return 0;
}

/** Processes elements from the send queue.
 * The elements from this queue are sent over a socket, one by one.
 */
int Connection::processSends(void)
{
  int ret;
  char *ptr;
  int bytes_sent = 0;
  int totalSize = 0;

start:
  struct cmd_header *cmd_to_send;

  *m_log << MSG::INFO << "Started send thread." << endmsg;
  cmd_to_send = m_mmObj.moveSendPointer();

	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopSending) ||
  	(m_stopSending == STOP_AFTER_PURGE && cmd_to_send)) {

    if(!cmd_to_send) {
    	*m_log << MSG::DEBUG << "Sleeping, nothing to send." << endmsg;
			cmd_to_send = m_mmObj.moveSendPointer();
      continue;
    }

	  *m_log << MSG::DEBUG << "Got command:" << (cmd_to_send->data.start_data.seq_num) << endmsg;

    totalSize = sizeof(struct cmd_header);
    if(cmd_to_send->cmd == CMD_WRITE_CHUNK)
    	totalSize += cmd_to_send->data.chunk_data.size;

    ptr = (char *)cmd_to_send;
    bytes_sent = 0;
    while(bytes_sent != totalSize) {

      ret = send(m_sockfd, ptr + bytes_sent, totalSize - bytes_sent, MSG_WAITALL);

      if(ret < 0 && (errno != EAGAIN && errno != EINTR)) {
        *m_log << MSG::ERROR << "Could not send command: errno= " <<
          errno << ", failing over." << endmsg;
        if(failover() == KILL_THREAD)
        	return 0;
        else
        	goto start;
      } else if(ret == 0) {
        *m_log << MSG::ERROR << "Could not send command: disconnected" <<
          errno << ", failing over." << endmsg;
        if(failover() == KILL_THREAD)
        	return 0;
        else
        	goto start;
      } else if(ret > 0) {
        bytes_sent += ret;
      } else if(ret < 0) {
      	continue;
      }
    }
    cmd_to_send = m_mmObj.moveSendPointer();
  }

  return 0;
}




/** Processes acknowledgements received from the server.
 * All received acknowledgements are removed from the command
 * cache. Acknowledgements are read nonblocking from the
 * server as long as there are acks to be read.
 */
int Connection::processAcks(void)
{
  int ret;
  unsigned char *ptr;

  struct pollfd pollfds[1];

start:
  pollfds[0].events = POLLIN | POLLERR;
  pollfds[0].revents = 0;
  pollfds[0].fd = m_sockfd;
  ptr = (unsigned char*)&m_ackHeaderBuf;


	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopAcking) ||
  	(m_stopAcking == STOP_AFTER_PURGE && m_mmObj.getQueueLength() != 0))
  {
    pollfds[0].revents = 0;
    ret = poll(pollfds, 1, POLL_INTERVAL);
    if(ret == 0) {
      continue;
    } else if(ret < 0 && (errno == EINTR || errno == EAGAIN)) {
      continue;
    } else if(ret < 0) {
      *m_log << MSG::WARNING << "Poll failed, errno = " << errno << endmsg;
      continue;
    }

    /* Means we got something to read. */
    ret = recv(m_sockfd, ptr + m_ackHeaderReceived,
        sizeof(struct ack_header) - m_ackHeaderReceived,
        MSG_DONTWAIT);

    /* On failover, just exit - we'll be restarted anyway */
    if(ret < 0 && (errno == EINTR || errno == EAGAIN)) {
      ret = 0;
    } else if(ret == 0) {
      *m_log << MSG::WARNING << "Disconnected, should fail over." << endmsg;
      if(failover() == KILL_THREAD)
      	return 0;
      else
      	goto start;
    } else if(ret < 0) {
      *m_log << MSG::WARNING << "Receive failed, should fail over." << endmsg;
      if(failover() == KILL_THREAD)
      	return 0;
      else
      	goto start;
    }

    m_ackHeaderReceived += ret;

    /* Still haven't received a full acknowledgement header, loop over. */
    if((unsigned)m_ackHeaderReceived < sizeof(struct ack_header))
      continue;

    m_ackHeaderReceived = 0;

    //Process the ack.
    unsigned int totalNumAcked = 0;
    if(m_ackHeaderBuf.min_seq_num > m_ackHeaderBuf.max_seq_num) {
      //Wraparound of sequence number has occurred.
      totalNumAcked = 0xffffffff - m_ackHeaderBuf.min_seq_num + 1;
      totalNumAcked += m_ackHeaderBuf.max_seq_num;
    } else {
      totalNumAcked = m_ackHeaderBuf.max_seq_num - m_ackHeaderBuf.min_seq_num + 1;
    }

    for(unsigned int i=0, seqNum=m_ackHeaderBuf.min_seq_num;
        i<totalNumAcked;
        i++,seqNum++) {

      struct cmd_header *cmd;

      if((cmd = m_mmObj.dequeueCommand(seqNum)) == NULL) {
        *m_log << MSG::ERROR << "FATAL, Received an unsolicited ack." << endmsg;
      } else {
        if(m_notifyClient) {
          switch(cmd->cmd) {
            case CMD_CLOSE_FILE:
              m_notifyClient->notifyClose(cmd);
              break;
            case CMD_OPEN_FILE:
              m_notifyClient->notifyOpen(cmd);
              break;
            default:
              break;
          }
        }
        m_mmObj.freeCommand(cmd);
      }
    }
  }
  return 0;
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
 *      determined from header.
 */
void Connection::sendCommand(struct cmd_header *header, void *data)
{
  //Make a copy of the command first.
  struct cmd_header *newHeader;
  int totalSize = 0;
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
  m_mmObj.enqueueCommand(newHeader);
}

/**
 * The ack thread body.
 * This thread just starts the processAcks() method of the
 * specified connection object in a separate thread.
 */
static void *ack_thread(void *args)
{

  Connection *conn;
  conn = (Connection*)args;
  currThread = ACK_THREAD;
  conn->processAcks();
  return NULL;
}

/**
 * The sender thread body.
 * This thread just starts the startSending() method of
 * the specified connection object in a separate thread.
 */
static void *send_thread(void *args)
{
  Connection *conn;
  conn = (Connection*)args;
  currThread = SEND_THREAD;
  conn->processSends();
  return NULL;
}

/**
 * Starts up the send thread.
 */
void Connection::startSendThread()
{
  int ret;
  m_stopSending = 0;
  ret = pthread_create(&m_sendThread, NULL, send_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create send thread " << errno << endmsg;
  }
}

/**
 * Stops the send thread (blocking).
 */
void Connection::stopSendThread(int stopLevel)
{
  int ret;
  m_stopSending = stopLevel;
  ret = pthread_join(m_sendThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop send thread " << errno << endmsg;
  }
}


/**
 * Starts up the ack thread.
 */
void Connection::startAckThread()
{
  int ret;
  m_stopAcking = 0;
  ret = pthread_create(&m_ackThread, NULL, ack_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create ack thread " << errno << endmsg;
    throw std::runtime_error("Could not start ack thread.");
  }
}

/**
 * Stops the ack thread (blocking).
 */
void Connection::stopAckThread(int stopLevel)
{
  int ret;
  m_stopAcking = stopLevel;
  ret = pthread_join(m_ackThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop ack thread " << errno << endmsg;
  }
}

#endif /* _WIN32 */
