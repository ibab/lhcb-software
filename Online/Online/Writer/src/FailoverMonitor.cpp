#ifdef BUILD_WRITER

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>

#include <stdexcept>

#include "GaudiKernel/MsgStream.h"
#include "Writer/FailoverMonitor.h"
#include "Writer/Connection.h"
#include "Writer/Utils.h"


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
	int ret;

	m_log = log;
	m_conn = conn;

	m_currAddr.sin_family = AF_INET;
	m_currAddr.sin_port = htons(serverPort);

	for(int i=0;i<NUM_NAME_RETRIES;i++) {
		ret = Utils::nameLookup(serverAddr, &m_currAddr, log);
		if(ret == 0)
			break;
		//*log << MSG::ERROR << "Name lookup failed for " << serverAddr << endmsg;
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
			*m_log << MSG::INFO << "Couldn't connect to " << (m_currAddr.sin_addr.s_addr & 0xff)  <<
			((m_currAddr.sin_addr.s_addr & 0xff00) >> 8)  <<
			((m_currAddr.sin_addr.s_addr & 0xff0000) >> 16)
			<< ((m_currAddr.sin_addr.s_addr & 0xff000000) >> 24) << endmsg;
		throw std::runtime_error("Could not connect to host.");
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
		if(m_nodeStates.size() == 0)
			throw std::runtime_error("Could not get next address:empty.");	/*Nothing else.*/

		m_currState++;
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
	int bRead = 0;

	BIF recvBif(m_sockFd, &fmsg, sizeof(struct failover_msg));


	//*m_log << MSG::INFO << "Want to receive num_msgs." << endmsg;

	ret = recvBif.nbRecvTimeout();
	if(ret != sizeof(struct failover_msg))
		throw std::runtime_error("Could not receive nodelist count from server.");

	for(i=0;i<fmsg.num_nodes;i++) {
		NodeState *nState = new NodeState();
		//*m_log << MSG::INFO << "Received " << (i + 1) << " of " << fmsg.num_nodes << endmsg;
		bRead = 0;

		BIF recvBif(m_sockFd, &nState->state, sizeof(struct nodestate));
		ret = recvBif.nbRecvTimeout();

		if(ret != sizeof(struct nodestate)) {
			/*Free everything alloced so far, and get out.*/
			std::list<NodeState*>::iterator ni;
			for(ni = nodeStates.begin(); ni != nodeStates.end(); ni++) {
				NodeState *ns = *ni;
				delete ns;
			}
			delete nState;
			cleanAllNodeStates();
			return 0;
		}

		/**m_log << MSG::INFO << " addr = "
		<< (nState->state.n_ipaddr & 0xff)  << "."
		<< ((nState->state.n_ipaddr & 0xff00) >> 8) << "."
		<< ((nState->state.n_ipaddr & 0xff0000) >> 16)  << "."
		<<  ((nState->state.n_ipaddr & 0xff000000) >> 24)  << endmsg;*/

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
	m_stopThread = 0;
	ret = pthread_create(&m_monitorThread, NULL, failover_thread, this);
	if(ret != 0) {
		throw std::runtime_error("Could not star the failover service thread.");
	}
}

void FailoverMonitor::listenForUpdates(void)
{
	int ret;
	struct failover_msg fmsg;
	int bRead = 0;

	while(!m_stopThread) {
		/*
		 * Listen for new discovery message structs, which will
		 * indicate that a server has either joined or left.
		 */
		BIF recvBif(m_sockFd, &fmsg, sizeof(struct failover_msg));
		ret = recvBif.nbRecv();
		if(ret == BIF::DISCONNECTED) {
			bRead = 0;
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

		//*m_log << MSG::INFO << "Reading info on " << fmsg.num_nodes << "nodes" <<endmsg;
		for(unsigned int i=0;i<fmsg.num_nodes;i++) {
			bRead = 0;
			BIF recvBif(m_sockFd, &nstate, sizeof(struct nodestate));
			ret = recvBif.nbRecvTimeout();
			if(ret == BIF::DISCONNECTED || ret == BIF::TIMEDOUT) {
				if(m_conn->failover(FAILOVER_THREAD) == KILL_THREAD)
					die = 1;
				break;
			}
			update(&fmsg, &nstate);
		}

		bRead = 0;

		if(die)
			break;
	}
}

void FailoverMonitor::update(struct failover_msg *fmsg, struct nodestate *nstate)
{
	std::list<NodeState*>::iterator ni;
	int found = 0;
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
			found = 1;
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

void FailoverMonitor::stop(void)
{
	m_stopThread = 1;
	pthread_join(m_monitorThread, NULL);
}

#endif
