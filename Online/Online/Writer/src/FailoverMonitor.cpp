
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdexcept>

#include "GaudiKernel/MsgStream.h"

#include "Writer/FailoverMonitor.h"
#include "Writer/Connection.h"
#include "Writer/Utils.h"


#define NUM_NAME_RETRIES 10

using namespace LHCb;

/**
 * Constructor.
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

	m_initAddr.sin_family = AF_INET;
	m_initAddr.sin_port = htons(serverPort);

	for(int i=0;i<NUM_NAME_RETRIES;i++) {
		ret = Utils::nameLookup(serverAddr, &m_initAddr, log);
		if(ret == 0)
			break;
		*log << MSG::ERROR << "Name lookup failed for " << serverAddr << endmsg;
	}
	if(ret != 0) {
		throw std::runtime_error("Could not look up host name.");
	}

	m_sockfd = Utils::connectToAddress(&m_initAddr,
		Utils::DEFAULT_BUF_SIZE, Utils::DEFAULT_BUF_SIZE, log);
	if(m_sockfd < 0) {
			*log << MSG::INFO << "Couldn't connect to " << (m_initAddr.sin_addr.s_addr & 0xff)  <<
			((m_initAddr.sin_addr.s_addr & 0xff00) >> 8)  <<
			((m_initAddr.sin_addr.s_addr & 0xff0000) >> 16)
			<< ((m_initAddr.sin_addr.s_addr & 0xff000000) >> 24) << endmsg;
		throw std::runtime_error("Could not connect to initial host.");
	}
	getAddressList();
}

/**
 * Deletes all node states that are allocated so far.
 */
void FailoverMonitor::cleanAllNodeStates()
{
	std::list<NodeState*>::iterator nlIterator;
	nlIterator = nodeStates.begin();
	while(nlIterator != nodeStates.end()) {
		NodeState *state = *nlIterator;
		delete state;
		nlIterator++;
	}
}

/**
 * Connects to the failover service.
 */
void FailoverMonitor::getAddressList()
{
	struct failover_msg fmsg;
	int ret;
	unsigned int i;

	ret = Utils::brecv(m_sockfd, &fmsg, sizeof(struct failover_msg), m_log);
	*m_log << MSG::INFO << "ERRONO = " << errno << endmsg;
	if(ret < 0)
		throw std::runtime_error("Could not receive nodelist count from server.");

	*m_log << MSG::INFO << "Server count = " << fmsg.num_nodes << endmsg;

	for(i=0;i<fmsg.num_nodes;i++) {
		NodeState *nState = new NodeState();
		ret = Utils::brecv(m_sockfd, &nState->state, sizeof(struct nodestate), m_log);

		*m_log << MSG::INFO << "Got from service: " << (nState->state.n_ipaddr & 0xff)  <<
			((nState->state.n_ipaddr & 0xff00) >> 8)  <<
			((nState->state.n_ipaddr & 0xff0000) >> 16)
			<< ((nState->state.n_ipaddr & 0xff000000) >> 24) << endmsg;

		if(ret < 0) {
			delete nState;
			cleanAllNodeStates();
			throw std::runtime_error("Could not receive all nodes from server.");
		}
		nodeStates.push_back(nState);
	}
	currState = nodeStates.begin();
}

/**
 * The failover thread body.
 */
static void *failover_thread(void *args)
{
	FailoverMonitor *fm = (FailoverMonitor*)args;
	while(1) {
		fm->getAddressList();
	}
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

void FailoverMonitor::getNextAddress(struct sockaddr_in *saddr_in)
{
	NodeState *ns;
	if(currState == nodeStates.end())
		currState = nodeStates.begin();
	if(currState == nodeStates.end())
		throw std::runtime_error("Could not get next address:empty.");	/*Nothing else.*/

	ns = *currState;

	saddr_in->sin_family = AF_INET;
	saddr_in->sin_port = m_initAddr.sin_port;
	saddr_in->sin_addr.s_addr = ns->state.n_ipaddr;
	currState++;
}

void FailoverMonitor::stop(void)
{
	m_stopThread = 1;
	pthread_join(m_monitorThread, NULL);
}
