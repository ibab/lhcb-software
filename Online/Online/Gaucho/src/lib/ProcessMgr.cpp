#include "Gaucho/ProcessMgr.h"
#include "Gaucho/DimInfoServers.h"
#include "Gaucho/DimInfoServices.h"
#include "Gaucho/BaseServiceMap.h"
#include "Gaucho/DimTimerProcess.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/Misc.h"
#include "CPP/Interactor.h"
#include "TFile.h"

ProcessMgr::ProcessMgr(std::string serviceOwner, IMessageSvc* msgSvc, Interactor *service, const int &refreshTime): m_serviceOwner(serviceOwner), m_name("ProcessMgr"), m_msgSvc(msgSvc), m_service(service), m_refreshTime(refreshTime)
{
  m_monitoringFarm = false;
  m_publishRates = false;
  m_fileName = "Waiting for command to save histograms............."; 
}

ProcessMgr::~ProcessMgr() {
  m_dimTimerProcess->stop();
  if (m_dimTimerProcess) {delete m_dimTimerProcess; m_dimTimerProcess=0;}
  if (m_dimInfoServices) {delete m_dimInfoServices; m_dimInfoServices=0;}
  if (m_dimInfoServers) {delete m_dimInfoServers; m_dimInfoServers=0;}
  if (m_serviceMap) {delete m_serviceMap; m_serviceMap=0;}
}

void ProcessMgr::updateMap(){
  m_serviceMap->updateMap(m_dimInfoServers->serverMap());
}

void ProcessMgr::createInfoServers() {
  MsgStream msg(msgSvc(), name());
  m_serviceMap = new BaseServiceMap(this);

  msg << MSG::DEBUG << "Creating Server Status"<< endreq;
  m_dimInfoServers = new DimInfoServers(this);
}

void ProcessMgr::destroyInfoServers() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Destroying Server Status"<< endreq;
  if (m_dimInfoServers) {delete m_dimInfoServers; m_dimInfoServers=0;}
  msg << MSG::DEBUG << "Destroying Service Map"<< endreq;
  if (m_serviceMap) {delete m_serviceMap; m_serviceMap=0;}
}


void ProcessMgr::createInfoServices(std::string serverName){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Creating Service Status servername "<< serverName << endreq;
  m_dimInfoServices = new DimInfoServices(this, serverName);
}

void ProcessMgr::destroyInfoServices(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Destroying Service Status "<< endreq;
  if (m_dimInfoServices) {delete m_dimInfoServices; m_dimInfoServices=0;}
}

void ProcessMgr::createTimerProcess() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Creating DimTimerAdder"<< endreq;
  m_dimTimerProcess = new DimTimerProcess (this, m_refreshTime, msgSvc());
}

void ProcessMgr::destroyTimerProcess() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Destroying DimTimerAdder"<< endreq;
  if (m_dimTimerProcess) {delete m_dimTimerProcess; m_dimTimerProcess=0;}
}

void ProcessMgr::timerHandler(){
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "inside timerHandler"<< endreq;

  if (m_serviceOwner.compare(s_Adder) == 0){
    msg << MSG::DEBUG << "isAdder"<< endreq;
    m_serviceMap->add();
  }
  else if (m_serviceOwner.compare(s_Saver) == 0) { //it's a saver May be we have to save every deltaT
    msg << MSG::DEBUG << "isSaver"<< endreq;
    //std::string fileName = "from timerHandler";
    //int fileSize = 0;
    //m_serviceMap->write(m_saveDir, *m_pFile);
    msg << MSG::DEBUG << "Before Save hiostograms in file: "<< m_fileName << endreq;
    write();
    msg << MSG::DEBUG << "After Save histograms in file: "<< m_fileName << endreq;
    //m_pGauchoMonitorSvc->updateAll(false);
    //msg << MSG::DEBUG << "After UpdateAll in Monitoring Service: "<< m_fileName << endreq;
  }
}

void ProcessMgr::write(){
  m_serviceMap->write(m_saveDir, m_fileName);
}

void ProcessMgr::setUtgid(const std::string &utgid)
{
  m_utgid = utgid;
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  std::size_t third_us = m_utgid.find("_", second_us + 1);
  int tmplen=second_us-first_us-1;
  if (third_us != std::string::npos) {
     // three underscores found in UTGID -> we're in the Monitoring Farm
     m_nodeName = m_utgid.substr(first_us+1,tmplen-2);
  }   
  else {
     m_nodeName = m_utgid.substr(0, first_us);
  }
}  


void ProcessMgr::setPartitionName(const std::string &partitionName)
{
  m_partitionName = partitionName;
}  
  
void ProcessMgr::updateServiceSet(std::string &dimString, std::set<std::string> &serviceSet) {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  msg << MSG::DEBUG << "******Possible changes in ServiceSet. We will update the ServiceSet******"<< endreq;
 
 // msg << MSG::DEBUG << "DimString = " << dimString << endreq;
  
  std::vector<std::string> serviceList = Misc::splitString(dimString, "\n");
  std::vector<std::string>::const_iterator serviceListIt;
  
  for(serviceListIt=serviceList.begin(); serviceListIt!=serviceList.end(); ++serviceListIt){
    
    std::string serviceName = (*serviceListIt).substr(0, (*serviceListIt).find("|"));
    std::string oper = serviceName.substr(0, 1);
    if ((oper.compare("+") == 0)||(oper.compare("-") == 0)) serviceName = serviceName.substr(1);
    
    bool includeService = true;
    if (oper.compare("-") == 0) includeService = false;
    
    msg << MSG::DEBUG << "Verifying service = " << serviceName << ". This is an " << m_serviceOwner <<   endreq;
    if (m_serviceOwner.compare(s_Adder)==0){
      msg << MSG::DEBUG << "Trying to decode Service List in Adder with " << m_subfarmName.size() << " subfarms in joboptions" << endreq;
      std::vector<std::string>::const_iterator it;
      for(it=m_subfarmName.begin(); it!=m_subfarmName.end(); ++it){
        msg << MSG::DEBUG << "subfarmName "<< (*it) << endreq;
      }
      
      if (m_subfarmName.size() != 0) {
        bool chooseIt=false;
        std::vector<std::string>::const_iterator it1;
        for(it1=m_subfarmName.begin(); it1!=m_subfarmName.end(); ++it1){
          msg << MSG::DEBUG << "verifying subfarmName in serviceName "<< serviceName << endreq;
	  //need to append underscore for 3rd level adder, and it should be not equal when it's found
          if (serviceName.find("/" + (*it1) + "_") != std::string::npos){
           chooseIt=true;
           break;
          }
        }
        if (!chooseIt) {
          msg << MSG::DEBUG << "REFUSED because subfarmName NOT OK" << endreq;
          continue;
        }
        else msg << MSG::DEBUG << "subfarmName OK" << endreq;
      }
      else {
        if (!m_monitoringFarm) {
	 if (m_farm=="EFF") {
	    if (serviceName.find("/" + m_nodeName) == std::string::npos) {
               msg << MSG::DEBUG << "REFUSED because partition name NOT OK nodename: " << m_nodeName << " serviceName "<< serviceName << endreq;
               continue;
             }
	 }    
	 else {
	    if (serviceName.find("_" + m_nodeName) == std::string::npos) {
               msg << MSG::DEBUG << "REFUSED because partition name NOT OK nodename: " << m_nodeName << " serviceName "<< serviceName << endreq;
               continue;
             }
	   }     
         }
       }
    }
    else if (m_serviceOwner.compare(s_Saver)==0){
      if (!m_monitoringFarm) {
        //here we need to check for m_partName[1] (only 1 partition at a time in EFF), not the nodename
        //if (serviceName.find("/" + m_nodeName) == std::string::npos) {
	msg << MSG::DEBUG << "checking for partitionname in servicename " << endreq;
	msg << MSG::DEBUG << "m_partName.size " <<    m_partName.size() << " partname "<< m_partName[0] << endreq;	
	if (serviceName.find("/" + m_partName[0]) == std::string::npos) {	
          msg << MSG::DEBUG << "REFUSED because partition name not found in servicename" << endreq;
          continue;
        }
      }
    }

    std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");

    //it should be possible to have an unlimited number of slashes im the histogram names
    if (serviceParts.size() < 4 ) continue; 
    std::string svctype = serviceParts[0];
    msg << MSG::DEBUG << "svctype = " << svctype << endreq;


    if ((svctype.compare(s_pfixMonH1F) != 0)&&(svctype.compare(s_pfixMonH1D) != 0)&&(svctype.compare(s_pfixMonH2F) != 0)&&
    (svctype.compare(s_pfixMonH2D) != 0)&&(svctype.compare(s_pfixMonProfile) != 0)&&(svctype.compare(s_pfixMonRate) != 0)) {
      msg << MSG::DEBUG << "REFUSED because Service is not MonHisto, MonProfile, MonRate: " << svctype << endreq;
      continue;
    }

    if (s_Saver == m_serviceOwner) { // savers do not save MonRate
       if (svctype.compare(s_pfixMonRate) == 0 ) {
        msg << MSG::DEBUG << "REFUSED because Savers do not Save MonRate !!" << endreq;
        continue; 
      } 
    }

    std::string utgid = serviceParts[1];
    msg << MSG::DEBUG << "utgid = " << utgid << endreq;

    std::string task;
    std::string part;
    int index=2;
    if (m_farm=="MF") {
      task = Misc::splitString(utgid, "_")[2];
      if ((task.compare("Saver")==0)||(task.compare("SAVER")==0)) continue; 
      part = Misc::splitString(utgid, "_")[0];
    }
    else {
      task = Misc::splitString(utgid, "_")[1];
      // we don't do nothing with Savers
      if ((task.compare("Saver")==0)||(task.compare("SAVER")==0)) continue; 

      if ((task.compare("Adder")==0)||(task.compare("ADDER")==0)) {
        if (serviceParts.size() < 5 ) continue; 
        task = serviceParts[index];
        index++;
      }
    }
    
    msg << MSG::DEBUG << "task = " << task << endreq;
    std::string algo = serviceParts[index];
    msg << MSG::DEBUG << "algo = " << algo << endreq;
    
    std::string object = serviceParts[index+1];
    for (unsigned int i = index+2; i < serviceParts.size(); ++i)
      object += "/" + serviceParts[i];
    
    msg << MSG::DEBUG << "object = " << object << endreq;
    
    msg << MSG::DEBUG << "task match : " << m_taskName << " vs " << task << endreq;
    if (Misc::findCaseIns(m_taskName, task) == std::string::npos) continue;
    
    bool matched = true;
    if (m_algorithmName.size() > 0)  matched = false;
    for(unsigned int i = 0; i < m_algorithmName.size(); i++) {
      msg << MSG::DEBUG << "algo match : " << m_algorithmName[i] << " vs " << algo << endreq;
      if (Misc::findCaseIns(m_algorithmName[i], algo) != std::string::npos) {
        matched = true;
        break;
      }
    }
    if (!matched) continue;

    matched = true;
    if (m_objectName.size() > 0)  matched = false;
    for(unsigned int i = 0; i < m_objectName.size(); i++) {
      msg << MSG::DEBUG <<  "object match : " << m_objectName[i] << " vs " << object  << endreq;
      if (m_objectName[i].find("*", 0) != std::string::npos) {
        std::vector<std::string> objectNameParts = Misc::splitString(m_objectName[i], "*");
        if (Misc::matchStringCaseIns(object, objectNameParts)) {
          matched = true;
          break;
        }
      }
      else{// No wildcard, then object must be equal to m_objectName[i]
        if (Misc::findCaseIns(m_objectName[i], object) != std::string::npos) {
          matched = true;
          break;
        }
      }
    }
    if (!matched) continue;

    if (includeService) {
      msg << MSG::DEBUG << "service INCLUDED =======> " << serviceName << endreq;
      serviceSet.insert(serviceName);
    }
    else {
      msg << MSG::DEBUG << "service EXCLUDED =======> " << serviceName << endreq;
      serviceSet.erase(serviceName);
    }
  }
  msg << MSG::DEBUG << "***************The process to update the ServiceSet is finished**********"<< endreq;
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;

}

void ProcessMgr::updateServerMap(std::string &dimString, std::map<std::string, bool, std::less<std::string> > &serverMap) {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  msg << MSG::DEBUG << "*******Posible changes in ServerMap. We will update the ServerMap********"<< endreq;
  
 // msg << MSG::DEBUG << "DimString = " << dimString << endreq;
  
  std::string value = dimString;
  std::string oper = value.substr(0, 1);
  if ((oper.compare("+") == 0)||(oper.compare("-") == 0)) value = value.substr(1);

  bool activeStatus = true;
  if (oper.compare("-") == 0) activeStatus = false;

  msg << MSG::DEBUG << "Verifying Server List"<< endreq;
  std::set<std::string> server = decodeServerList(value);
  std::set<std::string>::iterator it;
  
  if (server.size() > 0)
    msg << MSG::DEBUG << "Detected changes in Server List"<< endreq;
  else {
    msg << MSG::DEBUG << "Sorry no new Server"<< endreq;
    msg << MSG::DEBUG << "*********************************************"<< endreq;
    return;
  }

  msg << MSG::DEBUG << "Updating Server Status"<< endreq;
  
  for(it=server.begin(); it!=server.end(); ++it) serverMap[(*it)] = activeStatus;
  
  msg << MSG::DEBUG << "***************The process to update the ServerMap is finished***********"<< endreq;
  msg << MSG::DEBUG << "*************************************************************************"<< endreq;
  
}

std::set<std::string> ProcessMgr::decodeServerList(const std::string &serverListS)
{
  MsgStream msg(msgSvc(), name());
  std::set<std::string> serverList;
  std::vector<std::string> serverListTot = Misc::splitString(serverListS, "|");
  std::vector<std::string>::const_iterator serverListTotIt;
  std::vector<std::string>::const_iterator it;
  
  for(serverListTotIt = serverListTot.begin(); serverListTotIt!=serverListTot.end(); ++serverListTotIt) {
    //msg << MSG::DEBUG << "checking server="<<(*serverListTotIt)<< endreq;
    
    std::size_t first_us = (*serverListTotIt).find("_");
    if (first_us == std::string::npos) continue;
    
    std::size_t second_us = (*serverListTotIt).find("_", first_us + 1);
    if (second_us == std::string::npos) continue;
    
    std::size_t third_us;
    if (m_farm=="MF") {
        third_us = (*serverListTotIt).find("_", second_us + 1);	   
    }	   

    std::string partName;
    std::string nodeName;
    std::string taskName;

    if (m_serviceOwner.compare(s_Adder)==0){
      msg << MSG::DEBUG << " We are inside an ADDER " << endreq;
      if (m_farm=="EFF") {
         nodeName = (*serverListTotIt).substr(0, first_us);
         taskName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
      }
      else { 	 
         partName = (*serverListTotIt).substr(0, first_us);
         nodeName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
	 if (nodeName=="MONA0901") continue;
         taskName = (*serverListTotIt).substr(second_us + 1, third_us - second_us - 1);
      } 	 
      if ((taskName.compare("Saver")==0)||(taskName.compare("SAVER")==0)) {
        msg << MSG::DEBUG << "refused because we don't add SAVERS. "<< endreq;    
        continue; // we don't do nothing with Savers
      }
      // First level Adder => m_nodeName=HLTA0101. It must Add Gaucho Jobs: nodeNames=HLTA0101 and TaskNames != Adders
      // Second level Adder => m_nodeName=HLTA01 . It must Add 1st level Adders: nodeNames=HLTA0101 and TaskNames == Adder
      // Third level Adder (Top level Adders) => m_nodeName=PARTxx. 
      // It must Add 2nd level Adders: nodeNames=HLTXn (X=A,B,C...; n=01,02...) And check the Subfarm names

      if (m_nodeName.size() == 8) { //1st level Adder
        // checking the nodeName
        msg << MSG::DEBUG << "comparing nodeName="<< nodeName << " with "<< m_nodeName << endreq;    
        if ((Misc::findCaseIns(m_nodeName, nodeName) == std::string::npos)&(m_farm !="MF")){
          msg << MSG::DEBUG << "REFUSED because nodeName NOT OK" << endreq;
          continue;
        }
        else msg << MSG::DEBUG << "nodeName OK" << endreq;
       // This is a 1st level Adder, then we can check the task name here.
        if ((Misc::findCaseIns(m_taskName, taskName) == std::string::npos)&(m_farm !="MF")) {
          msg << MSG::DEBUG << "REFUSED because taskName "<< taskName << " NOT OK should be " << m_taskName << endreq;
          continue; 
        }
        else msg << MSG::DEBUG << "taskName OK" << endreq;
      }
      else if (((m_nodeName.size() == 6)&&(m_nodeName.substr(0,4)!="PART"))&&(m_farm=="EFF")) { //2nd level Adder
        if ((taskName.compare("Adder")!=0)&&(taskName.compare("ADDER")!=0)){
          msg << MSG::DEBUG << "REFUSED because 2nd level adders add only adders. "<< endreq;
          continue;
        }
        if (nodeName.size() <=  m_nodeName.size()) {
          msg << MSG::DEBUG << "REFUSED because 2nd level adders add only 1rst level adders (nodename must be longer)"<< endreq;
          continue;
        }
        nodeName = nodeName.substr(0, m_nodeName.size()); // we resize the nodename to do the match.
        msg << MSG::DEBUG << "nodeName="<< nodeName << endreq;
        // checking the nodeName
        msg << MSG::DEBUG << "comparing nodeName="<< nodeName << " with "<< m_nodeName << endreq;    
        if ((Misc::findCaseIns(m_nodeName, nodeName) == std::string::npos)||(m_nodeName=="MONA0901")){
          msg << MSG::DEBUG << "REFUSED because nodeName not OK" << endreq;
          continue;
        }
        else msg << MSG::DEBUG << "nodeName OK" << endreq;
      }
      else if  ((m_nodeName.size() == 6)&&((m_nodeName.substr(0,4)=="PART")||(partName=="LHCb"))) { //3rd level Adder (Top Level Adder)
       // if (nodeName.compare("Bridge")!=0) {
       //   msg << MSG::DEBUG << "REFUSED because it is a Top Level Adder and it must add only Bridges. "<< endreq;
       //   continue;
      //  }
        // checking the nodename is not necessary because the bridges are already selected.
        // Then we only filter by subfarmName 
       // std::string subfarmName = taskName;
	std::string subfarmName = nodeName;
        bool chooseIt=true;
        if (m_subfarmName.size()) chooseIt=false;
        for(it=m_subfarmName.begin(); it!=m_subfarmName.end(); ++it){
          msg << MSG::DEBUG << "comparing subfarmName="<< subfarmName << " with "<< (*it) << endreq;
          if (Misc::findCaseIns((*it), subfarmName) != std::string::npos) {
            chooseIt=true;
            break;
          } 
        }
        if (!chooseIt) {
          msg << MSG::DEBUG << "REFUSED because subfarmName NOT OK" << endreq;
          continue;
        }
        else msg << MSG::DEBUG << "subfarmName OK" << endreq;
      }
    }
    else if (m_serviceOwner.compare(s_Saver)==0){
      msg << MSG::DEBUG << " We are inside a SAVER " << endreq;
      if (m_monitoringFarm) { 
        std::size_t third_us = (*serverListTotIt).find("_", second_us + 1);
        if (third_us == std::string::npos) continue;
      
    //    partName = (*serverListTotIt).substr(0, first_us);
    //    nodeName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
    //    taskName = (*serverListTotIt).substr(second_us + 1, third_us - second_us - 1);
	
        msg << MSG::DEBUG << "partname "<< partName << " nodename " << nodeName << "   taskname " << taskName << endreq;
        
        bool chooseIt=true;
        if (m_partName.size()) chooseIt=false;
        for(it=m_partName.begin(); it!=m_partName.end(); ++it){
          msg << MSG::DEBUG << "comparing partName="<< partName << " with "<< (*it) << endreq;
          if (Misc::findCaseIns((*it), partName) != std::string::npos) {
            chooseIt=true;
            break;
          }
        }
        if (!chooseIt) {
          msg << MSG::DEBUG << "REFUSED because partName NOT OK " <<(*serverListTotIt) << endreq;
          continue; 
        }
        else msg << MSG::DEBUG << "partName OK server accepted: " << (*serverListTotIt) <<endreq;

        if (m_taskName.compare(taskName)!= 0) {
          msg << MSG::DEBUG << "REFUSED because taskName NOT OK" << endreq;
          continue; 
        }
      }
      else {
     //   nodeName = (*serverListTotIt).substr(0, first_us);
     //   taskName = (*serverListTotIt).substr(first_us + 1, second_us - first_us - 1);
       msg << MSG::DEBUG << "partname "<< partName << " nodename " << nodeName << "   taskname " << taskName << endreq;
 
        if (nodeName.compare("Bridge")==0) {
          msg << MSG::DEBUG << "refused because we don't save Bridges. "<< endreq;    
          continue; 
        }
	//inside EFF should have nodename = partition
	 msg << MSG::DEBUG << "checking partition= "<< nodeName << endreq;
	if (nodeName.compare(m_nodeName)!=0) {
          msg << MSG::DEBUG << "refused saver for partition " << m_nodeName << " ignores partition "<< nodeName << endreq;    
          continue; 
        }

        msg << MSG::DEBUG << "checking taskname= "<< taskName << endreq;
        if ((taskName.compare("Adder")!=0)&&(taskName.compare("ADDER")!=0)) {
          msg << MSG::DEBUG << "REFUSED because in the EFF savers can save only adders. Taskname= "<< taskName << endreq;
          continue; 
        }
      }
    }

    std::string serverName = (*serverListTotIt).substr(0, (*serverListTotIt).find("@"));
    msg << MSG::DEBUG << "We will consider -----------> Server = "<<serverName << endreq;
    serverList.insert(serverName);

  }  

  return serverList;
}
