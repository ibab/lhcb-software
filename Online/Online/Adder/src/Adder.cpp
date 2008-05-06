#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "RTL/rtl.h"
#include "dic.hxx"
#include "dis.hxx"

#include "Adder.h"
#include "DimInfoServerStatus.h" 
#include "DimTimerAdder.h" 

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Adder)

Adder::Adder(const std::string& name, ISvcLocator* ploc): 
  Algorithm(name, ploc)
{
  declareProperty("nodenames",m_nodeName);
  declareProperty("objectname",m_objectName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("taskname",m_taskName);
  declareProperty("refreshtime",m_refreshTime);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("servername",m_serverName);

  m_updateTimerAdder = new bool();
  m_choosedServerName = new std::string();
  m_firstExecute = true;
  // Server status. node up => status=true. node down => status=false.
  m_serverStatus = new std::map<std::string, bool, std::less<std::string> >();
  // Service set readed from some server. 
  m_serviceSet = new std::set<std::string>();
  // Both lists are filled in DimInfoServerStatus
}

StatusCode Adder::initialize() {
  
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());

  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to initialize Algorithm" << endreq;
    return sc;
  }
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "*************Welcome to Adder v5r0******************* " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "This Adder will add data published in : " << m_dimClientDns << endreq;
  
  msg << MSG::DEBUG << "Consider nodes: ";
  for(unsigned int i=0; i!=m_nodeName.size(); ++i) msg << m_nodeName[i] << " ";
  msg << endreq;
  
  msg << MSG::DEBUG << "Properties: " << endreq;
  for(unsigned int i=0; i!=m_objectName.size();++i) {
    msg << MSG::DEBUG << "         objectName: " << m_objectName[i] << endreq;
    msg << MSG::DEBUG << "         taskName: " << m_taskName[i] << endreq;
    msg << MSG::DEBUG << "         algorithmName: " << m_algorithmName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }
  
  msg << MSG::DEBUG << "Results in Server: " << m_serverName << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;

  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "creating Server/Service Status Objects" << endreq;
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  m_dimInfoServerStatus = new DimInfoServerStatus(m_serverStatus, 
                                                  m_serviceSet, 
                                                  m_updateTimerAdder, 
                                                  m_choosedServerName,
                                                  m_nodeName, 
                                                  m_taskName, 
                                                  m_algorithmName, 
                                                  m_objectName,
                                                  msgSvc());

  // test to verify if DimInfoServerStatus works fine   
/*  while(1){
    msg << MSG::DEBUG << m_serverStatus->size() << " Servers: " ;
    for (m_serverIt=m_serverStatus->begin() ; m_serverIt != m_serverStatus->end(); ++m_serverIt) {
      msg << m_serverIt->first ; if (m_serverIt->second) msg << "[active], "; else msg << "[inactive], "; }
    msg << " ||| " << m_serviceSet->size() << " Services in " << m_dimInfoServerStatus->choosedServer() << ": ";
    for (m_serviceIt=m_serviceSet->begin() ; m_serviceIt != m_serviceSet->end(); ++m_serviceIt) msg << (*m_serviceIt) << ", ";
    msg << endreq;
  }*/
  
  int tmpStringSize = -1;
  while ( (tmpStringSize = m_serviceSet->size()) <=0 ){
    msg << MSG::DEBUG << "******************************************************"<< endreq;
    msg << MSG::DEBUG << "Waiting 5 seconds until we have the Service Set loaded"<< endreq;
    msg << MSG::DEBUG << "******************************************************"<< endreq;
    sleep(5); 
  }

  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "************Service Set loaded***************"<< endreq;
  m_dimInfoServerStatus->printServiceSet();  
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  
//   msg << MSG::DEBUG << "Printing service map:" << endreq;
//   for (m_serviceIt=m_serviceSet->begin() ; m_serviceIt != m_serviceSet->end(); ++m_serviceIt){
//     std::string serviceName = (*m_serviceIt);
//     msg << MSG::DEBUG << serviceName << endreq;
//     for (m_serverIt=m_serverStatus->begin() ; m_serverIt != m_serverStatus->end(); ++m_serverIt){
//       if ((*m_serverIt).first.compare((*m_choosedServerName)) == 0) continue;
//       std::string::size_type posIni = serviceName.find((*m_choosedServerName));
//       std::string::size_type posFin = posIni + (*m_choosedServerName).size();
//       std::string otherServiceName = serviceName.substr(0, posIni) + (*m_serverIt).first + serviceName.substr(posFin);
//       msg << MSG::DEBUG << otherServiceName << endreq;
//     } 
//   }

  DimServer::autoStartOff();
  DimClient::setDnsNode(m_dimClientDns.c_str());

  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "creating DimTimerAdder:" << endreq;
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  m_dimTimerAdder = new  DimTimerAdder (m_serverStatus, 
                 m_serviceSet,
                 m_updateTimerAdder,
                 m_choosedServerName,
                 m_serverName,
                 m_refreshTime,
                 msgSvc());
 
 DimServer::start(RTL::processName().c_str());
 
 DimServer::autoStartOn(); 
 
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  msg << MSG::DEBUG << "Waiting until we have the Timer data loaded"<< endreq;
  msg << MSG::DEBUG << "*********************************************"<< endreq;
  sleep(10);
    
  return StatusCode::SUCCESS;
}


StatusCode Adder::execute() {
  MsgStream msg(msgSvc(), name());

  if (m_firstExecute) {
    //m_dimTimerAdder->enableHandler();
    m_dimTimerAdder->start(m_refreshTime);
    m_firstExecute = false;
  }
    
  if ((*m_updateTimerAdder)||m_dimTimerAdder->statusDiscrepancy()){ 
  
    if ((*m_updateTimerAdder)) { 
      msg << MSG::DEBUG << "**********************************************************************" << endreq;
      msg << MSG::DEBUG << "** Updatig Adder Maps because of changes in Servers/Services Status **" << endreq;
      msg << MSG::DEBUG << "**********************************************************************" << endreq;
    } 
    else 
    { 
      msg << MSG::DEBUG << "*****************************************************************" << endreq; 
      msg << MSG::DEBUG << "**    Updatig Adder Maps because of Discrepancy in Status      **" << endreq; 
      msg << MSG::DEBUG << "**    That is because Status changed in the last Updating      **" << endreq; 
      msg << MSG::DEBUG << "*****************************************************************" << endreq; 
    }
    
    msg << MSG::DEBUG << "Disabling TimerHandler"<< endreq;
    m_dimTimerAdder->stop();
    
//     //If here we have "Standard std::exception is caught" then we have to sleep time
//     //it might depends of the server numbers and the service monitored in each server
     msg << MSG::DEBUG << "********************************************************************" << endreq;
     msg << MSG::DEBUG << "** Waiting until we have Servers/Services Status completly loaded **" << endreq;
     msg << MSG::DEBUG << "********************************************************************" << endreq;
     sleep(10);    

    msg << MSG::DEBUG << "updateDimInfoServiceMap" << endreq;
    m_dimTimerAdder->updateDimInfoServiceMap();
    
/*    msg << MSG::DEBUG << "printDimInfoMap" << endreq;
    m_dimTimerAdder->printDimInfoMap();*/
    
    msg << MSG::DEBUG << "*********************************************"<< endreq;
    msg << MSG::DEBUG << " Waiting until we have the Timer data loaded "<< endreq;
    msg << MSG::DEBUG << "*********************************************"<< endreq;
    sleep(10);
    
    msg << MSG::DEBUG << "Enabling TimerHandler"<< endreq;
    m_dimTimerAdder->start(m_refreshTime);
    (*m_updateTimerAdder) = false;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode Adder::finalize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  delete m_dimTimerAdder; m_dimTimerAdder = 0;
  delete m_dimInfoServerStatus; m_dimInfoServerStatus=0;
  delete m_updateTimerAdder;  m_updateTimerAdder=0;
  delete m_choosedServerName; m_choosedServerName=0;
  delete m_serverStatus; m_serverStatus=0;
  delete m_serviceSet; m_serviceSet=0;
  return StatusCode::SUCCESS;
}


