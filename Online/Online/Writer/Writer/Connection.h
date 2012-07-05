#ifndef NETWORK_H_
#define NETWORK_H_

#define MAX_RETRIES  4

extern "C" {

#include <netinet/in.h>
#include "Writer/chunk_headers.h"
}

#include "Writer/MM.h"
#include "Writer/SendThread.h"
#include "Writer/AckThread.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#define MDFWRITER_THREAD  0x01   /**<< MDF Writer GaudiAlgorithm thread identifier.*/
#define ACK_THREAD         0x02   /**<< Acknowledgement thread identifier.*/
#define SEND_THREAD       0x03   /**<< Sender thread identifier.*/
#define FAILOVER_THREAD   0x04   /**<< Failover thread identifier.*/

/**
 * Returned by Connection::failover() to tell a thread that failover is already done,
 * and that it must die to be respawned.
 */
#define KILL_THREAD    (-1)
#define RESUME_THREAD  0

/**
 * A thread local variable to keep track of which thread we're in.
 */
//extern __thread int currThread;


/*
 * LHCb Namespace
 */

namespace LHCb {

  class INotifyClient;
  class FailoverMonitor;

  /**@class Connection
   * Abstracts away the networking / failover part of the MDF
   * Writer.
   */
  class Connection
  {
    private:

      /// A handle to the sender thread.
      SendThread *m_sendThread;

      /// A handle to the ack thread.
      AckThread *m_ackThread;

      /// A failover monitor object.
      FailoverMonitor *m_failoverMonitor;

      /// Flag to tell the acking thread to shut down.
      volatile int m_stopAcking;

      /// Flag to tell the sending thread to shut down.
      volatile int m_stopSending;

      /// Flag to tell the connection object to stop
      /// retyring to send commands when the buffer is full.
      volatile int m_stopRetrying;

      /// A lock to be held by a thread that goes into failover.
      /// This is to prevent both threads from initiating failover.
      pthread_mutex_t m_failoverLock;

      /// A client to be notified.
      INotifyClient *m_notifyClient;

      /// One of the Connection::STATE_XXX values.
      volatile int m_state;

      /// The address of the server to connect to.
      std::string m_serverAddr;

      /// The port of the server to connect to.
      int m_serverPort;

      /// The destination address in a struct.
      struct sockaddr_in m_destAddr;

      /// maximum size of the queue, only used in Connection to print out this valu
      /// real use is only in m_mmObj
      size_t m_maxQueueSize;

      /// An MM object to manage memory.
      MM m_mmObj;

      /// The currently open socket to the server.
      volatile int m_sockfd;

      /// Socket buffer sizes.
      int m_sndRcvSizes;

      /// A MessageSvc object to log messages to.
      MsgStream *m_log;

      /// Starts the acknowledgement thread.
      void startAckThread();

      //Reinits data that the ack thread uses.
      void reInitAckThread();

      /// Stops the acknowledgement thread.
      void stopAckThread(int stopLevel);

      /// Starts the sender thread.
      void startSendThread();

      //Reinits data that the sender thread uses.
      void reInitSendThread();

      /// Stops the sender thread.
      void stopSendThread(int stopLevel);


    public:
      /// getter for the length of the queue of the MM object
      inline unsigned int getQueueLength() const { return this->m_mmObj.getQueueLength(); }

      /// The connection is closed, no files are open.
      static const int STATE_CONN_CLOSED  =  0x01;
      /// The connection is open, and a file is open.
      static const int STATE_FILE_OPEN  =  0x02;
      /// The connection is open, but no file is open.
      static const int STATE_CONN_OPEN  =  0x03;
      /// The connection is being failed over.
      static const int STATE_FAILING_OVER = 0x04;

      /// Fails over onto an alternative storage cluster node.
      int failover(int currThread);

      /// Processes acknowledgements and dequeues the acknowledged commands.
      int processAcks(void);

      /// Conencts to a storage cluster node, and starts threads.
      void initialize();

      /// Connect.
      void connect();

      /// Closes the connection to the storage cluster node.
      void closeConnection(void);

      /// Sends a command and queues it for retries in case of failover.
      void sendCommand(struct cmd_header *cmd, void *data);

      /// Same as sendCommand(cmd_header, NULL);
      void sendCommand(struct cmd_header *cmd);

      /// Returns the state in which the Connection is.
      int getState() { return m_state; }

      /// Stops retrying to send a command when the buffer is full.
      /// Used to stop the infinite Retry-Loop when a run shall
      /// be stopped.
      void stopRetrying();

      /// Sets a notification listener for events on this connection.
      void setNotifyClient(INotifyClient *nClient) {
        m_ackThread->setNotifyClient(nClient);
      }

      /// Notifies the notification listener.
      void notify(struct cmd_header *cmd);

      /// Constructor
      Connection(std::string serverAddr, int serverPort, int sndRcvSizes,
          MsgStream * log, INotifyClient *nClient, size_t maxQueueSize);

  };

  /** @class An interface that can be used to register for notifications.
   * This interface must be implemented by any class that wants to receive
   * notifications of completions or errors in any of the send requests.
   */
  class INotifyClient
  {
    public:
      /// Called when the Connection object is notified of an error.
      virtual void notifyError(struct cmd_header *header, int err_no) = 0;
      /// Called when the Connection object is notified of a successful close.
      virtual void notifyClose(struct cmd_header *header) = 0;
      /// Called when the Connection object is notified of a successful open.
      virtual void notifyOpen(struct cmd_header *header) = 0;
  };

}

#endif /*NETWORK_H_*/
