//====================================================================
//  Saver.cpp
//--------------------------------------------------------------------
//
//  Package    : Saver
//
//  Description: This program saves histograms published by tasks using
//               the Gaucho MonitorSvc, specifically the Adder 
//
//  Author     : M.Pepe-Altarelli, E.van Herwijnen
//  Created    : 16/11/2006
//  Changes    : 
//
//====================================================================
// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include <time.h>

// for online monitoring
#include "GaudiKernel/IMonitorSvc.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RTL/rtl.h"
#include "dic.hxx"
#include "dis.hxx"

#include "Gaucho/MonObject.h"
#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/DimInfoMonObject.h"
#include "DimInfoServerStatus.h"
#include "Saver.h"
#include "Misc.h"
// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Saver)

// Constructor
//------------------------------------------------------------------------------
Saver::Saver(const std::string& name, ISvcLocator* ploc) : Algorithm(name, ploc)
{
  declareProperty("nodename",m_nodeName);
  declareProperty("objectname",m_objectName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("taskname",m_taskName);
  declareProperty("savedir",m_saveDir);

  m_serverStatus = new std::map<std::string, bool, std::less<std::string> >();

}

//------------------------------------------------------------------------------
StatusCode Saver::initialize() {
//------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());
  IIncidentSvc* incsvc;
  sc = service("IncidentSvc",incsvc);
  int priority=100;
  m_refreshTime=5;
  
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "*************Welcome to Saver v1r05******************* " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "This Saver will save data published in : " << m_dimClientDns << endreq;
  
  msg << MSG::DEBUG << "Consider node " << m_nodeName << endreq;
  
  msg << MSG::DEBUG << "TaskNames: " << endreq;
  for(unsigned int i=0; i < m_taskName.size();++i) {
    msg << MSG::DEBUG << "         taskName: " << m_taskName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }
  
  msg << MSG::DEBUG << "Properties: " << endreq;
  for(unsigned int i=0; i < m_objectName.size();++i) {
    msg << MSG::DEBUG << "         objectName: " << m_objectName[i] << endreq;
    msg << MSG::DEBUG << "         algorithmName: " << m_algorithmName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }
  
  if (sc.isSuccess()) incsvc->addListener(this,"SAVE_HISTOS",priority);

  sc = service("HistogramDataSvc", histoSvc, true );
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }
  
  msg << MSG::DEBUG << "creating Server/Service Status Objects" << endreq;
  m_dimInfoServerStatus = new DimInfoServerStatus(m_serverStatus, 
                                                  m_nodeName, 
                                                  m_taskName, 
                                                  msgSvc());
  
  m_fileName="Waiting for command to save histograms............."; 
  m_fileSize=0;
  declareInfo("file",m_fileName,"Filename of latest saveset");
  declareInfo("filesize",m_fileSize,"Filesize of latest saveset");

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Saver::execute() {
//------------------------------------------------------------------------------
  MsgStream  msg( msgSvc(), name() );
  //msg << MSG::INFO << "executing saver....command " << m_command << endreq;

  time_t rawTime;
  struct tm * timeInfo;
  
  std::string monSvcname;
  std::string monSvcnames;
  
  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char min[3];
  char sec[3];
  rawTime=time(NULL);
  timeInfo = localtime(&rawTime);
  strftime(year,sizeof(year),"%Y",timeInfo);
  strftime(month,sizeof(month),"%m",timeInfo);
  strftime(day,sizeof(day),"%d",timeInfo);
  strftime(hour,sizeof(hour),"%H",timeInfo);
  strftime(min,sizeof(min),"%M",timeInfo);
  strftime(sec,sizeof(sec),"%S",timeInfo);
  
  TFile *f=0;
  if (m_command=="SAVE_HISTOS") {
     msg << MSG::INFO << "SAVE_HISTOS" << endreq;
    for (unsigned int i = 0;i < m_taskName.size();i++) {
      m_fileSize=0;
      //set the filesize to 0 when starting to save
      //NB the ISO extended timestamp format is yearmonthdayThoursminssecs
      //can't use separators as ":" is no good for windows filenames
      //stick strictly to this standard so boost can parse it
      m_fileName=m_saveDir+"/"+m_taskName[i]+year+month+day+"T"+hour+min+sec+".root";
      msg << MSG::INFO << "Saver will save histograms in file " << m_fileName << endreq;
      msg << MSG::INFO << "Creating DimNames"<< endreq;
      CreateDimNames(m_nodeName, m_taskName[i], m_algorithmName, m_objectName);
      msg << MSG::INFO << "Creating MonObjects"<< endreq;
      CreateMonObjects(m_refreshTime);
      f = new TFile(m_fileName.c_str(),"create");
      msg << MSG::INFO << "Writing MonObjects"<< endreq;
      WriteMonObjects();
      m_fileSize=(int)f->GetSize();
      f->Close();
      delete f;
      f=0;
      msg << MSG::INFO << "Deleting MonObjects"<< endreq;
      DeleteMonObjects();
      m_command="";
    }
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void Saver::handle(const Incident& inc) {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Got incident " << inc.type() << " from " << inc.source() <<endmsg;
  m_command=inc.type();
}


//------------------------------------------------------------------------------
StatusCode Saver::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
//  delete f;
  return StatusCode::SUCCESS;
}

void Saver::CreateDimNames(std::string nodeName, std::string taskName, std::vector<std::string> algorithmName, std::vector<std::string> objectName){
   
  MsgStream mes1(msgSvc(), name());
  DimClient::setDnsNode(m_dimClientDns.c_str());
  DimBrowser m_dimBrowser;
  for (unsigned int j=0; j< objectName.size() ;j++) {
    //Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D
    std::string svcName = "*/" + nodeName+"*"+taskName +"*"+algorithmName[j]+"/"+objectName[j];

    //mes1 << MSG::INFO << "svcName=" + svcName  << endreq;
    
    m_dimBrowser.getServices(svcName.c_str());

    char *service;
    char *format;
    int type;
    int count=0;
    std::vector<std::string> dimInfoNamesVect1;

    while (count==0)
    {

      while (type = m_dimBrowser.getNextService(service, format))
      {
        std::string  serviceStr = service;//Ex--> MON/node00101_Test_1/GaudiExample/ExMonH1D
        mes1 << MSG::INFO << "Service found: " + serviceStr << endreq;
        dimInfoNamesVect1.push_back(serviceStr);
        count++;
      } 
      if (count>0) break;
      else lib_rtl_sleep(500);
    }

    /************************************************/
    m_dimInfoNamesVect2.push_back(dimInfoNamesVect1);
    /************************************************/
  }
  // print for test
/*    mes1 << MSG::DEBUG << "PRINTING DIM NAMES : " << endreq;

    for (unsigned int k = 0; k < m_dimInfoNamesVect2.size(); k++) {
      for (unsigned int j = 0; j < m_dimInfoNamesVect2[k].size(); j++) {
        mes1 << MSG::DEBUG << "	Info : " << m_dimInfoNamesVect2[k][j] << endreq;
      }
    }*/
}

void Saver::CreateMonObjects(int refreshTime) {
  MsgStream msg(msgSvc(), name());
  for (unsigned int k = 0; k < m_dimInfoNamesVect2.size(); k++) {
    std::vector<MonObject*> tmpMonObjectVect1;
    for (unsigned int i = 0; i < m_dimInfoNamesVect2[k].size(); i++) {
      std::string serverName = getServerName(m_dimInfoNamesVect2[k][i]);
      if (!m_dimInfoServerStatus->isServerActive(serverName)) {
        msg << MSG::WARNING <<"Server "<< serverName << " desactived. MonObjects in this server will not be Saved." << endreq;
        continue;
      }
      DimInfoMonObject* tmpDimInfoMonObject = new DimInfoMonObject(m_dimInfoNamesVect2[k][i], refreshTime, m_source);
      tmpDimInfoMonObject->setMsgSvc(msgSvc());
      tmpDimInfoMonObject->createMonObject();
      MonObject *tmpMonObject = 0;
      int ntries=0;
      while (1)
      {
        if (tmpDimInfoMonObject->monObject() != 0){
          //tmpMonObject = tmpDimInfoMonObject->monObject();
          tmpMonObject = MonObjectCreator::createMonObject(tmpDimInfoMonObject->monObject()->typeName(), msgSvc(), name());
	  tmpMonObject->copyFrom(tmpDimInfoMonObject->monObject());
          break;
        }
        ntries++;

        msg << MSG::WARNING <<"No MonObject found after " << ntries << " attempts." << endreq;

        if(ntries==10) break;
        lib_rtl_sleep(500); 
      }
      tmpMonObjectVect1.push_back(tmpMonObject);
      if (tmpDimInfoMonObject) {delete tmpDimInfoMonObject;tmpDimInfoMonObject=0;}
    }
    m_monObjectVect2.push_back(tmpMonObjectVect1);
  }
}

void Saver::WriteMonObjects(){
  for (unsigned int k = 0; k < m_monObjectVect2.size(); k++) {
    for (unsigned int i = 0; i < m_monObjectVect2[k].size(); i++) {
      if (m_monObjectVect2[k][i] != 0)
        m_monObjectVect2[k][i]->write();
    }
  }
}

void Saver::DeleteMonObjects(){
  for (unsigned int k = 0; k < m_monObjectVect2.size(); k++) {
    for (unsigned int i = 0; i < m_monObjectVect2[k].size(); i++) {
      if (m_monObjectVect2[k][i] != 0)
        delete m_monObjectVect2[k][i];
        m_monObjectVect2[k][i] = 0;
    }
  }
  for (unsigned int k = 0; k < m_monObjectVect2.size(); k++) {
    m_monObjectVect2[k].clear();
  }
  m_monObjectVect2.clear();
  for (unsigned int k = 0; k < m_dimInfoNamesVect2.size(); k++) {
    m_dimInfoNamesVect2[k].clear();
  }
  m_dimInfoNamesVect2.clear();
}

std::string Saver::getServerName(const std::string &serviceName){
  std::vector<std::string> serviceParts = Misc::splitString(serviceName, "/");
  return serviceParts[1];
}


