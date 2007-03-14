#ifdef BUILD_WRITER

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

#include "Writer/FailoverMonitor.h"
#include "Writer/Connection.h"
#include "Writer/MM.h"
#include "Writer/Utils.h"

#include "GaudiKernel/MsgStream.h"

#define POLL_INTERVAL         5000   /*<< The interval for poll() calls in millis.*/
#define FAILOVER_RETRY_SLEEP  20     /*<< Number of seconds between  retries.*/

#define STOP_URGENT				0x01	/**<< Tells a thread to stop immediately.*/
#define STOP_AFTER_PURGE	0x02	/**<< Tells a thread to stop after purging queue.*/


#define MDFWRITER_THREAD  0x01   /**<< MDF Writer GaudiAlgorithm thread identifier.*/
#define ACK_THREAD   			0x02   /**<< Acknowledgement thread identifier.*/
#define SEND_THREAD       0x03   /**<< Sender thread identifier.*/
#define FAILOVER_THREAD   0x04	 /**<< Failover thread identifier.*/

/**
 * Returned by Connection::failover() to tell a thread that failover is already done,
 * and that it must die to be respawned.
 */
#define KILL_THREAD		(-1)
#define RESUME_THREAD	0

/**
 * A thread local variable to keep track of which thread we're in.
 */
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
  m_log = log;
  m_notifyClient = nClient;
  m_sndRcvSizes = sndRcvSizes;
  m_failoverMonitor = NULL;

  pthread_mutex_init(&m_failoverLock, NULL);
  currThread = MDFWRITER_THREAD;
}

/**
 * Connects to a storage cluster node.
 */
void Connection::initialize(void) {
	m_failoverMonitor = new FailoverMonitor(m_serverAddr, m_serverPort + 1, this, m_log);
	connect();
	startAckThread();
	startSendThread();
}

/** Connects to a storage cluster node.
 */
void Connection::connect() {
  struct sockaddr_in destAddr;
  m_sockfd = 0;

  m_state = Connection::STATE_CONN_CLOSED;

  while(1) {
    m_failoverMonitor->getNextAddress(&destAddr);
  	m_sockfd = Utils::connectToAddress(&destAddr, m_sndRcvSizes, m_sndRcvSizes, m_log);
  	if(m_sockfd >= 0)
  		break;
 		*m_log << MSG::WARNING << "Could not connect, next server..." << endmsg;
 		sleep(1);
  }
  m_state = Connection::STATE_CONN_OPEN;
}


/** Closes the TCP connection.
 * This function blocks till all messages in the queue
 * are confirmed to have been sent.
 */
void Connection::closeConnection()
{
  stopSendThread(STOP_AFTER_PURGE);	/*Stop after all messages are sent.*/
  stopAckThread(STOP_AFTER_PURGE);	/*Stop after all acks have been received.*/
  Utils::closeSocket(&m_sockfd, m_log);
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

	ret = pthread_mutex_trylock(&m_failoverLock);
	if(ret == EBUSY) /* Someone else is already in the failover routine. Die.*/
		return KILL_THREAD;

	/*m_stopSending could be STOP_PURGE or 0, we must save and restore. */
	int oldSendStopLevel = m_stopSending;
	int oldAckStopLevel = m_stopAcking;

	/*
	 * This is called only by the sender thread or the
	 * ack thread. Either way, the other guy must be stopped.
	 */
	if(currThread == ACK_THREAD)
		stopSendThread(STOP_URGENT);
	else if(currThread == SEND_THREAD)
		stopAckThread(STOP_URGENT);

  reInitAckThread();	/*Reinit ack thread's data.*/
	reInitSendThread();	/*Reinit send thread's data.*/

	Utils::closeSocket(&m_sockfd, m_log);

  *m_log << MSG::INFO << "Reconnecting socket now..." << endmsg;
  connect();

  /* One of the two threads has been killed. So we have to
   * revive that one.
   */
  if(currThread == SEND_THREAD)
  	startAckThread();
  else if(currThread == ACK_THREAD)
  	startSendThread();

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
  int totalSize = 0;

start:
  struct cmd_header *cmd_to_send = NULL;

  *m_log << MSG::INFO << "Started send thread." << endmsg;

	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopSending) || (m_stopSending == STOP_AFTER_PURGE && cmd_to_send)) {

		cmd_to_send = m_mmObj.moveSendPointer();
    if(!cmd_to_send)
      continue;

    totalSize = sizeof(struct cmd_header);
    if(cmd_to_send->cmd == CMD_WRITE_CHUNK)
    	totalSize += cmd_to_send->data.chunk_data.size;

    ptr = (char *)cmd_to_send;
		ret = Utils::send(m_sockfd, ptr, totalSize, m_log);
		if(ret != totalSize) {
        if(failover() == KILL_THREAD)
        	return 0;
        else
        	goto start;
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
  struct ack_header ackHeaderBuf;

start:
	memset(&ackHeaderBuf, 0, sizeof(struct ack_header));

	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopAcking) ||
  	(m_stopAcking == STOP_AFTER_PURGE && m_mmObj.getQueueLength() != 0))
  {

    ret = Utils::brecv(m_sockfd, &ackHeaderBuf, sizeof(struct ack_header), m_log);
    if(ret != sizeof(struct ack_header)) {
      *m_log << MSG::WARNING << "Disconnected, should fail over." << endmsg;
      if(failover() == KILL_THREAD)
      	return 0;
      else
      	goto start;
    }

    unsigned int totalNumAcked = 0;
    if(ackHeaderBuf.min_seq_num > ackHeaderBuf.max_seq_num) { /*Sequence wraparound?*/
      totalNumAcked = 0xffffffff - ackHeaderBuf.min_seq_num + 1;
      totalNumAcked += ackHeaderBuf.max_seq_num;
    } else {
      totalNumAcked = ackHeaderBuf.max_seq_num - ackHeaderBuf.min_seq_num + 1;
    }

    for(unsigned int i=0, seqNum=ackHeaderBuf.min_seq_num;
        i<totalNumAcked;
        i++,seqNum++) {

      struct cmd_header *cmd;
      if((cmd = m_mmObj.dequeueCommand(seqNum)) == NULL) {
        *m_log << MSG::ERROR << "FATAL, Received an unsolicited ack." << endmsg;
      } else {
      	notify(cmd);
        m_mmObj.freeCommand(cmd);
      }
    }
  }
  return 0;
}

inline void Connection::notify(struct cmd_header *cmd)
{
  if(!m_notifyClient)
  	return;
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
  newHeader = MM::allocAndCopyCommand(header, data);	/* Will always succeed. */

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
  *m_log << MSG::INFO << "Starting send thread. . .";
  m_stopSending = 0;
  ret = pthread_create(&m_sendThread, NULL, send_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create send thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Done." << endmsg;
}

/**
 * Reinits the structures that the send thread uses,
 */
void Connection::reInitSendThread(void) {
	m_mmObj.resetSendPointer();
	*m_log << MSG::INFO << "Reset send thread data." << endmsg;
}


/**
 * Stops the send thread (blocking).
 */
void Connection::stopSendThread(int stopLevel)
{
  int ret;
  *m_log << MSG::INFO << "Stopping send thread. . .";
  m_stopSending = stopLevel;
  ret = pthread_join(m_sendThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop send thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Done." << endmsg;
}


/**
 * Starts up the ack thread.
 */
void Connection::startAckThread()
{
  int ret;
  m_stopAcking = 0;
  *m_log << MSG::INFO << "Starting ack thread . . .";
  ret = pthread_create(&m_ackThread, NULL, ack_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create ack thread " << errno << endmsg;
    throw std::runtime_error("Could not start ack thread.");
  }
  *m_log << MSG::INFO << "Done." << endmsg;
}

/**
 * Reinits the structures that the ack thread uses.
 */
void Connection::reInitAckThread(void) {
  *m_log << MSG::INFO << "Reinited ack thread data." << endmsg;
}

/**
 * Stops the ack thread (blocking).
 */
void Connection::stopAckThread(int stopLevel)
{
  int ret;
  *m_log << MSG::INFO << "Stopping ack thread . . .";
  m_stopAcking = stopLevel;
  ret = pthread_join(m_ackThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop ack thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Done." << endmsg;

}

#endif /* _WIN32 */
