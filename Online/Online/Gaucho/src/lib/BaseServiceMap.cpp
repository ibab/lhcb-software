#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonRate.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/DimInfoServers.h"
#include "Gaucho/ProcessMgr.h"
#include "Gaucho/BaseServiceMap.h"
#include "Gaucho/Misc.h"
#include "Gaucho/MonRateDecoder.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "RTL/rtl.h"

BaseServiceMap::BaseServiceMap(ProcessMgr *processMgr): 
  m_name("BaseServiceMap"),
  m_processMgr(processMgr)
{
m_saved=false;
}

BaseServiceMap::~BaseServiceMap() {
  removeMap();
}

void BaseServiceMap::removeMap() {
  MsgStream msg(m_processMgr->msgSvc(), name());
  std::map<std::string, bool, std::less<std::string> >::iterator svrMapIt;
  for (svrMapIt=m_serverMap.begin() ; svrMapIt != m_serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // is already inactive in local dimInfo
    excludeServerInMaps(svrMapIt->first);
  }
}

IMessageSvc* BaseServiceMap::msgSvc() 
{
  return m_processMgr->msgSvc();
}

void BaseServiceMap::printServiceSet() {
 MsgStream msg(m_processMgr->msgSvc(), name());
 std::set<std::string>::iterator it;
 for (it = m_serviceSet.begin(); it != m_serviceSet.end(); ++it){
   msg << MSG::INFO << "Service " << *it << endreq;    
  }
}

void BaseServiceMap::updateMap(std::map<std::string, bool, std::less<std::string> > serverMap)
{
  MsgStream msg(msgSvc(), name());
  
  if (0 == m_serviceSet.size()) return;
  if (serverMap == m_serverMap) return;
  
  std::map<std::string, bool, std::less<std::string> >::iterator svrMapIt;
  
// printServiceSet();
    

  for (svrMapIt=m_serverMap.begin() ; svrMapIt != m_serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // is already inactive in local dimInfo
    if (serverMap.find(svrMapIt->first)->second) continue; // Server active, we don't have to exclude
    excludeServerInMaps(svrMapIt->first);
  }
 for (svrMapIt=serverMap.begin() ; svrMapIt != serverMap.end(); ++svrMapIt){
    if (!svrMapIt->second) continue; // Server inactive, we don't have to include
    if (m_serverMap.find(svrMapIt->first) != m_serverMap.end()) // if it is in local we verify if its active
      if (m_serverMap.find(svrMapIt->first)->second) continue; // is already active in local dimInfo  
   
    includeServerInMaps(svrMapIt->first);
  }
  
  m_serverMap = serverMap;
}

void BaseServiceMap::includeServerInMaps(const std::string &serverName) {
  MsgStream msg(msgSvc(), name());

  std::set<std::string>::iterator svcSetIt;  

  bool insertDimSvc = false;

  if (m_processMgr->serviceOwner() == s_Adder) {
    if (m_dimInfo.size() <= 0) insertDimSvc = true;
  }

  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    insertDimInfo(*svcSetIt, serverName);    
  }

  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    loadDimInfo(*svcSetIt, serverName);
    if (insertDimSvc) {
     insertDimService(*svcSetIt, serverName);
    } 
  }
} 

void BaseServiceMap::printMap() {
  MsgStream msg(msgSvc(), name());
  std::map<std::string, DimInfoMonObject*>::iterator it; 
  
  for (m_dimInfoIt = m_dimInfo.begin(); m_dimInfoIt != m_dimInfo.end(); ++m_dimInfoIt){
    msg << MSG::INFO << "Grupo: " << m_dimInfoIt->first  << endreq;
    for (it = m_dimInfoIt->second.begin(); it != m_dimInfoIt->second.end(); ++it) {
      msg << MSG::INFO << "         Elemento: " << it->first << endreq;
    }
  }
}

void BaseServiceMap::excludeServerInMaps(const std::string &serverName) {
  MsgStream msg(msgSvc(), name());

  std::set<std::string>::iterator svcSetIt;  
  
  for (svcSetIt = m_serviceSet.begin(); svcSetIt != m_serviceSet.end(); ++svcSetIt) {
    int numLastInfos = deleteDimInfo(*svcSetIt, serverName);
    if ( 0 == numLastInfos) {
      if (m_processMgr->serviceOwner() == s_Adder) deleteDimService(*svcSetIt);
    }
  }
} 

void BaseServiceMap::insertDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder){
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    termSvcName = serviceName;
    groupName = createSaverName (termSvcName);// in the case of Saving Adders it should return "serverName"
    elementName = termSvcName;
  }
  
  
  m_dimInfo[groupName][elementName] = new DimInfoMonObject(termSvcName, m_processMgr->refreshTime()); 
  m_dimInfo[groupName][elementName]->setSourceName(name());
  m_dimInfo[groupName][elementName]->setMsgSvc(msgSvc());

}

void BaseServiceMap::loadDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());
  
  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    termSvcName = serviceName;
    groupName = createSaverName (termSvcName);// in the case of Saving Adders it should return "serverName"
    elementName = termSvcName;
  }
  
  
  bool created = false;  
  int maxtries=5;
  //reduced this to avoid Adder timeout on stoprun in RecFarm; before was mxtries=10, nsleep 10.
  int tries=0;
  while (!created) {
    created = m_dimInfo[groupName][elementName]->createMonObject();  
    usleep(1);
    tries++;
    if (tries > maxtries) break;
  }
  
  if (m_processMgr->serviceOwner() == s_Saver)
  {// If it's a Saver we also create the object
    if (0 != m_dimInfo[groupName][elementName]->monObject()){ 
      
      std::vector<std::string> serviceParts = Misc::splitString(termSvcName, "/");
      std::string newName="";
      int n = serviceParts.size();

      newName = serviceParts[2];
      for (int i=3; i < n; i++) {
        newName = newName + "/" + serviceParts[i];
      }

      std::string svctype = serviceParts[0];

      if (svctype.compare(s_pfixMonH1F) == 0){
       // newName = ((MonH1F*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
        // newName = m_processMgr->taskName() + "/"+ newName;
          ((MonH1F*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH1D) == 0){
       // newName = ((MonH1D*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
        ((MonH1D*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH2F) == 0){
       // newName = ((MonH2F*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
       ((MonH2F*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonH2D) == 0){
       // newName = ((MonH2D*) (m_dimInfo[groupName][elementName]->monObject()))->histName();
       // newName = m_processMgr->taskName() + "/"+ newName;
        ((MonH2D*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else if (svctype.compare(s_pfixMonProfile) == 0){
      // newName = ((MonProfile*) (m_dimInfo[groupName][elementName]->monObject()))->profileName();
       // newName = m_processMgr->taskName() + "/"+ newName;
       ((MonProfile*) (m_dimInfo[groupName][elementName]->monObject()))->createObject(newName);
      } else {
        msg << MSG::WARNING << "MonObject not included in the Saver: " << svctype << endreq;
      }
    }
  }
}

int BaseServiceMap::deleteDimInfo(const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::string termSvcName = createTermServiceName (serviceName, serverName);

  std::string groupName = "";
  std::string elementName = "";

  if (m_processMgr->serviceOwner() == s_Adder) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    groupName = serverName;
    elementName = termSvcName;
  }

  if (m_dimInfo.find(groupName) == m_dimInfo.end()){
    return 1;
  }

  if (m_dimInfo[groupName].find(elementName) == m_dimInfo[groupName].end()){
    return 1;
  }

  delete m_dimInfo[groupName][elementName]; m_dimInfo[groupName][elementName]=0; m_dimInfo[groupName].erase(elementName);

  if (m_dimInfo[groupName].size()!=0)  return 1;

  m_dimInfo.erase(groupName);

  return 0;
}

void BaseServiceMap::insertDimService(const std::string &serviceName, const std::string &serverName)
{

  MsgStream msg(msgSvc(), name());

  std::string adderName = createAdderName (serviceName);

  std::string groupName = "";
  std::string elementName = "";

  if (m_processMgr->serviceOwner() == s_Adder) {
    groupName = createAdderName (serviceName);
    elementName = serverName;
  }
  else {
    msg << MSG::WARNING << "This is not an Adder !!!!!!!!. You can not add DimServices." << endreq;
    return;  
  }

  MonObject *monObjectTmp = m_dimInfo[groupName][elementName]->monObject();
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];

  MonObject *monObjectAdder = MonObjectCreator::createMonObjectWithPrefix(svctype, msgSvc(), name());

  bool isCopied = false;
  if (monObjectTmp !=0) {
    monObjectAdder->copyFrom(monObjectTmp);
    isCopied = true;
  }
  DimServiceMonObject *dimServiceMonObjectAdder = new DimServiceMonObject(groupName, monObjectAdder);

  m_dimSrv[groupName] = std::pair<DimServiceMonObject*, MonObject*> (dimServiceMonObjectAdder, monObjectAdder);
  m_dimSrvStatus[groupName] = std::pair<bool, std::string> (isCopied, elementName);

  if (svctype.compare (s_pfixMonRate) == 0) {
    if (m_processMgr->publishRates())  {
      m_monRateDecoder = new MonRateDecoder(m_processMgr->msgSvc(), m_processMgr->utgid(), groupName);
    }
  }
}

void BaseServiceMap::deleteDimService(const std::string &serviceName){
  MsgStream msg(msgSvc(), name());
  std::string adderName = createAdderName (serviceName);
  
  std::string groupName = "";
  
  if (m_processMgr->serviceOwner() == s_Adder) groupName = createAdderName (serviceName);
  else {
    msg << MSG::WARNING << "This is not an Adder !!!!!!!!. You can not delete DimServices." << endreq;
    return;  
  }
  
  delete m_dimSrv[groupName].second; 
  m_dimSrv[groupName].second = 0;
  delete m_dimSrv[groupName].first; 
  m_dimSrv[groupName].first = 0; 
  m_dimSrv.erase(groupName);
}


std::string BaseServiceMap::createTermServiceName (const std::string &serviceName, const std::string &serverName)
{
  MsgStream msg(msgSvc(), name());

  std::size_t first_slash = serviceName.find("/");
  std::size_t second_slash = serviceName.find("/", first_slash + 1);
  if (serverName.find("Bridge")!= std::string::npos) {
    std::vector<std::string> parts = Misc::splitString(serverName, "_");
    std::string subfarmName = Misc::stringToUpper(parts[1]);    
    return serviceName.substr(0, first_slash + 1) + subfarmName + "_Adder_1" + serviceName.substr(second_slash);
  }
  else{
   return serviceName.substr(0, first_slash + 1) + serverName + serviceName.substr(second_slash);
  }
}    
    
std::string BaseServiceMap::createAdderName (const std::string &serviceName){
  // Creating the service name for the adder
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
  std::string task = Misc::splitString(utgid, "_")[1];
  std::string farm=m_processMgr->getFarm();
  if (farm=="MF") task = "Brunel" ;
    
  int index = 2;

  if ((task == "Adder")||(task == "ADDER")) {
    task = serviceParts[index];
    index++;
  }
      
  std::string algo = serviceParts[index];
  std::string object = serviceParts[index+1];
  for (unsigned int i = index+2; i < serviceParts.size(); ++i)
    object += "/" + serviceParts[i];

  std::string partitionName = Misc::splitString(m_processMgr->utgid(),"_")[0];
  std::string adderName="";
  if (partitionName.find("PART")!= std::string::npos) {
    adderName = svctype + "/" + m_processMgr->getPartitionName() + "_Adder_1/" + task + "/";
  }
  else {
     if (farm=="MF") {
        //in the monitoring farm, remove the nodename so the presenter can handle online/offline Brunel difference
	std::string recfarmAdderName =   Misc::splitString(m_processMgr->utgid(),"_")[2];
	std::string instance =  Misc::splitString(m_processMgr->utgid(),"_")[3];
        adderName = svctype + "/" + partitionName + "_" + recfarmAdderName + "_" + instance + "/" + task + "/";
	//msg << MSG::INFO << " adderName = " << adderName <<endreq; 
     }
     else adderName = svctype + "/" + m_processMgr->utgid()  + "/" + task + "/";  }
  if ((s_pfixMonH1F==svctype)||(s_pfixMonH2F==svctype)||(s_pfixMonH1D==svctype)||(s_pfixMonH2D==svctype)||(s_pfixMonProfile==svctype)||(s_pfixMonRate==svctype)||(s_pfixMonStatEntity==svctype))
    adderName = adderName + algo + "/" + object;
  else
    adderName = adderName + object;
  
  return adderName;
}

std::string BaseServiceMap::createSaverName (const std::string &serviceName){
  // Creating the task name for the saver
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  std::string svctype = serviceParts[0];
  std::string utgid = serviceParts[1];
//  std::string utgid = serviceParts[0];
  std::vector<std::string> utgidParts = Misc::splitString(utgid, "_");
  if (utgidParts.size() == 4) {
    std::string partName = utgidParts[0];
    std::string taskName = utgidParts[2];
    return partName + "-" + taskName;
  }
 // if (utgidParts[0].find("PART")!= std::string::npos) {
 //   return m_processMgr->getPartitionName() + "_Saver_1/";
 // }
  return utgid; // it should be equal to the serverName
}



bool BaseServiceMap::write(std::string saveDir, std::string &fileName, int runNumber)
{
  MsgStream msg(msgSvc(), name());

  if ((0 == m_serverMap.size())||(0 == m_serviceSet.size())||(0 == m_dimInfo.size())) {
    msg << MSG::DEBUG << " Writer can't write because ServerMap, ServiceMap or DimInfoMap is empty " << endreq;
    return false;
  }
   //get the runnumber
  std::vector<std::string> savedirParts = Misc::splitString(saveDir, "/");
  std::string tmpPart=savedirParts[savedirParts.size()-1];

  std::string runNumberstr;
  std::stringstream outstr;
  outstr << runNumber;
  runNumberstr=outstr.str();
  std::map<std::string, DimInfoMonObject*>::iterator it;

  TFile *f=0;

  char timestr[64];
  char year[5];
  char month[3];  
  char day[3];  

  time_t rawTime=time(NULL);
  struct tm* timeInfo = localtime(&rawTime);
  ::strftime(timestr, sizeof(timestr),"%Y%m%dT%H%M%S", timeInfo);
  ::strftime(year, sizeof(year),"%Y", timeInfo);
  ::strftime(month, sizeof(month),"%m", timeInfo);
  ::strftime(day, sizeof(day),"%d", timeInfo);
  bool save=false;
  
  int mins;
  mins=timeInfo->tm_min;
  int secs;
  secs=timeInfo->tm_sec;
  if ((mins==0)||(mins==15)||(mins==30)||(mins==45)) save=true;
  bool endofrun=false;
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt) 
  {   
     std::vector<std::string> serviceParts = Misc::splitString(m_dimInfoIt->first, "_");
     std::string farm=m_processMgr->getFarm();
     //Moore breaks Presenter history mode
     std::string taskName="GauchoJob";
     if (farm=="MF") {
        taskName = "Brunel";
     }  
     for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
        //check if all contributions have the end of run flag
        it->second->loadMonObject();
        endofrun = it->second->monObject()->endOfRun(); 
     }
     std::string eorstring="";
     if (endofrun) {
        eorstring="-EOR";
        save=true;
     }
     else m_saved=false;   
     if ((save==true)&&(m_saved==false)) {         
        std::string tmpfile = saveDir + taskName + "/" + month +"/" +day + "/"+ taskName + "-" + runNumberstr +"-"+ timestr + eorstring + ".root";
        fileName.replace(0, fileName.length(), tmpfile);
        std::string dirName = saveDir + taskName + "/" + month +"/" +day ;  
        void *hdir = gSystem->OpenDirectory(dirName.c_str());
        if (hdir == 0) {
           gSystem->mkdir(dirName.c_str(),true);
        }

        f = new TFile(fileName.c_str(),"create");
	
        std::vector<TDirectory*> dirs;
	dirs.clear();   
        for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
           std::string serverName = Misc::splitString(it->first, "/")[1];
           if(0 == it->second->monObject()) continue;
           std::string type = it->second->monObject()->typeName();
	   std::string histogramname=it->second->dimInfo()->getName();	   
           std::vector<std::string> HistoFullName = Misc::splitString(it->second->dimInfo()->getName(), "/");    
           it->second->loadMonObject();
           it->second->monObject()->loadObject();
	   std::string histdir=fileName+":";
	   int previous=0;
	
	   for (int i=3;i<(int)HistoFullName.size()-1;i++) {
	      TDirectory *dir=0; 
	      histdir=histdir+"/"+HistoFullName[i];
	      if (i>3) {   
		 bool dirfound=false;
                 for (int j5=0;j5<(int)dirs.size();j5++){
		    if (!strcmp(dirs[j5]->GetPath(),histdir.c_str())){ 		          
		       dirs[j5]->GetDirectory(histdir.c_str());
		       dirs[j5]->cd();
		       dirfound=true;
		       previous=j5;
		    }
		  }
		  if (!dirfound) {
		     dir=dirs[previous]->mkdir(HistoFullName[i].c_str()); 
		     dir->cd();
		     dirs.push_back(dir);
		     previous=dirs.size()-1;
		  } 	
	       }   		    		 
	       else {
		  bool dirfound=false;
	          if (dirs.size()>0) {		     
                     for (int j6=0;j6<(int)dirs.size();j6++){
			if (!strcmp(dirs[j6]->GetPath(),histdir.c_str())){ 
		            dirs[j6]->GetDirectory(histdir.c_str());
			    dirs[j6]->cd();
			    dirfound=true;
			    previous=j6;
			}
		      }      
		   }    
		   if (!dirfound) {
		       dir=f->mkdir(HistoFullName[i].c_str());
		       dirs.push_back(dir);
		       previous=dirs.size()-1;
		       dir->cd();      
		   }   
	        }
             }   
	//	 msg << MSG::DEBUG << " saving histogram " << it->second->dimInfo()->getName() << endreq;
             it->second->monObject()->write();  
	     HistoFullName.clear();
          } 
          f->Close();
          delete f;f=0;
          if (endofrun) m_saved=true;
       }  
   }
   return endofrun;
}


bool BaseServiceMap::add() {
  MsgStream msg(msgSvc(), name());
  bool endofrun=false;
  bool runchanged=false;
  std::map<std::string, DimInfoMonObject*>::iterator it;
  if (0 == m_serverMap.size()){
    msg << MSG::DEBUG << " Adder can't add because the ServerMap is empty " << endreq;
    return false;
  }
  if (0 == m_serviceSet.size()) {
    msg << MSG::DEBUG << " Adder can't add because the ServiceSet is empty " << endreq;
    return false;
  }
  if (0 == m_dimInfo.size()){
    msg << MSG::DEBUG << " Adder can't add because the DimInfoMap is empty " << endreq;
    return false;
  }
  bool toteor=true;
  for (m_dimInfoIt=m_dimInfo.begin(); m_dimInfoIt!=m_dimInfo.end(); ++m_dimInfoIt) 
  {


    if (!m_dimSrvStatus[m_dimInfoIt->first].first){
      MonObject *monObjectTmp = m_dimInfo[m_dimInfoIt->first][m_dimSrvStatus[m_dimInfoIt->first].second]->monObject();
      if (monObjectTmp !=0) {
        m_dimSrv[m_dimInfoIt->first].second->copyFrom(monObjectTmp);
        m_dimSrvStatus[m_dimInfoIt->first].first = true;
      }
      else {
        msg << MSG::DEBUG << "Adder MonObject still unloaded for " << m_dimInfoIt->first << endreq;
        continue;
      }
    }
  
     m_dimSrv[m_dimInfoIt->first].second->reset();

    if (m_processMgr->publishRates()) {

       runchanged= m_monRateDecoder->RunChange();
     }
     int eors=0;
     int inactive=0;
     for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
        if (!m_processMgr->dimInfoServers()->isActive(it->first)) {
	  inactive++;
	  continue;
	}
        if(0 == it->second->monObject()) {
           bool isLoaded = it->second->loadMonObject();
           if (!isLoaded){
	      inactive++;
              continue;
           }  
	}  
               
        bool tmpendofrun=false;
        tmpendofrun= it->second->monObject()->endOfRun();  
         //if we find an endof run, only set the endofrun flag if we have them all
       if (tmpendofrun) eors++;
     }
     //we are at the end of the run, so set the recipient to end of run
     if (eors == (int)m_serverMap.size()) {
        m_dimSrv[m_dimInfoIt->first].second->setEndOfRun(true);
	endofrun=true;
     } 
     else {
       toteor=false;
       // we need to set it to false; it may be true from a previous end of run
       m_dimSrv[m_dimInfoIt->first].second->setEndOfRun(false);

     }      
       
    try {
       int cntr=0;
       for (it=m_dimInfoIt->second.begin(); it!=m_dimInfoIt->second.end(); ++it) {
          //loop over servers
	  if (runchanged) {
	  
	     it->second->reset();
	     }
          if (!m_processMgr->dimInfoServers()->isActive(it->first)) continue;

          if (0 == it->second->monObject()) continue;
          bool isLoaded = it->second->loadMonObject();
          if (!isLoaded){
             continue;
          }
	  cntr++;

          if (m_dimSrv[m_dimInfoIt->first].second->typeName().compare(s_monRate) == 0){
             int countersAdder = ((MonRate*) (m_dimSrv[m_dimInfoIt->first].second))->numCounters();
             int countersTerm = ((MonRate*) it->second->monObject())->numCounters();
             if ( countersAdder < countersTerm) {
                ((MonProfile*) (m_dimSrv[m_dimInfoIt->first].second))->synchronizeLabelNames(it->second->monObject());
                ((MonRate*) (m_dimSrv[m_dimInfoIt->first].second))->setNumCounters(((MonRate*)it->second->monObject())->numCounters());
             }
          }

          m_dimSrv[m_dimInfoIt->first].second->combine(it->second->monObject());
       }

       //update after adding
       m_dimSrv[m_dimInfoIt->first].first->updateService(m_dimSrv[m_dimInfoIt->first].second->endOfRun());
       if (m_dimSrv[m_dimInfoIt->first].second->typeName().compare(s_monRate) == 0){
          if (m_processMgr->publishRates()) m_monRateDecoder->update((MonRate*)(m_dimSrv[m_dimInfoIt->first].second));
       }  
    }
    catch (const std::exception &ex){
       msg << MSG::INFO << "std::exception: " << ex.what() << endreq;
    }  
    catch (...){
       msg << MSG::INFO << "unrecognized exception. "<< endreq;
    } 
  }


  return toteor;

}
