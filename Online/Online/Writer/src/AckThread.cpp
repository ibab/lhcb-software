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

#include "Writer/Connection.h"
#include "Writer/MM.h"
#include "Writer/Utils.h"

#include "GaudiKernel/MsgStream.h"

using namespace LHCb;

/**
 * The acker thread body.
 * This thread just starts the startAcking() method of
 * the specified connection object in a separate thread.
 */
static void *ack_thread(void *args)
{
  AckThread *st;
  st = (AckThread*)args;
  currThread = ACK_THREAD;
  st->processAcks();
  return NULL;
}

/**
 * Starts up the ack thread.
 */
void AckThread::start()
{
  int ret;
  *m_log << MSG::INFO << "Starting ack thread. . .";
  m_stopAcking = 0;
  ret = pthread_create(&m_ackThread, NULL, ack_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create ack thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Done." << endmsg;
}

/**
 * Reinits the structures that the ack thread uses,
 */
void AckThread::reInit(int sockfd) {
	m_sockfd = sockfd;
	*m_log << MSG::INFO << "Reset ack thread data." << endmsg;
}


/**
 * Stops the ack thread (blocking).
 */
void AckThread::stop(int stopLevel)
{
  int ret;
  *m_log << MSG::INFO << "Stopping ack thread. . .";
  m_stopAcking = stopLevel;
  ret = pthread_join(m_ackThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop ack thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Done." << endmsg;
}


inline void AckThread::notify(struct cmd_header *cmd)
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

/** Processes acknowledgements received from the server.
 * All received acknowledgements are removed from the command
 * cache. Acknowledgements are read nonblocking from the
 * server as long as there are acks to be read.
 */
int AckThread::processAcks(void)
{
  int ret;
  struct ack_header ackHeaderBuf;

start:
	memset(&ackHeaderBuf, 0, sizeof(struct ack_header));

	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopAcking) ||
  	(m_stopAcking == STOP_AFTER_PURGE && m_mmObj->getQueueLength() != 0))
  {

    ret = Utils::brecv(m_sockfd, &ackHeaderBuf, sizeof(struct ack_header), m_log);
    if(ret != sizeof(struct ack_header)) {
      *m_log << MSG::WARNING << "Disconnected, should fail over." << errno << endmsg;
      if(m_conn->failover() == KILL_THREAD)
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
      if((cmd = m_mmObj->dequeueCommand(seqNum)) == NULL) {
        *m_log << MSG::ERROR << "FATAL, Received an unsolicited ack." << endmsg;
      } else {
      	notify(cmd);
        m_mmObj->freeCommand(cmd);
      }
    }
  }
  return 0;
}
#endif /*BUILD_WRITER*/
