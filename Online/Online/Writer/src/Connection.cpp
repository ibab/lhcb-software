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
#include "Writer/SendThread.h"

#include "GaudiKernel/MsgStream.h"

#include <map>
#define POLL_INTERVAL         5000   /*<< The interval for poll() calls in millis.*/
#define FAILOVER_RETRY_SLEEP  20     /*<< Number of seconds between  retries.*/

using namespace LHCb;

/**
 * Constructor.
 * Does nothing except initialise member variables.
 */
Connection::Connection(std::string serverAddr, int serverPort, int sndRcvSizes,
    MsgStream * log, INotifyClient *nClient, size_t maxQueueSize) : m_mmObj(maxQueueSize)  {

  m_state = Connection::STATE_CONN_CLOSED;
  m_serverAddr = serverAddr;
  m_serverPort = serverPort;
  m_log = new MsgStream(*log);
  m_maxQueueSize = maxQueueSize;
  m_notifyClient = nClient;
  m_sndRcvSizes = sndRcvSizes;
  m_failoverMonitor = NULL;
  m_stopRetrying = 0;
  pthread_mutex_init(&m_failoverLock, NULL);
}


/**
 * Connects to a storage cluster node.
 */
void Connection::initialize(void) {
  m_failoverMonitor = new FailoverMonitor(m_serverAddr, m_serverPort + 1, this, m_log);

  connect();

  m_failoverMonitor->start();

  m_ackThread = new AckThread(this, m_sockfd, &m_mmObj, m_log);
  m_ackThread->setNotifyClient(m_notifyClient);
  m_ackThread->start();

  m_sendThread = new SendThread(this, m_sockfd, &m_mmObj, m_log);
  m_sendThread->start();

  *m_log << MSG::INFO << WHERE
         << "Writer " << " Started all threads."
         << endmsg;
}

/** Connects to a storage cluster node.
 */
void Connection::connect() {
  struct sockaddr_in destAddr;
  m_sockfd = -1;
  m_state = Connection::STATE_CONN_CLOSED;
  m_failoverMonitor->getAddress(&destAddr);

  /*Need to change port to the storage service port.*/
  destAddr.sin_port = htons(m_serverPort);
  int a,b,c,d, addr;
  addr = htonl(destAddr.sin_addr.s_addr);
  a = (addr & 0xFF000000) >> 24;
  b = (addr & 0x00FF0000) >> 16;
  c = (addr & 0x0000FF00) >>  8;
  d = (addr & 0x000000FF);
  *m_log <<MSG::INFO << WHERE << "Trying to connect to address "
         << a << '.' << b << '.' << c << '.' << d  << std::dec
         << endmsg;
  m_sockfd = Utils::connectToAddress(&destAddr, m_sndRcvSizes, m_sndRcvSizes, m_log);
  if(m_sockfd < 0)
    throw std::runtime_error("Could not init stream to server.");
  m_state = Connection::STATE_CONN_OPEN;
}

/** Closes the TCP connection.
 * This function blocks till all messages in the queue
 * are confirmed to have been sent.
 */
void Connection::closeConnection()
{
  *m_log << MSG::INFO << WHERE << " Closing connection with axolotls." << endmsg;

  m_sendThread->stopAfterFinish();  /*Stop after all messages are sent.*/
  m_ackThread->stopAfterFinish();  /*Stop after all acks have been received.*/

  /* stopAfterFinish() doesn't do a thread join. So we wait till
   * the queue is empty. If the queue is empty, then the threads will
   * shut down almost immediately afterwards. getQueueLength() is locked,
   * so no harm in calling it often. After five retries (10 seconds) join the
   * threads anyway by calling stopUrgently
   */

  int ret;
  int retry = 5;
  while((ret = m_mmObj.getQueueLength()) > 0 && retry-- > 0) {
    *m_log << MSG::INFO << WHERE << "Waiting for queue length to be zero. Queue size is: " << ret << endmsg;
    for (std::map<unsigned int, MMQueue*>::iterator queueIter = (m_mmObj.m_runMap).begin(); queueIter != (m_mmObj.m_runMap).end(); queueIter++)
    {
    	*m_log << MSG::INFO << WHERE << "Run number: " << queueIter->first << " Queue Length: " << (queueIter->second)->getQueueLength() << endmsg;
    }
    sleep(2);
  }

  /* commented this out, because this was doing the job of
   * ensuring that the threads are not deleted (below) before the queue
   * is empty. this is now replaced with an explicit joining of the threads.
  */
//  sleep(2);

  /* The failover monitor is directly stopped using stopUrgently().
   */

  m_failoverMonitor->stopUrgently();
  m_sendThread->stopUrgently();
  m_ackThread->stopUrgently();
  delete m_sendThread;
  delete m_ackThread;
  delete m_failoverMonitor;

  Utils::closeSocket((int*)&m_sockfd, m_log);
}

/** Fails over onto an alternative storage cluster server.
 * This entails connecting to a different server and
 * re-sending all the queued requests.
 * @param stopAckThread If 1, then the ackThread needs
 * to be stopped. If 0, then it means that the function
 * is being called by the ack thread, and the thread
 * will stop on its own after function returns.
 */
int Connection::failover(int currThread)
{
  int ret = 0;
  MsgStream l_log(*m_log);

  ret = pthread_mutex_trylock(&m_failoverLock);
  if(ret == EBUSY) /* Someone else is already in the failover routine. Die.*/
    return KILL_THREAD;

  switch(currThread) {
    case ACK_THREAD:
      l_log << MSG::WARNING << WHERE
             << " Failover initiated by ACK thread. Errno is: " << errno
             << endmsg;
      break;
    case SEND_THREAD:
      l_log << MSG::WARNING << WHERE
             << " Failover initiated by SEND thread. Errno is: " << errno
             << endmsg;
      break;
    case FAILOVER_THREAD:
      l_log << MSG::WARNING << WHERE
             << " Failover initiated by FAILOVER thread. Errno is: " << errno
             << endmsg;

      break;
  }

  /*
   * This is called by one of the three threads. Stop the other two.
   */
  if(currThread == ACK_THREAD) {
    m_sendThread->stopUrgently();
    m_failoverMonitor->stopUrgently();
  } else if(currThread == SEND_THREAD) {
    m_ackThread->stopUrgently();
    m_failoverMonitor->stopUrgently();
  } else if(currThread == FAILOVER_THREAD) {
    m_sendThread->stopUrgently();
    m_ackThread->stopUrgently();
  }

  Utils::closeSocket((int*)&m_sockfd, m_log);

  while(1) {
    try {
      m_failoverMonitor->connectToAlternative();
      connect();
      break;
    } catch(std::exception &ex) {
      *m_log << MSG::WARNING << WHERE  << ex.what() << endmsg;
    }
    sleep(1);
  }

  m_ackThread->reInit(m_sockfd);  /*Reinit ack thread's data.*/
  m_sendThread->reInit(m_sockfd);  /*Reinit send thread's data.*/

  /*
   * Start up the threads that we stopped.
   */
  if(currThread == FAILOVER_THREAD) {
    m_ackThread->start();
    m_sendThread->start();
  } else if(currThread == ACK_THREAD) {
    m_failoverMonitor->start();
    m_sendThread->start();
  } else if(currThread == SEND_THREAD) {
    m_failoverMonitor->start();
    m_ackThread->start();
  }

  l_log << MSG::WARNING << WHERE
         << "Successfully failed over. "
         << endmsg;
  pthread_mutex_unlock(&m_failoverLock);

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
 *      determined from header in case of a WRITE command, and from a size of for a close command.
 */
void Connection::sendCommand(struct cmd_header *header, void *data)
{
  static int failureCnt=0;
  MsgStream l_log(*m_log);

  struct cmd_header *newHeader;
  int totalSize = 0;
  switch(header->cmd) {
    case CMD_WRITE_CHUNK:
      totalSize += header->data.chunk_data.size;
    case CMD_CLOSE_FILE:
      m_state = STATE_CONN_OPEN; //XXX principle ok for the totalSize summing but the m_State value is then incorrect on close ...
    case CMD_OPEN_FILE:
      m_state = STATE_FILE_OPEN;
      totalSize += sizeof(struct cmd_header);
      break;
  }
  newHeader = NULL;
  do {
      newHeader = m_mmObj.allocAndCopyCommand(header, data);  /* Will always succeed. */
      if(newHeader == NULL) {
          if(failureCnt%60==0 && failureCnt!=0) {
//              *m_log << MSG::FATAL << "Buffer is full!, allocated bytes=" <<m_mmObj.getAllocByteCount() << ", MaxQueueSizeBytes=" << m_maxQueueSize
              l_log << MSG::WARNING << "Writing an event was delayed due to back pressure from the writerd. Performance drop -> /clusterlogs/services/writerd.log" << endmsg;
          }
          failureCnt ++;
          sleep(1);
      }
  } while(newHeader == NULL && m_stopRetrying == 0);
  if(m_stopSending == 1) {
      // do a clean shutdown
      // m_mmObj and its buffer will be deleted when the
      // connection object will be deleted
      l_log << MSG::FATAL << m_mmObj.getAllocByteCount() << " Bytes are lost." << endmsg;
      return;
  }
  if(newHeader != NULL) {
	  if(header->cmd == CMD_CLOSE_FILE)
		  l_log << MSG::INFO << WHERE << "Begin: enqueue stop command for file: " << header->file_name << endmsg;
      m_mmObj.enqueueCommand(newHeader);
      if(header->cmd == CMD_CLOSE_FILE)
    	  l_log << MSG::INFO << WHERE << "End: enqueue stop command for file: " << header->file_name << endmsg;
      if(failureCnt >= 30) {
          l_log << MSG::WARNING << "Event written after " << failureCnt << " second(s)." << endmsg;
      }
      failureCnt=0;
  } else {
        // Should normally never happen
        l_log << MSG::FATAL << "ERROR: Buffer is still full, giving up. Event is lost!" << endmsg;
    }
}

void Connection::stopRetrying() {
    m_stopRetrying = 1;
}

#endif /* _WIN32 */
