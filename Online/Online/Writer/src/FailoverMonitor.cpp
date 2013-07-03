#ifdef BUILD_WRITER

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <stdexcept>

#include "GaudiKernel/MsgStream.h"
#include "Writer/FailoverMonitor.h"
#include "Writer/Connection.h"
#include "Writer/Utils.h"

#define MODFAILOVER 1
#define NUM_NAME_RETRIES 10

using namespace LHCb;

/**
 * Constructor for FailoverMonitor.
 * The connection to the initial server is established in the constructor,
 * and the object can be created only if the initial server can be connected,
 * and a list of other servers can be obtained from it.
 * @param serverAddr An initial server to connect to.
 * @param serverPort The port of the initial server which is contacted.
 * @param conn The connection object to associate this failover object with.
 */
FailoverMonitor::FailoverMonitor(std::string &serverAddr, int serverPort,
  Connection *conn, MsgStream *log)
{
  int ret = -1;
  m_log = new MsgStream(*log);
  m_conn = conn;
  m_stopUrgently = false;
  m_currAddr.sin_family = AF_INET;
  m_currAddr.sin_port = htons(serverPort);

  for(int i=0;i<NUM_NAME_RETRIES;i++) {
    ret = Utils::nameLookup(serverAddr, &m_currAddr, log);
    if(ret == 0)
      break;
    *log << MSG::ERROR << "Writer " << getpid()
         << "Name lookup failed for " << serverAddr
         << endmsg;
  }

  memcpy(&m_initAddr, &m_currAddr, sizeof(struct sockaddr_in));

  if(ret != 0) {
    throw std::runtime_error("Could not look up host name.");
  }
  if(connect(m_nodeStates) != 0) {
    throw std::runtime_error("Could not get address list.");
  }
  m_currState = m_nodeStates.begin();
}

FailoverMonitor::~FailoverMonitor() {
    delete m_log;
    shutdown(m_sockFd, SHUT_RDWR);
    close(m_sockFd);

}

/**
 * Makes a connection to a specified address, and obtains a list of
 * addresses from it.
 * @return 0 in case of success, -1 in case of failure.
 */
int FailoverMonitor::connect(std::list<NodeState*> &nodeStates)
{
  int ret;
  m_sockFd = Utils::connectToAddress(&m_currAddr,
    Utils::DEFAULT_BUF_SIZE, Utils::DEFAULT_BUF_SIZE, m_log);

  if(m_sockFd < 0) {
    *m_log << MSG::WARNING << "Could not connect to host "
           << std::hex << ntohl(m_currAddr.sin_addr.s_addr) << endmsg;
    throw std::runtime_error("Could not connect to host");
  }

  /* Let's get the list of hosts now. */
  ret = getAddressList(nodeStates);
  if(ret < 1)
    return -1;
  return 0;
}

/**
 * Connect to one of the alternative servers.
 */
void FailoverMonitor::connectToAlternative(void)
{

  while(1) {

    std::list<NodeState*> nodeStates;

    /*We're already on m_currState, so we move to the next one and try connecting.*/
    *m_log << MSG::INFO <<"The total number of alternative machines is: "<< m_nodeStates.size() << endmsg; ;
    if(m_nodeStates.size() == 0) {
      *m_log << MSG::WARNING << "No failover nodes found!" << endmsg;
      throw std::runtime_error("Could not get next failover address: none does exist.");  /*Nothing else.*/
    }

    m_currState++;
//    *m_log << MSG::INFO << "m_currState address: " << m_currState->state.n_ipaddr << endmsg;
    if(m_currState == m_nodeStates.end())
      m_currState = m_nodeStates.begin();

    getAddress(&m_currAddr);
    /*Now that we have a list, we can replace our old one.*/
    if(connect(nodeStates) == 0) {
      cleanAllNodeStates();
      m_nodeStates = nodeStates;
      break;
    }

    /*Sleep for a second between retries.*/
    sleep(1);
  }
  m_currState = m_nodeStates.begin();

}

/**
 * Deletes all node states that are allocated so far.
 */
void FailoverMonitor::cleanAllNodeStates()
{
  std::list<NodeState*>::iterator nlIterator;
  nlIterator = m_nodeStates.begin();
  while(nlIterator != m_nodeStates.end()) {
    NodeState *state = *nlIterator;
    delete state;
    nlIterator++;
  }
  m_nodeStates.erase(m_nodeStates.begin(), m_nodeStates.end());
}

/**
 * Connects to the failover service and gets a list of servers to fail over to.
 * @param nodeStates A list of nodes that were obtained from the server. This is not read
 * directly into the m_nodeStates. In case getAddressList() is being called to
 * read in a list from a server that we have just failed over to, and a problem
 * occurs while reading, we may end up with no nodes in the list. So until a
 * full list can be read from a server, the existing list must not be cleaned.
 * @return The number of addresses that could be obtained.
 */
int FailoverMonitor::getAddressList(std::list<NodeState*> &nodeStates)
{
  struct failover_msg fmsg;
  int ret;
  unsigned int i;
  int numHosts = 0;

#if MODFAILOVER
  /* BEGIN: Modifications to Failover*/
  unsigned int *storenodeIP = NULL;
  int numstorenodeIPs = 0;
  struct hostent *storenodelist;
  int gethosterrno = 0;
  const char *storenodesname = "storewrt";
  do
  {
	  *m_log << MSG::INFO << WHERE << "Getting IP addresses for storewrt.lbdaq.cern.ch " << endmsg;

	  storenodelist = gethostbyname(storenodesname);
	  if (storenodelist == NULL)
	  {
		  gethosterrno = (int)h_errno;
		  switch (gethosterrno)
		  {
		  case HOST_NOT_FOUND:
			  *m_log << MSG::ERROR << "Unknown host: storewrt.lbdaq.cern.ch" << endmsg;
			  break;
//		  case NO_ADDRESS:
		  case NO_DATA:
			  *m_log << MSG::ERROR << "storewrt.lbdaq.cern.ch does not have an IP address" << endmsg;
			  break;
		  case NO_RECOVERY:
			  *m_log << MSG::ERROR << "Non-recoverable nameserver error" << endmsg;
			  break;
		  case TRY_AGAIN:
			  *m_log << MSG::ERROR << "Temporary nameserver error: Will retry" << endmsg;
			  break;
		  default:
			  break;
		  }
	  }
  } while (storenodelist == NULL && gethosterrno == TRY_AGAIN);

  if(storenodelist == NULL)
	  return 0;


  while ((storenodeIP = (unsigned int *)storenodelist->h_addr_list[numstorenodeIPs++]) != NULL)
	  {
	  NodeState *nState = new NodeState();

//	  inet_aton(storenodeIP, (in_addr *)(&(nState->state.n_ipaddr)));
	  nState->state.n_ipaddr = *storenodeIP;
	  nState->state.n_id = numstorenodeIPs;
	  nState->state.n_last_ka = 0xFFFF;
	  nState->state.n_load.l_connections = 0xFFFF;

	  *m_log << MSG::INFO << WHERE << "Store node IP: " <<  IP(nState->state.n_ipaddr) << endmsg;

	  if(nState->state.n_ipaddr == m_currAddr.sin_addr.s_addr)
		  nodeStates.push_front(nState);
	  else
		  nodeStates.push_back(nState);
  }

  return (numstorenodeIPs -1);

  /* END: Modifications to Failover*/
#endif

  BIF recvBif(m_sockFd, &fmsg, sizeof(struct failover_msg));
  ret = recvBif.nbRecvTimeout();

  if(ret != sizeof(struct failover_msg))
    throw std::runtime_error("Could not receive nodelist count from server.");

  for(i=0;i<fmsg.num_nodes;i++) {
    NodeState *nState = new NodeState();
    BIF recvBif(m_sockFd, &nState->state, sizeof(struct nodestate));
    ret = recvBif.nbRecvTimeout();

    if(ret != sizeof(struct nodestate)) {
      /*Free everything alloced so far, and get out.*/
      std::list<NodeState*>::iterator ni;
      for(ni = nodeStates.begin(); ni != nodeStates.end(); ni++) {
        NodeState *ns = *ni;
        *m_log << MSG::INFO << "node list address: " << IP((*ni)->state.n_ipaddr) << endmsg;
        delete ns;
      }
      delete nState;
      cleanAllNodeStates();
      return 0;
    }

    *m_log << MSG::INFO << WHERE
           << " Read Server Addr = " << IP(nState->state.n_ipaddr) << endmsg;

    //If it's the current host, push it to the top.
    if(nState->state.n_ipaddr == m_currAddr.sin_addr.s_addr)
      nodeStates.push_front(nState);
    else
      nodeStates.push_back(nState);

    numHosts++;
  }
  return numHosts;
}

/**
 * The failover thread body.
 */
static void *failover_thread(void *args)
{
  Utils::blockSignals();

  FailoverMonitor *fm = (FailoverMonitor*)args;
  fm->listenForUpdates();
  return NULL;
}

void FailoverMonitor::start(void)
{
  int ret;
  m_stopUrgently = false;
  ret = pthread_create(&m_monitorThread, NULL, failover_thread, this);
  if(ret != 0) {
    throw std::runtime_error("Could not start the failover service thread.");
  }
  *m_log << MSG::INFO << WHERE << " Failover thread started." << endmsg;
}

void FailoverMonitor::listenForUpdates(void)
{
  int ret;
  struct failover_msg fmsg;

  while(!m_stopUrgently) {
    /*
     * Listen for new discovery message structs, which will
     * indicate that a server has either joined or left.
     */
    BIF recvBif(m_sockFd, &fmsg, sizeof(struct failover_msg));
    ret = recvBif.nbRecv(m_log);
    if(ret == BIF::DISCONNECTED) {
      if(m_conn->failover(FAILOVER_THREAD) == KILL_THREAD)
        break;
      else
        continue;
    } else if(ret == BIF::AGAIN || ret == BIF::TIMEDOUT) {
      continue;
    }

    /*Let's receive each one of the advertised nodes.*/
    int die = 0;
    struct nodestate nstate;
    *m_log << MSG::INFO << WHERE << "Failover thread expects update about "
           << fmsg.num_nodes << " node(s) in state " << (fmsg.msg_type == 1 ? "joined":"left")
           << endmsg;
    for(unsigned int i=0;i<fmsg.num_nodes;i++) {
      BIF recvBif(m_sockFd, &nstate, sizeof(struct nodestate));
      ret = recvBif.nbRecvTimeout();
      if(ret == BIF::DISCONNECTED || ret == BIF::TIMEDOUT) {
        if(m_conn->failover(FAILOVER_THREAD) == KILL_THREAD)
          die = 1;
        break;
      }
      *m_log << MSG::INFO << WHERE << "Received update for node: " << IP(nstate.n_ipaddr) << " Identifier is: " << nstate.n_id << endmsg;
      update(&fmsg, &nstate);
    }
    if(die)
      break;
  }
}

void FailoverMonitor::update(struct failover_msg *fmsg, struct nodestate *nstate)
{
  std::list<NodeState*>::iterator ni;
  for(ni=m_nodeStates.begin();ni!=m_nodeStates.end();ni++) {
    NodeState *ns = *ni;
    if((ns->state.n_ipaddr == nstate->n_ipaddr) && fmsg->msg_type == NODE_JOINED) {
      /*Already existing node, someone joined. Update load.*/
      memcpy(&ns->state.n_load, &nstate->n_load, sizeof(struct load));
      m_nodeStates.sort();
      restoreIterator();
      return;
    } else if((ns->state.n_id == nstate->n_id) && fmsg->msg_type == NODE_LEFT) {

      /*Remove this node from here.*/
      m_nodeStates.erase(ni);
      restoreIterator();
      delete ns;
      return;
    }
  }

  /*We have new node information.*/
  if(fmsg->msg_type == NODE_JOINED) {
    /*Doesn't exist, add it.*/
    NodeState *nState = new NodeState();
    memcpy(&nState->state, nstate, sizeof(struct nodestate));
    if(nstate->n_ipaddr == m_initAddr.sin_addr.s_addr) {
      /*Prioritize this node to trigger a failback, if it was initial node.*/
      m_nodeStates.push_front(nState);
    } else {
      /*Just add, and reorder based on load.*/
      m_nodeStates.push_back(nState);
      m_nodeStates.sort();
    }
  }
  restoreIterator();
}

/**
 * Sets the iterator to the current address in the list.
 * When an address is added or removed from the list, then the iterator
 * becomes invalid. The idea is to reposition the iterator at the location
 * of the currently connected address.
 */
void FailoverMonitor::restoreIterator()
{
  m_currState = m_nodeStates.begin();
  while(m_currState!=m_nodeStates.end()) {
    NodeState *curr = *m_currState;
    if(curr->state.n_ipaddr == m_currAddr.sin_addr.s_addr)
      return;
    m_currState++;
  }
  m_currState = m_nodeStates.begin();
}

void FailoverMonitor::getAddress(struct sockaddr_in *saddr_in)
{
  NodeState *ns;
  ns = *m_currState;

  saddr_in->sin_family = AF_INET;
  saddr_in->sin_port = m_currAddr.sin_port;
  saddr_in->sin_addr.s_addr = ns->state.n_ipaddr;
}

void FailoverMonitor::stopUrgently(void)
{
  m_stopUrgently = true;
  pthread_join(m_monitorThread, NULL);
}

#endif
