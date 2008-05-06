#include <iostream>
#include<time.h>

#include "DimTimerAdder.h"
#include "Misc.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimServiceMonObject.h"

DimTimerAdder::DimTimerAdder (const std::map<std::string, bool, std::less<std::string> > *serverStatus, 
                              const std::set<std::string> *serviceSet,
                              bool *updateTimerAdder,
                              const std::string *choosedServerName,
                              const std::string &serverNameAdder,
                              const int &refreshTime,
                              IMessageSvc* msgSvc) : 
  DimTimer(refreshTime),
  m_serverStatus(serverStatus),
  m_serviceSet(serviceSet),
  m_updateTimerAdder(updateTimerAdder),
  m_choosedServerName(choosedServerName),
  m_serverNameAdder(serverNameAdder),
  m_refreshTime(refreshTime),
  m_msgSvc(msgSvc)
{
  m_name = "DimTimerAdder";
  MsgStream msg(msgSvc, name());
  msg << MSG::DEBUG << "Inside DimTimerAdder Creator." << endreq;
  stop();
  msg << MSG::DEBUG << "updateDimInfoServiceMap" << endreq;
  //disableHandler();
  updateDimInfoServiceMap();
  msg << MSG::DEBUG << "printDimInfoMap" << endreq;
  printDimInfoMap();
  //start(m_refreshTime);
  //enableHandler();
}

DimTimerAdder::~DimTimerAdder(){
  std::map<std::string, DimServiceMonObject*> m_dimSrv;
  std::map<std::string, MonObject*> m_monObject;
  std::map<std::string, std::map<std::string, DimInfoMonObject*> > m_dimInfo;

  for (m_dimSrvIt=m_dimSrv.begin(); m_dimSrvIt!=m_dimSrv.end(); ++m_dimSrvIt){
    delete m_dimSrvIt->second; m_dimSrvIt->second = 0; 
  }
  m_dimSrv.clear();
  
  for (m_monObjectIt=m_monObject.begin(); m_monObjectIt!=m_monObject.end(); ++m_monObjectIt){
    delete m_monObjectIt->second; m_monObjectIt->second = 0; 
  }
  m_monObject.clear();

  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt){
    for (m_dimInfo2It=m_dimInfoIt->second.begin(); m_dimInfo2It!=m_dimInfoIt->second.end(); ++m_dimInfo2It){
      delete m_dimInfo2It->second; m_dimInfo2It->second = 0;
    }
    m_dimInfoIt->second.clear();
  }
  m_dimInfo.clear();
}

void DimTimerAdder::timerHandler(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "**********************************" << endreq;
  msg << MSG::DEBUG << "         timerHandler             " << endreq;
  time_t now;
  time(&now);
  // Here we can not call updateDimInfoServiceMap because 
  // I dont know why DIM doesnt return DimInfo.getData() 
  // when we try to create DimInfo maps.
  // Then we call that function in Adder.execute()
  // and here we consider only the dimInfos which 
  // have the server active and the service in the ServerSet.
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt){
    if (!adderActive(m_dimInfoIt->first)) continue;
    m_monObject[m_dimInfoIt->first]->reset();
//    msg << MSG::DEBUG << "Adder " << m_dimInfoIt->first << " adds " << m_dimInfoIt->second.size() << " Terms." << endreq;
    for (m_dimInfo2It=m_dimInfoIt->second.begin(); m_dimInfo2It!=m_dimInfoIt->second.end(); ++m_dimInfo2It){
      if (!termActive(m_dimInfo2It->first)) continue;
      m_dimInfo2It->second->loadMonObject();
      m_monObject[m_dimInfoIt->first]->combine(m_dimInfo2It->second->monObject());
    }
//    msg << MSG::DEBUG << "Results in MonObject: " << endreq;
//    m_dimSrv[m_dimInfoIt->first]->monObject()->print();
//    msg << MSG::DEBUG << "The Same in in MonObject: " << endreq;
//    m_monObject[m_dimInfoIt->first]->print();
//    msg << MSG::DEBUG << "Publish it in " << m_dimSrv[m_dimInfoIt->first]->getName() << endreq;
  }
  start(m_refreshTime);
  msg << MSG::DEBUG << "        end  timerHandler         " << endreq;
  msg << MSG::DEBUG << "**********************************" << endreq;
}

bool DimTimerAdder::adderActive(const std::string &adderName){
  MsgStream msg(msgSvc(), name());
  //msg << MSG::DEBUG << "adderName = "<< adderName << endreq;
  //msg << MSG::DEBUG << "m_choosedServerNameLocal = "<< m_choosedServerNameLocal << endreq;
  std::string coosedServiceName = m_dimInfo[adderName][m_choosedServerNameLocal]->dimInfo()->getName();
  if (m_serviceSet->find(coosedServiceName) != m_serviceSet->end()) {
    //msg << MSG::DEBUG << "Adder " << adderName << " is active"<< endreq;
    return true;
  }
  //msg << MSG::DEBUG << "Adder " << adderName << " is inactive"<< endreq;
  return false;
}

bool DimTimerAdder::termActive(const std::string &termName){
  MsgStream msg(msgSvc(), name());
  if (m_serverStatus->find(termName)->second){ 
    //msg << MSG::DEBUG << "Server " << termName << " is active"<< endreq;
    return true;
  }
  //msg << MSG::DEBUG << "Server " << termName << " is inactive."<< endreq;
  return false;
}

void DimTimerAdder::printDimInfoMap(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "DimInfoMap has "<< m_dimInfo.size() << " adders" << endreq; 
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt){
    msg << MSG::DEBUG << "   Adder "<< m_dimInfoIt->first << " has "<< m_dimInfoIt->second.size() << " terms" << endreq; 
    for (m_dimInfo2It=m_dimInfoIt->second.begin(); m_dimInfo2It!=m_dimInfoIt->second.end(); ++m_dimInfo2It)
      msg << MSG::DEBUG << "      "<< m_dimInfo2It->first<< endreq;
  }
}

void DimTimerAdder::printServerStatus(std::map<std::string, bool, std::less<std::string> > serverStatus) {
  MsgStream msg(msgSvc(), name());
  std::map<std::string, bool, std::less<std::string> >::const_iterator itStat;
  msg << MSG::DEBUG << serverStatus.size() << " Servers: " ;
  if (serverStatus.size() > 0){
    for (itStat= serverStatus.begin() ; itStat!=serverStatus.end(); ++itStat) {
      msg << itStat->first ; if (itStat->second) msg << "[active], "; else msg << "[inactive], ";}
  }
  msg << endreq;
}

void DimTimerAdder::printServiceSet(const std::set<std::string> serviceSet){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << serviceSet.size() << " Services " << endreq;
  if (serviceSet.size() > 0){
     std::set<std::string>::iterator serviceIt;
     for (serviceIt=serviceSet.begin() ; serviceIt != serviceSet.end(); ++serviceIt)
       msg << MSG::DEBUG << " service: " <<  (*serviceIt) << endreq;
  }
}


bool DimTimerAdder::statusDiscrepancy() {

  std::map<std::string, bool, std::less<std::string> >::const_iterator itStat;
  
  if (m_choosedServerNameLocal.compare((*m_choosedServerName)) != 0 ) return true;
  
  if (m_serverStatusLocal.size() != m_serverStatus->size()) return true;
  
  for (itStat=m_serverStatusLocal.begin() ; itStat != m_serverStatusLocal.end(); ++itStat)
    if (m_serverStatus->find(itStat->first)->second != itStat->second) return true;
    
  return false;
}    

  
void DimTimerAdder::updateDimInfoServiceMap() {
  MsgStream msg(msgSvc(), name());

  std::set<std::string>::const_iterator it;
  std::map<std::string, bool, std::less<std::string> >::const_iterator itStat;
  
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Local Server Status:" << endreq;
  printServerStatus(m_serverStatusLocal);
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Current Server Status:" << endreq;
  printServerStatus((*m_serverStatus));
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Local Server Choosed:" <<  m_choosedServerNameLocal << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Current Server Choosed:" <<  (*m_choosedServerName) << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Local Service Set:" << endreq;
  printServiceSet(m_serviceSetLocal);
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << "Current Service Set:" << endreq;
  printServiceSet((*m_serviceSet));
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Excluding Services from Timer Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  for (it=m_serviceSetLocal.begin() ; it != m_serviceSetLocal.end(); ++it)
    if (m_serviceSet->find((*it)) == m_serviceSet->end()){
      msg << MSG::DEBUG << "   Service: " << (*it) << endreq;
      excludeServiceInMaps((*it));
    }
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Excluding Servers from Timer Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  for (itStat=m_serverStatusLocal.begin() ; itStat != m_serverStatusLocal.end(); ++itStat){
    if (!itStat->second) continue; // is already inactive in local dimInfo
    //if ((*m_serverStatus)[itStat->first]) continue; // Server active, we don't have to exclude
    if (m_serverStatus->find(itStat->first)->second) continue; // Server active, we don't have to exclude
    msg << MSG::DEBUG << "   Server: " << itStat->first << endreq;
    excludeServerInMaps(itStat->first);
  }
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Including Servers in Timer Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  for (itStat=m_serverStatus->begin() ; itStat != m_serverStatus->end(); ++itStat){
    if (!itStat->second) continue; // Server inactive, we don't have to include
    //if (m_serverStatusLocal[itStat->first]) continue; // is already active in local dimInfo
    if (m_serverStatusLocal.find(itStat->first) != m_serverStatusLocal.end()) // if it is in local we verify if its active
      if (m_serverStatusLocal.find(itStat->first)->second) continue; // is already active in local dimInfo  
   
    msg << MSG::DEBUG << "   Server: " << itStat->first << endreq;
    includeServerInMaps(itStat->first);
  }
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  msg << MSG::DEBUG << " Including Services in Timer Maps" << endreq;
  msg << MSG::DEBUG << "***************************************************************" << endreq;
  for (it=m_serviceSet->begin() ; it != m_serviceSet->end(); ++it)
    if (m_serviceSetLocal.find((*it)) == m_serviceSetLocal.end()) {
      msg << MSG::DEBUG << "   Service: " << (*it) << endreq;
      includeServiceInMaps((*it));
    }
  //msg << MSG::DEBUG << "copy the Service Set map to local" << endreq;
  m_serviceSetLocal = (*m_serviceSet);
  //msg << MSG::DEBUG << "copy the Server Status map to local" << endreq;
  m_serverStatusLocal = (*m_serverStatus);
  //msg << MSG::DEBUG << "copy the choosed Server Name to local" << endreq;
  m_choosedServerNameLocal = (*m_choosedServerName);
  
  //msg << MSG::DEBUG << "*m_updateTimerAdder = false" << endreq;
  (*m_updateTimerAdder) = false;

}    
  
std::string DimTimerAdder::createAdderName (const std::string &serviceName){
  // Creating the service name for the adder
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
  std::string algo = serviceParts[2];
  std::string object = serviceParts[3];
  std::string task = Misc::splitString(utgid, "_")[1];
  
  std::string adderName = svctype + "/" + m_serverNameAdder + "_Adder_1/" + task + "/";
  
  if ((s_pfixMonH1F==svctype)||(s_pfixMonH2F==svctype)||(s_pfixMonH1D==svctype)||(s_pfixMonH2D==svctype)||(s_pfixMonProfile==svctype))
    adderName = adderName + algo + "/" + object;
  else
    adderName = adderName + object;
  
  return adderName;
}

std::string DimTimerAdder::createTermServiceName (const std::string &serviceName, const std::string &serverName){
  std::string newServiceName = serviceName;
  if (serverName.compare((*m_choosedServerName)) != 0) {
    std::string::size_type posIni = serviceName.find((*m_choosedServerName));
    std::string::size_type posFin = posIni + (*m_choosedServerName).size();
    newServiceName = serviceName.substr(0, posIni) + serverName + serviceName.substr(posFin);
  }
  return newServiceName;
}

void DimTimerAdder::includeServiceInMaps(const std::string &serviceName){
  // Is the same than include an adder, 
  // The input is the serviceName corresponding to the coosed server. We have to create the 
  // serviceNames for the other active servers. For each server we declare a DimInfoMonObjetc 
  // and we put it in a bi-dimensional map: m_dimInfo[adderName, termName]
  // adderName = f(serviceName), termName  = serverName
  MsgStream msg(msgSvc(), name());
  // adderName = f(serviceName), from serviceName we take the svctype and the task name.
  // the adder server name is specified in the options file.
  std::string adderName  = createAdderName (serviceName);
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  msg << MSG::DEBUG << " including adder: " << adderName << endreq;
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  
  // if the adderName exists in the map, we exclude it
  m_dimInfoIt = m_dimInfo.find(adderName);
  if ( m_dimInfoIt != m_dimInfo.end()){
    msg << MSG::DEBUG << "adder already exists, then exclude it" << endreq;
    excludeServiceInMaps(serviceName);
  }
    
  // if no servers, then no Terms
  if (m_serverStatus->size() < 1) {
    msg << MSG::DEBUG << "No servers, then return" << endreq;
    return;
  }
  
  std::map<std::string, bool, std::less<std::string> >::const_iterator itStat;
  // For every server
  for (itStat=m_serverStatus->begin() ; itStat != m_serverStatus->end(); ++itStat){
    if (!itStat->second) continue; // inactive Server, 
    std::string termName = (*itStat).first;
    msg << MSG::DEBUG << "  creating  Term => " << termName << " in Adder => " << adderName << endreq;
    //msg << MSG::DEBUG << "termName:" << termName << endreq;
    std::string newServiceName = createTermServiceName (serviceName, (*itStat).first);
    // Aqui puede ser que necesite verificar y excluir la existencia  //excludeTerm???
    //msg << MSG::DEBUG << "creating DimInfoMonObject => " << newServiceName << endreq;
    //m_dimInfo[adderName][termName] =  new DimInfoMonObject(newServiceName, m_refreshTime, name());
    //msg << MSG::DEBUG << "creating DimInfoMonObject " << newServiceName << endreq;
    m_dimInfo[adderName][termName] = new DimInfoMonObject(newServiceName, m_refreshTime, name());
    //m_dimInfo[adderName][termName]->setMsgSvc(msgSvc()); 
    m_dimInfo[adderName][termName]->setMsgSvc(msgSvc()); 
    //msg << MSG::DEBUG << "creating MonObject " << endreq;
    //m_dimInfo[adderName][termName]->createMonObject();
    //msg << MSG::DEBUG << "creating MonObject in DimInfoMonObject " << endreq;
    m_dimInfo[adderName][termName]->createMonObject();
  } 
  
  // We create the MonObject to be published by Adder DimServiceMonObject...We copy structure 
  // from any MonObject Term. We put MonObject in map m_monObject [adderName, MonObject]
  // and DimServiceMonObject in map m_dimSrv [adderName, DimServiceMonObject]
  MonObject *monObjectTest = m_dimInfo[adderName][(*m_choosedServerName)]->monObject();
  //msg << MSG::DEBUG << "printing MonObjectTest: " << endreq;
  //monObjectTest->print();
  //msg << MSG::DEBUG << "creating MonObject for Adder" << endreq;
  m_monObject[adderName] = MonObjectCreator::createMonObject(monObjectTest->typeName(), msgSvc(), name());
  m_monObject[adderName]->copyFrom(monObjectTest);
  int tsize = MonObjectCreator::monObjectSize(m_monObject[adderName]);
  //msg << MSG::DEBUG << "creating DimServiceMonObject for Adder" << adderName << endreq;
  //msg << MSG::DEBUG << "Size = " << tsize << endreq;
  //msg << MSG::DEBUG << "MonObject: " << tsize << endreq;
  //monObjectAdder->print();
  m_dimSrv[adderName] = new DimServiceMonObject(adderName, m_monObject[adderName], tsize);
 
} 

void DimTimerAdder::includeServerInMaps(const std::string &serverName) {
  MsgStream msg(msgSvc(), name());
  std::string termName = serverName;

  if (m_dimInfo.size() <= 0 ) {
    msg << MSG::DEBUG << "Sorry, there is not active Adders to include the Server " << endreq;
    return;
  }
  msg << MSG::DEBUG << "verifying " << m_dimInfo.size() << " Adders " << endreq;
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt){
    std::string adderName = m_dimInfoIt->first;
    msg << MSG::DEBUG << "adderName = " << adderName << endreq;
    std::string coosedServiceName = m_dimInfoIt->second[m_choosedServerNameLocal]->dimInfo()->getName();
    msg << MSG::DEBUG << "coosedServiceName = " << coosedServiceName << endreq;
    std::string newServiceName = createTermServiceName (coosedServiceName, serverName);
    msg << MSG::DEBUG << "newServiceName = " << newServiceName << endreq;
    // Aqui pode ser que precise verificar e excluir a existencia  //excludeTerm???
    m_dimInfoIt->second[termName] = new DimInfoMonObject(newServiceName, m_refreshTime, name());
    msg << MSG::DEBUG << "setMsgSvc " << endreq;
    m_dimInfoIt->second[termName]->setMsgSvc(msgSvc());
    msg << MSG::DEBUG << "createMonObject " << endreq;
    m_dimInfoIt->second[termName]->createMonObject();
  }    
} 

void DimTimerAdder::excludeServiceInMaps(const std::string &serviceName){
  MsgStream msg(msgSvc(), name());  
  std::string adderName  = createAdderName (serviceName);
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  msg << MSG::DEBUG << " excluding Adder " << adderName << endreq;  
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  msg << MSG::DEBUG << " erasing DimService " << adderName << " from map" <<endreq;  
  delete m_dimSrv[adderName]; m_dimSrv[adderName] = 0; m_dimSrv.erase(adderName);
  msg << MSG::DEBUG << " erasing published MonObject from map" <<endreq;  
  delete m_monObject[adderName]; m_monObject[adderName] = 0; m_monObject.erase(adderName);
  if (m_dimInfo.find(adderName) != m_dimInfo.end()){
    for (m_dimInfo2It=m_dimInfo[adderName].begin(); m_dimInfo2It!=m_dimInfo[adderName].end(); ++m_dimInfo2It){
      msg << MSG::DEBUG << " erasing Term "<< m_dimInfo2It->first <<" from Adder "<< adderName <<endreq;
      msg << MSG::DEBUG << " the serviceName was: "<< m_dimInfo[adderName][m_dimInfo2It->first]->dimInfo()->getName() <<endreq;
      delete m_dimInfo[adderName][m_dimInfo2It->first]; m_dimInfo[adderName][m_dimInfo2It->first]=0;
    }
    msg << MSG::DEBUG << " clearing Adder "<< adderName << endreq;  
    m_dimInfo[adderName].clear();
    msg << MSG::DEBUG << " erasing Adder "<< adderName << " from map "<< endreq;  
    m_dimInfo.erase(adderName);
  }
}

void DimTimerAdder::excludeServerInMaps(const std::string &serviceName, const std::string &serverName){
  MsgStream msg(msgSvc(), name());  
  std::string adderName  = createAdderName (serviceName);
  std::string termName = serverName;
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  msg << MSG::DEBUG << " excluding Adder " << adderName << ", Term " << termName << "fromMap" <<endreq;    
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  if (m_dimInfo.find(adderName) != m_dimInfo.end()) {
    msg << MSG::DEBUG << " sorry there is no Adder " << adderName << " in the map" << endreq;    
    return;
  }
  if (m_dimInfo[adderName].find(termName) != m_dimInfo[adderName].end()){
    msg << MSG::DEBUG << " sorry there is no Term " << termName << "in Adder " << adderName << endreq;    
    return;
  }

  msg << MSG::DEBUG << " erasing Term "<< termName <<" from Adder "<< adderName <<endreq;
  msg << MSG::DEBUG << " the serviceName was: "<< m_dimInfo[adderName][termName]->dimInfo()->getName() <<endreq;
  delete m_dimInfo[adderName][termName]; m_dimInfo[adderName][termName]=0; m_dimInfo[adderName].erase(termName);
  if (m_dimInfo[adderName].size()==0) {
    msg << MSG::DEBUG << " The adder is empty after erasing Term "<< termName << ". We have to erase Adder too." <<endreq;
    msg << MSG::DEBUG << " erasing Adder "<< adderName << " from map "<< endreq;  
    m_dimInfo.erase(adderName);
    msg << MSG::DEBUG << " erasing DimService " << adderName << " from map" <<endreq;  
    delete m_dimSrv[adderName]; m_dimSrv[adderName] = 0; m_dimSrv.erase(adderName);
    msg << MSG::DEBUG << " erasing published MonObject from map" <<endreq;  
    delete m_monObject[adderName]; m_monObject[adderName] = 0; m_monObject.erase(adderName);
  }
}

void DimTimerAdder::excludeServerInMaps(const std::string &serverName){
  MsgStream msg(msgSvc(), name());  
  std::string termName = serverName;
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  msg << MSG::DEBUG << " excluding Term " << termName << " from all Adders" <<endreq;    
  msg << MSG::DEBUG << "-----------------------------------------------------------" << endreq;
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt){
    if (m_dimInfoIt->second.find(termName) == m_dimInfoIt->second.end()) continue;  
    msg << MSG::DEBUG << " erasing Term "<< termName <<" from Adder "<< m_dimInfoIt->first <<endreq; 
    msg << MSG::DEBUG << " the serviceName was: "<< m_dimInfoIt->second[termName]->dimInfo()->getName() <<endreq;
    delete m_dimInfoIt->second[termName]; m_dimInfoIt->second[termName]=0; m_dimInfoIt->second.erase(termName);
    if (m_dimInfoIt->second.size()==0) {
      msg << MSG::DEBUG << " The adder is empty after erasing Term "<< termName << ". We have to erase Adder too." <<endreq;
      msg << MSG::DEBUG << " erasing DimService " << m_dimInfoIt->first << " from map" <<endreq;  
      delete m_dimSrv[m_dimInfoIt->first]; m_dimSrv[m_dimInfoIt->first] = 0; m_dimSrv.erase(m_dimInfoIt->first);
      msg << MSG::DEBUG << " erasing published MonObject from map" <<endreq;  
      delete m_monObject[m_dimInfoIt->first]; m_monObject[m_dimInfoIt->first] = 0; m_monObject.erase(m_dimInfoIt->first);
      msg << MSG::DEBUG << " erasing Adder "<< m_dimInfoIt->first << " from map "<< endreq;  
      m_dimInfo.erase(m_dimInfoIt);
    }
  }
} 
