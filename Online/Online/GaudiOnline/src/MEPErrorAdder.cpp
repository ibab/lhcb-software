//  ============================================================
//
//  ErrorAddingSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : David Svantesson
//
//  ===========================================================
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/Lock.h"
#include "GaudiOnline/MEPErrorAdder.h"

#include "GaudiOnline/MEPRxSys.h"

// Publish DIM counters. DNS from environment
#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name,"I", & m_ ## name, sizeof(int64_t), desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size()*sizeof(int64_t), desc, this);} while(0);
#define UNPUBCNT(name) do {m_monSvc->undeclareInfo(#name, this);} while(0);

// Template for resetting counters
template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
}
// Resetting 2D arrays
template <typename T> static void reset2DCounters(T& cnt, size_t len1, size_t len2) {
  cnt.resize(len1);
  for (unsigned int i=0; i< len1; i++) {
    cnt[i].resize(len2,0);
  }
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPErrorAdder);

using namespace LHCb;

MEPErrorAdder::MEPErrorAdder(const std::string& nam, ISvcLocator* svc) 
: Service(nam, svc), m_svcState(NOT_READY), m_incidentSvc(0), m_log(msgSvc(),nam)
{

  
  declareProperty("listenerDnsNode",    m_listenerDnsNode = "localhost.localdomain"); 		//Also where we get RunInfo (ECSXX)
  declareProperty("updateFrequency",    m_updateFrequency =5);					//How often the statistics should be updated
  
  declareProperty("sumPartition",	m_sumPartition=false);			 	  	//Is this a sum over nodes in subfarm, or subfarms in partition?
  
  //Options for sum over partition 
  declareProperty("partitionName",      m_partitionName = "");	 
  //declareProperty("runInfoDnsNode",	m_runInfoDnsNode = "");					// DIM_DNS_NODE for RunInfo, if other than listenerDnsNode  

  //Options for sum over subfarm
  declareProperty("nrSubNodes",         m_nrSubNodes =4);			         	//Number of nodes per subfarm
  
  declareProperty("nSrc",               m_nSrc=1);						//Numer of TELL1 sources this partition have
  
  m_allNames = NULL;
  m_allNamesSize = 0;
  m_zero = 0;
  
}

MEPErrorAdder::~MEPErrorAdder() {

}

/** Initalization of algorithm
 *
 *  Configure and setup of counters and subscriptions
 */
StatusCode
MEPErrorAdder::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) {
    return StatusCode::FAILURE; 
  }
  if (!service("IncidentSvc", m_incidentSvc).isSuccess()) {
    m_log << MSG::ERROR << "Failed to access incident service." << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    m_incidentSvc->addListener(this, "DAQ_CANCEL");
    m_incidentSvc->addListener(this, "DAQ_ENABLE");
  }

  char temp[100];

  // HOST set by job file
  if (m_listenerDnsNode=="localhost.localdomain" && getenv("HOST")!=NULL) m_listenerDnsNode= getenv("HOST");
  // Partition name set by job file
  if (m_sumPartition && m_partitionName=="" && getenv("SET_PARTITION")!=NULL) {
	m_partitionName = getenv("SET_PARTITION");
  }
 
  // listen to DNS node
  DimClient::setDnsNode(m_listenerDnsNode.c_str());
  m_log << MSG::INFO << "Listening to DNS Node: " << m_listenerDnsNode << endmsg; 
 
  if (m_sumPartition) {
  	// We want to subscribe to when the subfarm setup for this partition changes
	sprintf(temp,"RunInfo/%s/HLTsubFarms",m_partitionName.c_str());
  	m_subsSubFarms = new DimUpdatedInfo(temp,(char *)"",this);
	
	m_log << MSG::INFO << "Summing up statistics for partition: " << m_partitionName << endmsg;

	// Get subfarms first time
	sprintf(temp,"RunInfo/%s/HLTsubFarms",m_partitionName.c_str());
	DimInfo t_subFarms(temp,(char *)"");  
	while (t_subFarms.getSize()<1) { MEPRxSys::microsleep(10000); } // Must wait until data received to avoid segfault

	readRunInfo(&t_subFarms);
  }

  //Setup all services we want to publish, through the Monitoring Svc
  m_log << MSG::DEBUG << "Setup Monitoring Svc" << endmsg;
  if (service("MonitorSvc", m_monSvc).isSuccess()) {
          if (setupCounters()) {
                  m_log << MSG::ERROR << "Failed to setup counters";
                  return StatusCode::FAILURE;
          }
  }
  else {
    m_log << MSG::ERROR << "Failed to access monitor service.";
    return StatusCode::FAILURE;
  }

  // Setup all subscriptions
  setupSubs();

 
  m_log << MSG::DEBUG << "Initialization done" << endmsg;
  m_svcState = READY;
  
  return StatusCode::SUCCESS;
}

/** read from RunInfo a list of all subfarms
 */
int
MEPErrorAdder::readRunInfo(DimInfo* dim) {

	m_log << MSG::DEBUG << "Acquiring RunInfo information about subfarms etc. in partition" << endmsg;  
 	//if (m_runInfoDnsNode.size()>0) DimClient::setDnsNode(m_runInfoDnsNode.c_str());
	//else DimClient::setDnsNode(m_listenerDnsNode.c_str());

	// Get the list of subfarms
	const char* subFarmsChar = dim->getString(); 
	int subFarmsSize = dim->getSize();	

  	m_subFarms.clear();
	if (subFarmsChar[0] == 0) {
		// No subfarms
		m_log << MSG::WARNING << "Partition " << m_partitionName << " have no subfarms" << endmsg;
		m_nrSubFarms = 0;
		return 1;
        }

	// Read all subfarms into vector
	int start = 0;
	std::string printSubfarms = "";
	int i=0;
	while (subFarmsSize>start) {    
		m_subFarms.push_back(&subFarmsChar[start]);
    		start += (m_subFarms[i].size() + 1);
		printSubfarms += m_subFarms[i] + ", ";
		i++;
  	}
	m_log << MSG::INFO << "Subfarms in partition " << m_partitionName << ": " << printSubfarms << endmsg;
	m_nrSubFarms = m_subFarms.size();

	return 0;
}

/** Finalize algorithm
 */
StatusCode
MEPErrorAdder::finalize() {

  removeSubs();

  delete m_subsSubFarms;

  if (m_monSvc) {
    m_monSvc->undeclareAll(this);
    m_monSvc->release();
    m_monSvc = 0;
  }
 
  delete[] m_allNames;
  
  m_svcState = NOT_READY;  
  return Service::finalize();
} 

/** Remove all DIM subscriptions
 *
 *  Used at finalize or
 *  for partition mode, if subfarms have changed
 */
int
MEPErrorAdder::removeSubs() {

  int size = (int) m_subsBadLenPkt.size();
  for (int i=0; i<size; i++) {
    delete m_subsBadLenPkt[i];
    delete m_subsBadPckFktPkt[i];
    delete m_subsMisPkt[i];
    delete m_subsTruncPkt[i];
    delete m_subsMultipleEvt[i];
    delete m_subsRxOct[i];
    delete m_subsRxPkt[i];
    delete m_subsRxEvt[i];
    delete m_subsRxMEP[i];
    delete m_subsTotRxOct[i];
    delete m_subsTotRxPkt[i];
    delete m_subsTotRxEvt[i];
    delete m_subsTotRxMEP[i];
    delete m_subsIncEvt[i];
    delete m_subsTotBadMEP[i];
    delete m_subsTotMEPReq[i];
    delete m_subsTotMEPReqPkt[i];
    delete m_subsNumMEPRecvTimeouts[i];
    delete m_subsNotReqPkt[i];
    delete m_subsTotWrongPartID[i];
    delete m_subsSentEvt[i];
    delete m_subsSentOct[i];
    delete m_subsSentEvtErr[i];

    delete m_subsSrcName[i];
  }	 

  m_subsBadLenPkt.clear();
  m_subsBadPckFktPkt.clear();
  m_subsMisPkt.clear();
  m_subsTruncPkt.clear();
  m_subsMultipleEvt.clear();
  m_subsRxOct.clear();
  m_subsRxPkt.clear();
  m_subsRxEvt.clear();
  m_subsTotRxOct.clear();
  m_subsTotRxPkt.clear();
  m_subsTotRxEvt.clear();
  m_subsTotRxMEP.clear();
  m_subsIncEvt.clear();
  m_subsTotBadMEP.clear();
  m_subsTotMEPReq.clear();
  m_subsTotMEPReqPkt.clear();
  m_subsNumMEPRecvTimeouts.clear();
  m_subsNotReqPkt.clear();
  m_subsTotWrongPartID.clear();
  m_subsSentEvt.clear();
  m_subsSentOct.clear();
  m_subsSentEvtErr.clear();

  m_subsSrcName.clear();

  return 0;
}

/** Help-function for receiving array counters
 *
 *  return false if not the correct service, otherwise get data and return true
 *
 *  If the received data size does not correspond to the number of sources, 
 *  resetPublishedServices is called, which will republish all services with the 
 *  correct size
 *
 */
bool
MEPErrorAdder::ReceiveArrayService(DimInfo * curr, DimInfo * subs,  std::vector<int64_t> &rArray, std::vector<int64_t> &sArray) {

  // Return false if not the correct service
  if (curr != subs) return false;

  // If new size of data, we have to resize all published counters.
  int arraySize = curr->getSize()/sizeof(int64_t);
  if (arraySize != m_nSrc) { 
	resetPublishedServices(arraySize);
	m_log << MSG::INFO << " Received from " << subs->getName() << endmsg;
  }

  int64_t * data = (int64_t*) curr->getData();
  int64_t diff;

  // Receive all data
  for (int j=0; j<arraySize; j++) {
    diff = data[j] - rArray[j];	// Value to add/subt
    rArray[j] = data[j];	// Save value
    sArray[j] += diff;  	// Change value
  }

  return true;
}

/** Help-function for receiving single counters
 *
 *  return false if not the correct service, otherwise get data and return true
 */
bool
MEPErrorAdder::ReceiveSingleService(DimInfo * curr, DimInfo * subs, int64_t &rValue, int64_t &sValue) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int64_t data = *((int64_t*) curr->getData());
  int64_t diff = data - rValue;	//Value to add/subtract
  
  rValue = data;	// saved value
  sValue += diff;	// Change value

  return true;

}

/** Help-function for receiving single counters, 32bit! 
 * (DiskWR)
 *
 *  return false if not the correct service, otherwise get data and return true
 */
bool
MEPErrorAdder::ReceiveSingleService_32(DimInfo * curr, DimInfo * subs, int64_t &rValue, int64_t &sValue) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int32_t data = *((int32_t*) curr->getData());
  int64_t diff = data - rValue;	//Value to add/subtract
  
  rValue = data;	// saved value
  sValue += diff;	// Change value

  return true;

}

/** DimInfoHandler function
 */
void
MEPErrorAdder::infoHandler() {
  DimInfo * curr = getInfo();
 
  m_log << MSG::VERBOSE << "Receiving data from " << curr->getName() << endmsg;

  // If subfarms have changed, update and re-setup subscriptions
  if (curr==m_subsSubFarms) {
    changeSubFarms(m_subsSubFarms);
    return;
  }

  //To know what service, we must iterate over all subscriptions...
  for (int i=0;i<m_nrServices; i++) {
    // These array counters can change size, if the number of TELL1 sources changes
    if (ReceiveArrayService(curr,m_subsBadLenPkt[i], m_rBadLenPkt[i], m_badLenPkt)) break;
    if (ReceiveArrayService(curr,m_subsBadPckFktPkt[i], m_rBadPckFktPkt[i], m_badPckFktPkt)) break;
    if (ReceiveArrayService(curr,m_subsMisPkt[i], m_rMisPkt[i], m_misPkt)) break;
    if (ReceiveArrayService(curr,m_subsTruncPkt[i], m_rTruncPkt[i], m_truncPkt)) break;
    if (ReceiveArrayService(curr,m_subsMultipleEvt[i], m_rMultipleEvt[i], m_multipleEvt)) break;
    if (ReceiveArrayService(curr,m_subsRxOct[i], m_rRxOct[i], m_rxOct)) break;
    if (ReceiveArrayService(curr,m_subsRxPkt[i], m_rRxPkt[i], m_rxPkt)) break;
    if (ReceiveArrayService(curr,m_subsRxEvt[i], m_rRxEvt[i], m_rxEvt)) break;
    if (ReceiveArrayService(curr,m_subsRxMEP[i], m_rRxMEP[i], m_rxMEP)) break;
   
    // These will not change
    if (ReceiveSingleService(curr,m_subsTotRxOct[i], m_rTotRxOct[i], m_totRxOct)) break;    
    if (ReceiveSingleService(curr,m_subsTotRxPkt[i], m_rTotRxPkt[i], m_totRxPkt)) break;    
    if (ReceiveSingleService(curr,m_subsTotRxEvt[i], m_rTotRxEvt[i], m_totRxEvt)) break;
    if (ReceiveSingleService(curr,m_subsTotRxMEP[i], m_rTotRxMEP[i], m_totRxMEP)) break;
    if (ReceiveSingleService(curr,m_subsIncEvt[i], m_rIncEvt[i], m_incEvt)) break;    
    if (ReceiveSingleService(curr,m_subsTotBadMEP[i], m_rTotBadMEP[i], m_totBadMEP)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReq[i], m_rTotMEPReq[i], m_totMEPReq)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReqPkt[i], m_rTotMEPReqPkt[i], m_totMEPReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsNumMEPRecvTimeouts[i], m_rNumMEPRecvTimeouts[i], m_numMEPRecvTimeouts)) break;    
    if (ReceiveSingleService(curr,m_subsNotReqPkt[i], m_rNotReqPkt[i], m_notReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsTotWrongPartID[i], m_rTotWrongPartID[i], m_totWrongPartID)) break;    
 
    if (m_sumPartition) { 
        if (ReceiveSingleService(curr,m_subsSentEvt[i], m_rSentEvt[i], m_sentEvt)) break;
        if (ReceiveSingleService(curr,m_subsSentOct[i], m_rSentOct[i], m_sentOct)) break;
        if (ReceiveSingleService(curr,m_subsSentEvtErr[i], m_rSentEvtErr[i], m_sentEvtErr)) break;
    } else {
        if (ReceiveSingleService_32(curr,m_subsSentEvt[i], m_rSentEvt[i], m_sentEvt)) break;
        if (ReceiveSingleService_32(curr,m_subsSentOct[i], m_rSentOct[i], m_sentOct)) break;
        if (ReceiveSingleService_32(curr,m_subsSentEvtErr[i], m_rSentEvtErr[i], m_sentEvtErr)) break;         
    }

    if (curr == m_subsSrcName[i]) {
	//Receiving source names
	
	int size = curr->getSize();
        // Now only look if size have changed, need to go through whole list?
        if (size!=m_allNamesSize) {
		//Source names seem to have changed, remove...
		if (m_allNamesSize!=0) m_monSvc->undeclareInfo("srcName", this);
		delete[] m_allNames;
		m_allNames = NULL;		

		if (size>0) {
			// Publish new source-names
			m_allNamesSize=size;
			if (!(m_allNames = new char[size+1])) {
				m_log << MSG::ERROR << "Failed to store source names" << endmsg;
				return;
			}
   		
			::memcpy(m_allNames, (const char *) curr->getData(), size);
		
			m_monSvc->declareInfo("srcName", "C", m_allNames, size, "Source IP names", this);
		}
	}
    }

  }

}

StatusCode MEPErrorAdder::run() {

  while (m_svcState != RUNNING) {
    switch(m_svcState) {
    case STOPPED:
    case NOT_READY:
      m_log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
      //continue;
      return StatusCode::SUCCESS;
    case READY:
      MEPRxSys::microsleep(100000); // 100 ms
      break;
    default: continue;
    }
  }

  for (;;) {
    if (m_svcState != RUNNING) {
      //Do something here?
      
      return StatusCode::SUCCESS;
    }
    MEPRxSys::microsleep(100000); // 100 ms
  }

  return 1;
}

// IInterface implementation: Query interface
StatusCode
MEPErrorAdder::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( riid == IRunable::interfaceID() )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly availible: try a base class
  return Service::queryInterface(riid, ppvInterface);
}

void  MEPErrorAdder::handle(const Incident& inc)    {
  m_log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  {
    m_svcState = STOPPED;
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_svcState = RUNNING;
  }
}

/** Setup published counters
 *
 *  reset array counters and publish array and single counters
 */
int
MEPErrorAdder::setupCounters() {  
  resetAllCounters();
  publishArrayCounters();
  publishSingleCounters();

  return 0;
}

/**Reset size and values to zero for all array counters
 *
 * Single counters set to zero when published...
 */
int
MEPErrorAdder::resetAllCounters() {
  resetCounters(m_rxOct, m_nSrc);
  resetCounters(m_rxPkt, m_nSrc);
  resetCounters(m_rxEvt, m_nSrc);
  resetCounters(m_rxMEP, m_nSrc);
  resetCounters(m_badPckFktPkt, m_nSrc);
  resetCounters(m_badLenPkt, m_nSrc);
  resetCounters(m_misPkt, m_nSrc);
  resetCounters(m_truncPkt, m_nSrc);
  resetCounters(m_multipleEvt, m_nSrc); 

  return 0;
}

/** Reset all 2D arrays, they keep track of received values
 */
int
MEPErrorAdder::resetRem2DCounters() {
  reset2DCounters(m_rBadLenPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rBadPckFktPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMisPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rTruncPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMultipleEvt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxOct,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxEvt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxMEP,m_nrServices,m_nSrc);

  return 0;
}

/** Reset all arrays for remembering received single counters
 */
int
MEPErrorAdder::resetRemSingleCounters() {
  m_rTotRxOct.resize(m_nrServices,0);
  m_rTotRxPkt.resize(m_nrServices,0);
  m_rTotRxEvt.resize(m_nrServices,0);
  m_rTotRxMEP.resize(m_nrServices,0);
  m_rIncEvt.resize(m_nrServices,0);
  m_rTotBadMEP.resize(m_nrServices,0);
  m_rTotMEPReq.resize(m_nrServices,0);
  m_rTotMEPReqPkt.resize(m_nrServices,0);
  m_rNumMEPRecvTimeouts.resize(m_nrServices,0);
  m_rNotReqPkt.resize(m_nrServices,0);
  m_rTotWrongPartID.resize(m_nrServices,0);

  m_rSentEvt.resize(m_nrServices,0); 
  m_rSentOct.resize(m_nrServices,0);
  m_rSentEvtErr.resize(m_nrServices,0);
 
  return 0;
}

/** Publishes all single counters
 */
int
MEPErrorAdder::publishSingleCounters() {
  PUBCNT(totRxOct,           "Total received bytes");
  PUBCNT(totRxPkt,           "Total received packets");
  PUBCNT(totRxEvt,	     "Total received events");
  PUBCNT(totRxMEP,           "Total received MEPs");
  PUBCNT(incEvt,             "Incomplete events"); 
  PUBCNT(totBadMEP,          "Total bad MEPs");
  PUBCNT(totMEPReq,          "Total requested MEPs");
  PUBCNT(totMEPReqPkt,       "Total Sent MEP-request packets");
  PUBCNT(numMEPRecvTimeouts, "MEP-receive Timeouts");
  PUBCNT(notReqPkt,          "Total unsolicited packets");
  PUBCNT(totWrongPartID,     "Packets with wrong partition-ID");
  
  PUBCNT(sentEvt,	     "Events sent from HLT (MDF)");
  PUBCNT(sentOct,            "Bytes sent from HLT");
  PUBCNT(sentEvtErr,	     "Number of event send errors from HLT");

  return 0;
}

/** Publishes all array counters
 */
int
MEPErrorAdder::publishArrayCounters() {
  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
  PUBARRAYCNT(misPkt,        "Missing MEPs");
  PUBARRAYCNT(badPckFktPkt,  "MEPs with wrong packing (MEP) factor");
  PUBARRAYCNT(truncPkt,      "Truncated (empty) MEPs");
  PUBARRAYCNT(multipleEvt,   "Duplicate Events");
  PUBARRAYCNT(rxOct,         "Received bytes");
  PUBARRAYCNT(rxPkt,         "Received packets");
  PUBARRAYCNT(rxEvt,         "Received events");
  PUBARRAYCNT(rxMEP,         "Received MEPs"); 

  return 0;
}

/** Unpublishes all array counters
 */
int
MEPErrorAdder::unpublishArrayCounters() {
  UNPUBCNT(badLenPkt);
  UNPUBCNT(misPkt);
  UNPUBCNT(badPckFktPkt);
  UNPUBCNT(truncPkt);
  UNPUBCNT(multipleEvt);
  UNPUBCNT(rxOct);
  UNPUBCNT(rxPkt);
  UNPUBCNT(rxEvt);
  UNPUBCNT(rxMEP);

  return 0;
}

/** This setup all subscriptions
 */
int
MEPErrorAdder::setupSubs() {

  char temp[100];

  // add all services we want to listen to..
  if (m_sumPartition) m_nrServices = m_nrSubFarms;
  else m_nrServices = m_nrSubNodes;     
  
  m_log << MSG::DEBUG << "Listening to " << m_nrServices << " services" << endmsg;

  // need to resize
  m_subsBadLenPkt.resize(m_nrServices,NULL);
  m_subsBadPckFktPkt.resize(m_nrServices,NULL);
  m_subsMisPkt.resize(m_nrServices,NULL);
  m_subsTruncPkt.resize(m_nrServices,NULL);
  m_subsMultipleEvt.resize(m_nrServices,NULL);
  m_subsRxOct.resize(m_nrServices,NULL);
  m_subsRxPkt.resize(m_nrServices,NULL);
  m_subsRxEvt.resize(m_nrServices,NULL);
  m_subsRxMEP.resize(m_nrServices,NULL);

  m_subsTotRxOct.resize(m_nrServices,NULL);
  m_subsTotRxPkt.resize(m_nrServices,NULL);
  m_subsTotRxEvt.resize(m_nrServices,NULL);
  m_subsTotRxMEP.resize(m_nrServices,NULL);
  m_subsIncEvt.resize(m_nrServices,NULL);
  m_subsTotBadMEP.resize(m_nrServices,NULL);
  m_subsTotMEPReq.resize(m_nrServices,NULL);
  m_subsTotMEPReqPkt.resize(m_nrServices,NULL);
  m_subsNumMEPRecvTimeouts.resize(m_nrServices,NULL);
  m_subsNotReqPkt.resize(m_nrServices,NULL);
  m_subsTotWrongPartID.resize(m_nrServices,NULL);
 
  m_subsSentEvt.resize(m_nrServices,NULL);
  m_subsSentOct.resize(m_nrServices,NULL);
  m_subsSentEvtErr.resize(m_nrServices,NULL);

  m_subsSrcName.resize(m_nrServices,NULL);

  // Reset arrays for remembering received values
  resetRem2DCounters();
  resetRemSingleCounters();

  if (m_sumPartition) { 
    //Partition sum

    for (int i=0;i<m_nrSubFarms;i++) {
  	//all subfarms
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/badLenPkt",m_subFarms[i].c_str());
	m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/badPckFktPkt",m_subFarms[i].c_str());
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s_MEPRxSTAT_1/Runable/misPkt",m_subFarms[i].c_str());
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/truncPkt",m_subFarms[i].c_str());
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/multipleEvt",m_subFarms[i].c_str());
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxOct",m_subFarms[i].c_str());
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxPkt",m_subFarms[i].c_str());
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxEvt",m_subFarms[i].c_str());
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxMEP",m_subFarms[i].c_str());
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
		
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxOct",m_subFarms[i].c_str());
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxPkt",m_subFarms[i].c_str());
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxEvt",m_subFarms[i].c_str());
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxMEP",m_subFarms[i].c_str());
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
      	sprintf(temp,"%s_MEPRxSTAT_1/Runable/incEvt",m_subFarms[i].c_str());
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totBadMEP",m_subFarms[i].c_str());
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totMEPReq",m_subFarms[i].c_str());
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totMEPReqPkt",m_subFarms[i].c_str());
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/numMEPRecvTimeouts",m_subFarms[i].c_str());
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/notReqPkt",m_subFarms[i].c_str());
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totWrongPartID",m_subFarms[i].c_str());
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    
	//MDF, from HLT
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentEvt",m_subFarms[i].c_str());
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentOct",m_subFarms[i].c_str());
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentEvtErr",m_subFarms[i].c_str());
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
 	

	sprintf(temp,"%s_MEPRxSTAT_1/Runable/srcName",m_subFarms[i].c_str());
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,(char *)"",this);
    }
  } else {
    //Subfarm sum
    for (int i=0;i<m_nrSubNodes;i++) {
   	//all nodes in this subfarm 
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badLenPkt",m_listenerDnsNode.c_str(),i+1);
        m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badPckFktPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/misPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/truncPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/multipleEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxOct",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxMEP",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxOct",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxEvt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxMEP",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_MEPRx_1/Runable/incEvt",m_listenerDnsNode.c_str(),i+1);
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totBadMEP",m_listenerDnsNode.c_str(),i+1);
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReq",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/numMEPRecvTimeouts",m_listenerDnsNode.c_str(),i+1);
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/notReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totWrongPartID",m_listenerDnsNode.c_str(),i+1);
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	//MDF, from HLT
    	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/BytesOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/EventsOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/ErrorsOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentEvtErr[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     
	sprintf(temp,"%s%.2i_MEPRx_1/Runable/srcName",m_listenerDnsNode.c_str(),i+1);
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,(char *)"",this);
 
    }
  }

  return 0;
}


/** When received arrays change size...
 * 
 * 1. Unpublish counters
 * 2. Reset all counters that are published or rememberd 2D counters
 * 3. Publish them again
 */
int
MEPErrorAdder::resetPublishedServices(int nSrc) {
   
  m_log << MSG::INFO << "Services have changed size (nr Src), previous: "<<m_nSrc<< ", new: "<<nSrc<<endmsg;
  
  m_nSrc = nSrc;  

  unpublishArrayCounters();
  
  resetAllCounters();
  resetRem2DCounters();
  
  publishArrayCounters();

  return 0;

}

/** Called when RunInfo setup changes, i.e. subfarms in partition changes
 *
 *  Will remove all subscriptions, update the list of subfarms, and setup 
 "  the subscriptions for the new subfarm list.
 *
 */
int
MEPErrorAdder::changeSubFarms(DimInfo* dim) {

  m_log << MSG::INFO << "Partition subFarm setup changed" << endmsg;

  //Remove subscriptions
  removeSubs();
 
  m_log << MSG::DEBUG << "Subscriptions removed" << endmsg;

  // Read RunInfo again to update subfarm list
  readRunInfo(dim);

  // Setup subscriptions again from all new subfarms
  m_log << MSG::DEBUG << "Setting up subscriptions again..." << endmsg;
  setupSubs();

  m_log << MSG::INFO << "Subscriptions have been re-initialized." << endmsg;  

  return 0;
	
}

