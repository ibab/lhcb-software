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
  SendThread *st;
  st = (SendThread*)args;
  currThread = SEND_THREAD;
  st->processSends();
  return NULL;
}

/**
 * Starts up the send thread.
 */
void SendThread::start()
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
void SendThread::reInit(int sockfd) {
	m_sockfd = sockfd;
	m_mmObj->resetSendPointer();
	*m_log << MSG::INFO << "Reset send thread data." << endmsg;
}


/**
 * Stops the send thread (blocking).
 */
void SendThread::stop(int stopLevel)
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

/** Processes elements from the send queue.
 * The elements from this queue are sent over a socket, one by one.
 */
int SendThread::processSends(void)
{
  int ret;
  char *ptr;
  int totalSize = 0;

start:
  struct cmd_header *cmd_to_send = NULL;

  *m_log << MSG::INFO << "Started send thread." << endmsg;

	/*While either you don't need to stop, or you need to stop after purging entries.*/
  while((!m_stopSending) || (m_stopSending == STOP_AFTER_PURGE && cmd_to_send)) {

		cmd_to_send = m_mmObj->moveSendPointer();
    if(!cmd_to_send)
      continue;

    totalSize = sizeof(struct cmd_header);
    if(cmd_to_send->cmd == CMD_WRITE_CHUNK)
    	totalSize += cmd_to_send->data.chunk_data.size;

    ptr = (char *)cmd_to_send;
		ret = Utils::send(m_sockfd, ptr, totalSize, m_log);
		if(ret != totalSize) {
        if(m_conn->failover() == KILL_THREAD)
        	return 0;
        else
        	goto start;
    }
    cmd_to_send = m_mmObj->moveSendPointer();
  }

  return 0;
}

#endif /*BUILD_WRITER*/
