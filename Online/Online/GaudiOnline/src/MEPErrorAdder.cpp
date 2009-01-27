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

#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name, m_ ## name, desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size()*sizeof(int), desc, this);} while(0);

template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPErrorAdder);

using namespace LHCb;

MEPErrorAdder::MEPErrorAdder(const std::string& nam, ISvcLocator* svc) 
: Service(nam, svc), m_svcState(NOT_READY), m_incidentSvc(0), m_log(msgSvc(),nam)
{

  
  declareProperty("listenerDnsNode",    m_listenerDnsNode); 		//Also where we get RunInfo (ECSXX)
  declareProperty("senderDnsNode",      m_senderDnsNode);

  declareProperty("updateFrequency",    m_updateFrequency =5);		//How often the statistics should be updated
  
  declareProperty("sumPartition",	m_sumPartition=false); 	  	//Is this a sum over nodes in subfarm, or subfarms in partition?
  
  //Options for sum over partition 
  declareProperty("partitionName",      m_partitionName = "LHCb");	 
  declareProperty("runInfoDnsNode",	m_runInfoDnsNode = "");		// DIM_DNS_NODE for RunInfo, if other than listenerDnsNode  

  //Options for sum over subfarm
  declareProperty("nrSubNodes",         m_nrSubNodes =4);         	//Number of nodes per subfarm
  
  declareProperty("nSrc",               m_nSrc=100);			//Numer of TELL1 sources this partition have
									// Maybe we could get this from RunInfo instead....
}

MEPErrorAdder::~MEPErrorAdder() {
  
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
  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
  PUBARRAYCNT(misPkt,        "Missing MEPs");
  PUBARRAYCNT(badPckFktPkt,  "MEPs with wrong packing (MEP) factor");
  PUBARRAYCNT(truncPkt,      "Truncated (empty) MEPs");
  PUBARRAYCNT(multipleEvt,   "Duplicate Events");
  PUBARRAYCNT(rxOct,         "Received bytes");
  PUBARRAYCNT(rxPkt,         "Received packets");
  PUBARRAYCNT(rxEvt,         "Received events");

  return 0;
}


StatusCode
MEPErrorAdder::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) {
    return StatusCode::FAILURE; 
  }
  m_log << MSG::DEBUG << "Partition Name: " << m_partitionName << endmsg;
  
  DimServer::autoStartOff();

  char temp[100];

  if (m_sumPartition) {    
  	m_log << MSG::INFO << "Acquiring RunInfo information about subfarms etc. in partition" << endmsg;  
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
  DimServer::setDnsNode(m_senderDnsNode.c_str());

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

  if (m_sumPartition) sprintf(temp,"%s_MEPRx_1",m_partitionName.c_str());
  else sprintf(temp,"%s_MEPRx_1",m_listenerDnsNode.c_str());
  m_log << MSG::DEBUG << "Starting DIM Server:" << temp <<  endmsg;
  DimServer::start(temp);

  //m_log << MSG::DEBUG << "Listen to services" << endmsg;
  
  // add all services we want to listen to..
  int nrservices;
  if (m_sumPartition) nrservices = m_nrSubFarms;
  else nrservices = m_nrSubNodes;     
  // need to resize
  m_subsBadLenPkt.resize(nrservices,NULL);
  m_subsBadPckFktPkt.resize(nrservices,NULL);
  m_subsMisPkt.resize(nrservices,NULL);
  m_subsTruncPkt.resize(nrservices,NULL);
  m_subsMultipleEvt.resize(nrservices,NULL);
  m_subsRxOct.resize(nrservices,NULL);
  m_subsRxPkt.resize(nrservices,NULL);
  m_subsRxEvt.resize(nrservices,NULL);
  m_rBadLenPkt.resize(nrservices);
  m_rBadPckFktPkt.resize(nrservices);
  m_rMisPkt.resize(nrservices);
  m_rTruncPkt.resize(nrservices);
  m_rMultipleEvt.resize(nrservices);
  m_rRxOct.resize(nrservices);
  m_rRxPkt.resize(nrservices);
  m_rRxEvt.resize(nrservices);
  if (m_sumPartition) { 
    //Partition sum
    for (int i=0;i<m_nrSubFarms;i++) {
  	//all subfarms
	sprintf(temp,"%s_MEPRx_1/Runable/badLenPkt",m_subFarms[i].c_str());
	m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/badPckFktPkt",m_subFarms[i].c_str());
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
   	sprintf(temp,"%s_MEPRx_1/Runable/misPkt",m_subFarms[i].c_str());
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/truncPkt",m_subFarms[i].c_str());
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/multipleEvt",m_subFarms[i].c_str());
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxOct",m_subFarms[i].c_str());
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxPkt",m_subFarms[i].c_str());
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s_MEPRx_1/Runable/rxEvt",m_subFarms[i].c_str());
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,0,this);
	
    }
  } else {
    //Subfarm sum
    for (int i=0;i<m_nrSubNodes;i++) {
   	//all nodes in this subfarm 
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badLenPkt",m_listenerDnsNode.c_str(),i+1);
        m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badPckFktPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/misPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/truncPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/multipleEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxOct",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,0,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,0,this);
	
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

  m_svcState = NOT_READY;  
  return Service::finalize();
} 

void
MEPErrorAdder::infoHandler() {
  DimInfo * curr = getInfo();
 
  m_log << MSG::DEBUG << "Receiving data from " << curr->getName() << endmsg;
  
  int nrservices;
  if (m_sumPartition) nrservices = m_nrSubFarms;
  else nrservices = m_nrSubNodes;     
 
  int diff;
  int prevSize;
  int newSize;
  //To know what service, we must iterate over all...
  for (int i=0;i<nrservices; i++) {
    if (curr == m_subsBadLenPkt[i]) {
      prevSize = m_rBadLenPkt[i].size();
      newSize = curr->getSize()/sizeof(int);
      if (prevSize<newSize) m_rBadLenPkt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rBadLenPkt[i][j];
        m_rBadLenPkt[i][j] = data[j]; 	//save the received data
        m_badLenPkt[j] += diff;		//add difference to get current value
	//m_log << MSG::DEBUG << "Data: " << data[j] << " Diff: " << diff << " Value: "<< m_badLenPkt[j] << endmsg;
      }
      break;
    }
    if (curr == m_subsBadPckFktPkt[i]) {
      prevSize = m_rBadPckFktPkt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rBadPckFktPkt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rBadPckFktPkt[i][j];
        m_rBadPckFktPkt[i][j] = data[j];
        m_badPckFktPkt[j] += diff;
      }
      break;
    }
    if (curr == m_subsMisPkt[i]) {
      prevSize = m_rMisPkt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rMisPkt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rMisPkt[i][j];
        m_rMisPkt[i][j] = data[j];
        m_misPkt[j] += diff;
      }
      break;
    }
    if (curr == m_subsTruncPkt[i]) {
      prevSize = m_rTruncPkt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rTruncPkt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rTruncPkt[i][j];
        m_rTruncPkt[i][j] = data[j];
        m_truncPkt[j] += diff;
      }
      break;
    }
    
    if (curr == m_subsMultipleEvt[i]) {
      prevSize = m_rMultipleEvt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rMultipleEvt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rMultipleEvt[i][j];
        m_rMultipleEvt[i][j] = data[j];
        m_multipleEvt[j] += diff;
      }
      break;
    }
    if (curr == m_subsRxOct[i]) {
      prevSize = m_rRxOct[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rRxOct[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rRxOct[i][j];
        m_rRxOct[i][j] = data[j];
        m_rxOct[j] += diff;
      }
      break;
    }
    if (curr == m_subsRxPkt[i]) {
      prevSize = m_rRxPkt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rRxPkt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rRxPkt[i][j];
        m_rRxPkt[i][j] = data[j];
        m_rxPkt[j] += diff;
      }
      break;
    }
    if (curr == m_subsRxEvt[i]) {
      prevSize = m_rRxEvt[i].size();
      newSize = curr->getSize()/sizeof(int);
      
      if (prevSize<newSize) m_rRxEvt[i].resize(newSize,0); 
      int * data = (int*) curr->getData();
      
      for (int j=0; j<newSize; j++) {
        diff = data[j] - m_rRxEvt[i][j];
        m_rRxEvt[i][j] = data[j];
        m_rxEvt[j] += diff;
      }
      break;
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



