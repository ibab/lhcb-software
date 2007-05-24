#ifndef ACKTHREAD_H_
#define ACKTHREAD_H_

#include "Writer/Connection.h"
#include "Writer/MM.h"
#include "Writer/chunk_headers.h"
#include "GaudiKernel/MsgStream.h"

#include <pthread.h>

namespace LHCb {

  class Connection;
  class INotifyClient;
  /**
   * Takes care of purging the queue and acking chunks to the server.
   */
  class AckThread {

    private:

      Connection *m_conn;
      MsgStream *m_log;
      MM *m_mmObj;
      volatile bool m_stopUrgently;
      volatile bool m_stopAfterFinish;
      volatile int m_sockFd;
      pthread_t m_ackThread;
      INotifyClient *m_notifyClient;

    public:
      AckThread(Connection *conn, int sockfd, MM *mmObj, MsgStream *log) {
        m_conn = conn;
        m_log = new MsgStream(*log);
        m_mmObj = mmObj;
        m_sockFd = sockfd;
        m_notifyClient = NULL;
        m_stopUrgently = false;
        m_stopAfterFinish = false;
      }
      virtual ~AckThread() { delete m_log; }
      void start(void);
      void stop(void);
      void stopUrgently(void);
      void stopAfterFinish(void);
      void reInit(int sockfd);
      int processAcks(void);
      void notify(struct cmd_header *cmd);
      void setNotifyClient(INotifyClient *nl) {
        m_notifyClient = nl;
      }

  };


}

#endif /*ACKTHREAD_H_*/
