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
  Utils::blockSignals();
  AckThread *st;
  st = (AckThread*)args;
  st->processAcks();
  return NULL;
}

/**
 * Starts up the ack thread.
 */
void AckThread::start()
{
  m_stopUrgently = false;

  int ret = pthread_create(&m_ackThread, NULL, ack_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create ack thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Writer " << getpid()
         << "Started Ack Thread." << endmsg;
}

/**
 * Reinits the structures that the ack thread uses,
 */
void AckThread::reInit(int sockfd) {
  m_sockFd = sockfd;
}

/**
 * Stops the ack thread (blocking).
 */
void AckThread::stopUrgently()
{
  m_stopUrgently = true;
  stop();
}

void AckThread::stopAfterFinish()
{
  m_stopAfterFinish = true;
}

void AckThread::stop(void)
{
  int ret = pthread_join(m_ackThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop ack thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Writer " << getpid()
         << "Stopped Ack Thread." << endmsg;
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
  BIF *bif = NULL;

start:
  memset(&ackHeaderBuf, 0, sizeof(struct ack_header));

  /*While either you don't need to stop, or you need to stop after purging entries.*/
  while( (m_stopUrgently == false && m_stopAfterFinish == false) ||
      (m_stopUrgently == false && m_stopAfterFinish == true && m_mmObj->getQueueLength() > 0))
  {
    if(!bif) {
      bif = new BIF(m_sockFd, &ackHeaderBuf, sizeof(struct ack_header));
    }

    ret = bif->nbRecv();
    if(ret == BIF::AGAIN) {
      continue;
    } else if(ret == BIF::DISCONNECTED) {
      delete bif;
      bif = NULL;
      if(m_conn->failover(ACK_THREAD) == KILL_THREAD)
        return 0;
      else
        goto start;
    }

    delete bif;
    bif = NULL;

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
      if((cmd = m_mmObj->dequeueCommand(seqNum, ackHeaderBuf.run_no)) == NULL) {
        *m_log << MSG::ERROR << "DANGER: Received an unsolicited ack, run = "
           << ackHeaderBuf.run_no
           << endmsg;
      } else {
        notify(cmd);
        m_mmObj->freeCommand(cmd);
      }
    }
  }

  if(bif)
    delete bif;
  return 0;
}
#endif /*BUILD_WRITER*/
