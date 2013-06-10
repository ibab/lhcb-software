#ifndef FAILOVERMONITOR_
#define FAILOVERMONITOR_

#include <netinet/in.h>
#include <pthread.h>

#include <list>

#include "GaudiKernel/MsgStream.h"
#include "Writer/failover_structs.h"

namespace LHCb {

  class Connection;

  /**@class NodeState
   * A wrapper over struct nodestate, to make it more easily usable
   * with self-sorting lists.
   */
  class NodeState {
    public:
      struct nodestate state;
      struct sockaddr_in addr;

      /**
       * Overloaded for sorting based on minimum loads.
       */
      bool operator <(NodeState &ns) {
        if(state.n_load.l_connections < ns.state.n_load.l_connections)
          return true;
        return false;
      }
  };

  /**@class FailoverMonitor
   * Abstracts away the logic of talking to the failover service
   * and keeping track of the load on each of the servers. To summarize,
   * this class encapsulates the following functionality.
   * 1.  Connect to the server, and get an initial list of possible servers
   *     to connect to.
   * 2.  Provide a preferred address to connect to in case a node has failed.
   *     In case the node is in "failed-over" mode, then monitor the failed
   *     node to see if it comes up, and connect to it when it does.
   * NOTE: The failover service need not necessarily always be connected
   * to the same server as the writer. The failover service does not interfere
   * with the writer service.
   */
  class FailoverMonitor {

    private:

      std::list<NodeState*> m_nodeStates;    /**<< A list of all the nodes discovered.*/
      std::list<NodeState*>::iterator m_currState; /**<< Marks current position.*/
      pthread_t m_monitorThread;      /**<< The thread handle.*/
      volatile bool m_stopUrgently;      /**<< Tells the thread to start or stop.*/
      Connection *m_conn;              /**<< The associated connection object.*/
      int m_sockFd;                    /**<< A socket to connect to the service.*/
      MsgStream *m_log;                /**<< For logging.*/
      struct sockaddr_in m_initAddr;  /**<< The address of the initial server.*/
      struct sockaddr_in m_currAddr;  /**<< The current address connected to.*/

    public:
      /**
       * Constructor.
       * @param serverAddr An initial server to connect to.
       * @param serverPort The port of the initial server which is contacted.
       * @param conn The connection object to associate this failover object with.
       */
      FailoverMonitor(std::string & serverAddr, int serverPort, Connection *conn, MsgStream *log);

      virtual ~FailoverMonitor();

      /**
       * Starts the failover thread.
       */
      void start(void);

      /**
       * Returns the most preferred address to connect to.
       */
      void getAddress(struct sockaddr_in *saddr_in);

      /**
       * Connects to the currently selected address and
       * obtains a list of alternative addresses.
       */
      int connect(std::list<NodeState*> &nodeStates);

      /**
       * Connects to one of the alternative servers.
       */
      void connectToAlternative(void);

      /**
       * Updates the status of nodes.
       */
      void update(struct failover_msg *fmsg, struct nodestate *nstate);

      /**
       * Get initial address list from the service.
       */
      int getAddressList(std::list<NodeState*> &nodeStates);

      /**
       * Listen for changes of servers coming up and down.
       */
      void listenForUpdates();

      /**
       * Cleans up all the node information.
       */
      void cleanAllNodeStates();

      /**
       * Repositions the iterator at the location of the currently
       * connected address in the list.
       */
      void restoreIterator();

      /**
       * Stops the failover thread.
       */
      void stopUrgently(void);
  };
}

#endif /*FAILOVERMONITOR_*/
