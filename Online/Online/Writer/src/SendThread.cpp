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
 * The sender thread body.
 * This thread just starts the startSending() method of
 * the specified connection object in a separate thread.
 */
static void *send_thread(void *args)
{
  Utils::blockSignals();

  SendThread *st;
  st = (SendThread*)args;
  st->processSends();
  return NULL;
}

/**
 * Starts up the send thread.
 */
void SendThread::start()
{
  m_stopUrgently = false;

  int ret = pthread_create(&m_sendThread, NULL, send_thread, this);
  if(ret != 0) {
    *m_log << MSG::FATAL << "Could not create send thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Writer " << getpid()
         <<" Send thread started" << endmsg;
}

/**
 * Reinits the structures that the send thread uses,
 */
void SendThread::reInit(int sockFd) {
  m_sockFd = sockFd;
  m_mmObj->resetSendPointer();
}

/**
 * Stops the send thread (blocking).
 */
void SendThread::stopUrgently()
{
    m_stopUrgently = true;
    stop();
}

void SendThread::stopAfterFinish()
{
    m_stopAfterFinish = true;
}

void SendThread::stop(void) {

  int ret = pthread_join(m_sendThread, NULL);
  if(ret != 0) {
    *m_log << MSG::ERROR << "Could not stop send thread " << errno << endmsg;
    return;
  }
  *m_log << MSG::INFO << "Writer " << getpid()
         << " Send thread stopped." << endmsg;
}

/** Processes elements from the send queue.
 * The elements from this queue are sent over a socket, one by one.
 * Note that the pdu of the close command is not sent as it is used only for failover.
 */
int SendThread::processSends(void)
{
  int ret;
  char *ptr;
  int totalSize = 0;
  cmd_header *cmd_to_send = NULL;
  BIF *bif = NULL;


  struct timespec iSleep;
  iSleep.tv_sec = 0;
  iSleep.tv_nsec = 0;

  /*While either you don't need to stop, or you need to stop after purging entries.*/
  do {

    if(!bif) {
      cmd_to_send = m_mmObj->moveSendPointer();
      if(!cmd_to_send)
        continue;

      totalSize = sizeof(struct cmd_header);
      if(cmd_to_send->cmd == CMD_WRITE_CHUNK)
        totalSize += cmd_to_send->data.chunk_data.size;
      if(cmd_to_send->cmd == CMD_CLOSE_FILE)	// For debugging and expert logging
    	  *m_log << MSG::INFO <<  WHERE << "Close command for file: " << cmd_to_send->file_name << endmsg;
      ptr = (char *)cmd_to_send;
      bif = new BIF(m_sockFd, ptr, totalSize);
    }

//    static unsigned int dbg_CountAgain =0; //XXX

    ret = bif->nbSend(m_log);
    if(ret == totalSize) {
    	if ( cmd_to_send->cmd == CMD_CLOSE_FILE)
    		 *m_log << MSG::INFO << "CLOSE CMD Send was successful for file: " << cmd_to_send->file_name << endmsg;

//      dbg_CountAgain=0;

      delete bif;
      bif = NULL;
      if(iSleep.tv_nsec > 0) {
          iSleep.tv_sec = 0;
          iSleep.tv_nsec = 0;
      }
    } else if(ret == BIF::DISCONNECTED) {
      *m_log << MSG::INFO << "send returned: DISCONNECTED " << endmsg;
      if(m_conn->failover(SEND_THREAD) == KILL_THREAD)  {
        return 0;
      } else {
        cmd_to_send = NULL;
        delete bif;
        bif = NULL;
        continue;
      }
    } else if(ret == BIF::AGAIN) {
          //*m_log << MSG::INFO << "send returned AGAIN: " << ++dbg_CountAgain  << endmsg;
//    	*m_log << MSG::INFO << "send returned: AGAIN: " << endmsg;
          nanosleep(&iSleep, NULL);
          if(iSleep.tv_sec < 1) {
              if(iSleep.tv_nsec >= 900000000) {
                  iSleep.tv_sec=1;
                  iSleep.tv_nsec = 1;
              }
              else
                  iSleep.tv_nsec += 1000;
          }
          continue; // try to send again
      }
      else {
//          *m_log << MSG::INFO << "send returned:  " << ret << endmsg;
      	}
  } while( (m_stopUrgently == false && m_stopAfterFinish == false) ||
      (m_stopUrgently == false && m_stopAfterFinish == true && cmd_to_send != NULL));

  if(bif)
    delete bif;
  if(cmd_to_send != NULL)
	  *m_log << MSG::INFO <<  WHERE << "m_stopUrgently = " << m_stopUrgently << " , m_stopAfterFinish = " << m_stopAfterFinish << " for filename = " << cmd_to_send->file_name << " :Exiting processSends()" << endmsg;
  else
	  *m_log << MSG::INFO <<  WHERE << "m_stopUrgently = " << m_stopUrgently << " , m_stopAfterFinish = " << m_stopAfterFinish << " , cmd_to_send is NULL, Exiting processSends()" << endmsg;
  return 0;
}

#endif /*BUILD_WRITER*/
