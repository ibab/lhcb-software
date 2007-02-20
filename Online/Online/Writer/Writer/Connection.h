

#ifndef NETWORK_H_
#define NETWORK_H_

#define MAX_RETRIES	4

extern "C" {

#include <netinet/in.h>
#include "Writer/chunk_headers.h"
}

#include "Writer/MM.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

/*
 * LHCb Namespace
 */

namespace LHCb {

  class INotifyClient;

  /**@class Connection
    * Abstracts away the networking / failover part of the MDF
    * Writer.
    */
  class Connection
  {
    private:

      /// Flag to tell the acking thread to shut down.
      volatile int m_stopAcking;

      /// A handle to the ack thread.
      pthread_t m_ackThread;

      /// A client to be notified.
      INotifyClient *m_notifyClient;

      /// One of the Connection::STATE_XXX values.
      int m_state;

      /// The address of the server to connect to.
      std::string m_serverAddr;

      /// The port of the server to connect to.
      int m_serverPort;

      /// The destination address in a struct.
      struct sockaddr_in m_destAddr;

      /// A counter to assign sequence numbers to messages.
      unsigned int m_seqCounter;

      /// An MM object to manage memory.
      MM m_mmObj;

      /// The currently open socket to the server.
      int m_sockfd;

      /// A buffer in which acknowledgement packets can be received.
      struct ack_header m_ackHeaderBuf;

      /// A count of how many bytes are in m_ackHeaderBuf
      int m_ackHeaderReceived;

      /// A MessageSvc object to log messages to.
      MsgStream *m_log;

      /// Fails over onto an alternative storage cluster node.
      void failover(void);

      /// Starts the acknowledgement thread.
      void startAckThread(void);

      /// Stops the acknowledgement thread.
      void stopAckThread(void);


    public:

      /// The connection is closed, no files are open.
      static const int STATE_CONN_CLOSED	=	0x01;
      /// The connection is open, and a file is open.
      static const int STATE_FILE_OPEN	=	0x02;
      /// The connection is open, but no file is open.
      static const int STATE_CONN_OPEN	=	0x03;

      /// Processes acknowledgements and dequeues the acknowledged commands.
      int processAcks(int blocking);

      /// Connects to a storage cluster node.
      void connectAndNegotiate(std::string serverAddr, int serverPort,
	  int soTimeout, int sndRcvSizes, MsgStream *log, INotifyClient *nClient);

      /// Closes the connection to the storage cluster node.
      void closeConnection(void);

      /// Sends a command and queues it for retries in case of failover.
      void sendCommand(struct cmd_header *cmd, void *data);

      /// Same as sendCommand(cmd_header, NULL);
      void sendCommand(struct cmd_header *cmd);

      /// Returns the state in which the Connection is.
      int getState() { return m_state; }

      /// Sets a notification listener for events on this connection.
      void setNotifyClient(INotifyClient *nClient) { m_notifyClient = nClient; }
  };

  /** @class An interface that can be used to register for notifications.
    * This interface must be implemented by any class that wants to receive
    * notifications of completions or errors in any of the send requests.
    */
  class INotifyClient
  {
    public:
      /// Called when the Connection object is notified of an error.
      virtual void notifyError(struct cmd_header *header, int errno) = 0;
      /// Called when the Connection object is notified of a successful close.
      virtual void notifyClose(struct cmd_header *header) = 0;
      /// Called when the Connection object is notified of a successful open.
      virtual void notifyOpen(struct cmd_header *header) = 0;
  };

}

#endif /*NETWORK_H_*/
