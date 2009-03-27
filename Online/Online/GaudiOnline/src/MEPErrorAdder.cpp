//  ============================================================
//
//  ErrorAddingSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : GaudiOnline
//
//  Author    : David Svantesson i
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

#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name,"X", & m_ ## name, sizeof(int64_t), desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "X", & m_ ## name [0], m_ ## name.size()*sizeof(int64_t), desc, this);} while(0);

template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
}

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

  
  declareProperty("listenerDnsNode",    m_listenerDnsNode); 		//Also where we get RunInfo (ECSXX)
  declareProperty("updateFrequency",    m_updateFrequency =5);		//How often the statistics should be updated
  
  declareProperty("sumPartition",	m_sumPartition=false); 	  	//Is this a sum over nodes in subfarm, or subfarms in partition?
  
  //Options for sum over partition 
  declareProperty("partitionName",      m_partitionName = "LHCb");	 
  declareProperty("runInfoDnsNode",	m_runInfoDnsNode = "");		// DIM_DNS_NODE for RunInfo, if other than listenerDnsNode  

  //Options for sum over subfarm
  declareProperty("nrSubNodes",         m_nrSubNodes =4);         	//Number of nodes per subfarm
  
  declareProperty("nSrc",               m_nSrc=100);			//Numer of TELL1 sources this partition have

}

MEPErrorAdder::~MEPErrorAdder() {
  int size = (int) m_badLenPkt.size();
  for (int i=0; i<size; i++) {
    delete m_subsBadLenPkt[i];
    delete m_subsBadPckFktPkt[i];
    delete m_subsMisPkt[i];
    delete m_subsTruncPkt[i];
    delete m_subsMultipleEvt[i];
    delete m_subsRxOct[i];
    delete m_subsRxPkt[i];
    delete m_subsRxEvt[i];
    delete m_subsTotRxOct[i];
    delete m_subsTotRxPkt[i];
    delete m_subsIncEvt[i];
    delete m_subsTotBadMEP[i];
    delete m_subsTotMEPReq[i];
    delete m_subsTotMEPReqPkt[i];
    delete m_subsNumMEPRecvTimeouts[i];
    delete m_subsNotReqPkt[i];
    delete m_subsTotWrongPartID[i];
    delete m_subsSrcName[i];
  }	
}

int
MEPErrorAdder::setupCounters() {
  resetCounters(m_rxOct, m_nSrc);
  resetCounters(m_rxPkt, m_nSrc);
  resetCounters(m_rxEvt, m_nSrc);
  resetCounters(m_badPckFktPkt, m_nSrc);
  resetCounters(m_badLenPkt, m_nSrc);
  resetCounters(m_misPkt, m_nSrc);
  resetCounters(m_truncPkt, m_nSrc);
  resetCounters(m_multipleEvt, m_nSrc); 

  m_totMEPReq          = 0;
  m_totMEPReqPkt       = 0;
  m_numMEPRecvTimeouts = 0;
  m_notReqPkt          = 0;
  m_totRxOct           = 0;
  m_totRxPkt           = 0;
  m_incEvt             = 0; 
  m_totMEPReqPkt       = 0;
  m_numMEPRecvTimeouts = 0;
  m_totMEPReq          = 0;
  m_totBadMEP          = 0;
  m_totWrongPartID     = 0;

  m_allNamesSize       = 0;

  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
  PUBARRAYCNT(misPkt,        "Missing MEPs");
  PUBARRAYCNT(badPckFktPkt,  "MEPs with wrong packing (MEP) factor");
  PUBARRAYCNT(truncPkt,      "Truncated (empty) MEPs");
  PUBARRAYCNT(multipleEvt,   "Duplicate Events");
  PUBARRAYCNT(rxOct,         "Received bytes");
  PUBARRAYCNT(rxPkt,         "Received packets");
  PUBARRAYCNT(rxEvt,         "Received events");

  PUBCNT(totRxOct,           "Total received bytes");
  PUBCNT(totRxPkt,           "Total received packets");
  PUBCNT(incEvt,             "Incomplete events"); 
  PUBCNT(totBadMEP,          "Total bad MEPs");
  PUBCNT(totMEPReq,          "Total requested MEPs");
  PUBCNT(totMEPReqPkt,       "Total Sent MEP-request packets");
  PUBCNT(numMEPRecvTimeouts, "MEP-receive Timeouts");
  PUBCNT(notReqPkt,          "Total unsolicited packets");
  PUBCNT(totWrongPartID,     "Packets with wrong partition-ID");

  return 0;
}


StatusCode
MEPErrorAdder::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) {
    return StatusCode::FAILURE; 
  }
  m_log << MSG::DEBUG << "Partition Name: " << m_partitionName << endmsg;
  
  char temp[100];

  if (m_sumPartition) {    
  	m_log << MSG::DEBUG << "Acquiring RunInfo information about subfarms etc. in partition" << endmsg;  
 	if (m_runInfoDnsNode.size()>0) DimClient::setDnsNode(m_runInfoDnsNode.c_str());
	else DimClient::setDnsNode(m_listenerDnsNode.c_str());

  	//First get RunInfo (list of subfarms).
	sprintf(temp,"RunInfo/%s/HLTnSubFarms",m_partitionName.c_str());
  	
	DimCurrentInfo nSubFarms(temp,-1);  //must use CurrentInfo to not get segfault (wait for data)
  	m_nrSubFarms = nSubFarms.getInt();
  	m_log << MSG::INFO << "Number of subfarms in partition: " << m_nrSubFarms << endmsg;
  
  	sprintf(temp,"RunInfo/%s/HLTsubFarms",m_partitionName.c_str());
  	DimCurrentInfo SubFarms(temp,(char*)"NA");
  	const char* subFarmsChar = SubFarms.getString(); 
 
  	m_subFarms.resize(m_nrSubFarms);
  	int start = 0;
  	for (int i=0;i<m_nrSubFarms;i++) {
    		m_subFarms[i].insert(0,&subFarmsChar[start]);
    		m_log << MSG::DEBUG << "Subfarm: " << m_subFarms[i] << endmsg;
    		start += (m_subFarms[i].size() + 1);
  	}  
  }
  
  DimClient::setDnsNode(m_listenerDnsNode.c_str());


  //All services we want to publish, through the Monitoring Svc
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

  // add all services we want to listen to..
  if (m_sumPartition) m_nrServices = m_nrSubFarms;
  else m_nrServices = m_nrSubNodes;     
  // need to resize
  m_subsBadLenPkt.resize(m_nrServices,NULL);
  m_subsBadPckFktPkt.resize(m_nrServices,NULL);
  m_subsMisPkt.resize(m_nrServices,NULL);
  m_subsTruncPkt.resize(m_nrServices,NULL);
  m_subsMultipleEvt.resize(m_nrServices,NULL);
  m_subsRxOct.resize(m_nrServices,NULL);
  m_subsRxPkt.resize(m_nrServices,NULL);
  m_subsRxEvt.resize(m_nrServices,NULL);

  m_subsTotRxOct.resize(m_nrServices,NULL);
  m_subsTotRxPkt.resize(m_nrServices,NULL);
  m_subsIncEvt.resize(m_nrServices,NULL);
  m_subsTotBadMEP.resize(m_nrServices,NULL);
  m_subsTotMEPReq.resize(m_nrServices,NULL);
  m_subsTotMEPReqPkt.resize(m_nrServices,NULL);
  m_subsNumMEPRecvTimeouts.resize(m_nrServices,NULL);
  m_subsNotReqPkt.resize(m_nrServices,NULL);
  m_subsTotWrongPartID.resize(m_nrServices,NULL);

  m_subsSrcName.resize(m_nrServices,NULL);

  reset2DCounters(m_rBadLenPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rBadPckFktPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMisPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rTruncPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMultipleEvt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxOct,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxEvt,m_nrServices,m_nSrc);

  m_rTotRxOct.resize(m_nrServices,0);
  m_rTotRxPkt.resize(m_nrServices,0);
  m_rIncEvt.resize(m_nrServices,0);
  m_rTotBadMEP.resize(m_nrServices,0);
  m_rTotMEPReq.resize(m_nrServices,0);
  m_rTotMEPReqPkt.resize(m_nrServices,0);
  m_rNumMEPRecvTimeouts.resize(m_nrServices,0);
  m_rNotReqPkt.resize(m_nrServices,0);
  m_rTotWrongPartID.resize(m_nrServices,0);

  longlong zero = 0;

  if (m_sumPartition) { 
    //Partition sum
    for (int i=0;i<m_nrSubFarms;i++) {
  	//all subfarms
	sprintf(temp,"%s_MEPRx_1/Runable/badLenPkt",m_subFarms[i].c_str());
	m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/badPckFktPkt",m_subFarms[i].c_str());
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
   	sprintf(temp,"%s_MEPRx_1/Runable/misPkt",m_subFarms[i].c_str());
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/truncPkt",m_subFarms[i].c_str());
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/multipleEvt",m_subFarms[i].c_str());
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxOct",m_subFarms[i].c_str());
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxPkt",m_subFarms[i].c_str());
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxEvt",m_subFarms[i].c_str());
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
	
	sprintf(temp,"%s_MEPRx_1/Runable/totRxOct",m_subFarms[i].c_str());
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/totRxPkt",m_subFarms[i].c_str());
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/incEvt",m_subFarms[i].c_str());
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/totBadMEP",m_subFarms[i].c_str());
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/totMEPReq",m_subFarms[i].c_str());
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/totMEPReqPkt",m_subFarms[i].c_str());
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/numMEPRecvTimeouts",m_subFarms[i].c_str());
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/notReqPkt",m_subFarms[i].c_str());
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/totWrongPartID",m_subFarms[i].c_str());
	m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    
	sprintf(temp,"%s_MEPRx_1/Runable/srcName",m_subFarms[i].c_str());
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,"",this);
    }
  } else {
    //Subfarm sum
    for (int i=0;i<m_nrSubNodes;i++) {
   	//all nodes in this subfarm 
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badLenPkt",m_listenerDnsNode.c_str(),i+1);
        m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badPckFktPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/misPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/truncPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/multipleEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxOct",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);

	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxOct",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/incEvt",m_listenerDnsNode.c_str(),i+1);
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totBadMEP",m_listenerDnsNode.c_str(),i+1);
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReq",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/numMEPRecvTimeouts",m_listenerDnsNode.c_str(),i+1);
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/notReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totWrongPartID",m_listenerDnsNode.c_str(),i+1);
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,zero,this);
    
	sprintf(temp,"%s%.2i_MEPRx_1/Runable/srcName",m_listenerDnsNode.c_str(),i+1);
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,"",this);
 
    }
  }
 
  m_log << MSG::DEBUG << "Initialization done" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode
MEPErrorAdder::finalize() {
  if (m_monSvc) {
    m_monSvc->undeclareAll(this);
    m_monSvc->release();
    m_monSvc = 0;
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
  m_subsIncEvt.clear();
  m_subsTotBadMEP.clear();
  m_subsTotMEPReq.clear();
  m_subsTotMEPReqPkt.clear();
  m_subsNumMEPRecvTimeouts.clear();
  m_subsNotReqPkt.clear();
  m_subsTotWrongPartID.clear();
  m_subsSrcName.clear();

  m_svcState = NOT_READY;  
  return Service::finalize();
} 

/** Help-function for receiving array counters
 *
 */
bool
MEPErrorAdder::ReceiveArrayService(DimInfo * curr, DimInfo * subs,  std::vector<int64_t> &rArray, std::vector<int64_t> &sArray) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int arraySize = curr->getSize()/sizeof(int64_t);
  if (arraySize<m_nSrc) {
    m_log << MSG::INFO << "Received less data than number of sources from a service." << endmsg;
  }
  else if (arraySize>m_nSrc) {
    //This is an error, we can't receive more data than the number of sources.
    m_log << MSG::WARNING << "Received too much data from a service (compared to number of sources), ignoring..." << endmsg;	
    arraySize = m_nSrc;
  }

  int64_t * data = (int64_t*) curr->getData();
  int64_t diff;

  for (int j=0; j<arraySize; j++) {
    diff = data[j] - rArray[j];
    sArray[j] += diff;
  }

  return true;
}

/** Help-function for receiving single counters
 *
 */
bool
MEPErrorAdder::ReceiveSingleService(DimInfo * curr, DimInfo * subs, int64_t rValue, int64_t sValue) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int64_t data = *((int64_t*) curr->getData());
  
  int64_t diff = data - rValue;
  rValue = data;
  sValue += diff;

  return true;

}

void
MEPErrorAdder::infoHandler() {
  DimInfo * curr = getInfo();
 
  m_log << MSG::DEBUG << "Receiving data from " << curr->getName() << endmsg;

  //To know what service, we must iterate over all...
  for (int i=0;i<m_nrServices; i++) {
    if (ReceiveArrayService(curr,m_subsBadLenPkt[i], m_rBadLenPkt[i], m_badLenPkt)) break;
    if (ReceiveArrayService(curr,m_subsBadPckFktPkt[i], m_rBadPckFktPkt[i], m_badPckFktPkt)) break;
    if (ReceiveArrayService(curr,m_subsMisPkt[i], m_rMisPkt[i], m_misPkt)) break;
    if (ReceiveArrayService(curr,m_subsTruncPkt[i], m_rTruncPkt[i], m_truncPkt)) break;
    if (ReceiveArrayService(curr,m_subsMultipleEvt[i], m_rMultipleEvt[i], m_multipleEvt)) break;
    if (ReceiveArrayService(curr,m_subsRxOct[i], m_rRxOct[i], m_rxOct)) break;
    if (ReceiveArrayService(curr,m_subsRxPkt[i], m_rRxPkt[i], m_rxPkt)) break;
    if (ReceiveArrayService(curr,m_subsRxEvt[i], m_rRxEvt[i], m_rxEvt)) break;
   
    if (ReceiveSingleService(curr,m_subsTotRxOct[i], m_rTotRxOct[i], m_totRxOct)) break;    
    if (ReceiveSingleService(curr,m_subsTotRxPkt[i], m_rTotRxPkt[i], m_totRxPkt)) break;    
    if (ReceiveSingleService(curr,m_subsIncEvt[i], m_rIncEvt[i], m_incEvt)) break;    
    if (ReceiveSingleService(curr,m_subsTotBadMEP[i], m_rTotBadMEP[i], m_totBadMEP)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReq[i], m_rTotMEPReq[i], m_totMEPReq)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReqPkt[i], m_rTotMEPReqPkt[i], m_totMEPReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsNumMEPRecvTimeouts[i], m_rNumMEPRecvTimeouts[i], m_numMEPRecvTimeouts)) break;    
    if (ReceiveSingleService(curr,m_subsNotReqPkt[i], m_rNotReqPkt[i], m_notReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsTotWrongPartID[i], m_rTotWrongPartID[i], m_totWrongPartID)) break;    
  
    if (curr == m_subsSrcName[i]) {
	//Receiving source names
	
	int size = curr->getSize();
	if (size>1) {
		if (m_allNamesSize==0) {
			//Received first time
			m_allNamesSize=size;
			if (!(m_allNames = new char[size])) {
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
      //m_log << MSG::DEBUG << "Not ready anymore" << endmsg;
      continue;
      //return StatusCode::SUCCESS;
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
    MEPRxSys::microsleep(100000);
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



