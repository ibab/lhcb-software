// $Id: DimCmdServer.cpp,v 1.19 2010-02-17 13:44:46 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "DimCmdServer.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
//#include "OnlineHistDB/OnlineHistDB.h"

  //HDS =HistogramDataService but used as generic name for anything on store
  static IHistogramSvc* m_histogramSvc; 
  //static IService* m_HDS = 0;
  static char *m_nextcommand;
  std::string cmdstr = "C";

DimCmdServer::DimCmdServer(std::string name, ISvcLocator* svclocator, IGauchoMonitorSvc* publishsvc) : 
  DimCommand((char*)name.c_str(),(char*)cmdstr.c_str()), m_incidentSvc(0) {
  StatusCode sc;

  m_histogramSvc = 0; 
  m_nextcommand = 0;

  m_publishSvc = publishsvc;
  // get pointer to ServiceLocator
  m_svcloc = svclocator;

  // get msg logging
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimCmdServer");  

  log << MSG::DEBUG << "Creating DimCmdServer for thisnodename " << name 
      << endreq;

  // to walk the transient store  
  sc = svclocator->service("HistogramDataSvc",m_histogramSvc,true); 
  if (sc.isSuccess()){
    log << MSG::DEBUG << "Found the HistogramDataService" << endreq;
  }
  else {
    log << MSG::WARNING << "Unable to locate the HistogramDataService" 
        << endreq;
  }

  //to traverse the transient store
/*  sc = svclocator->service("MonitorSvc", m_publishSvc, true );
  if( sc.isSuccess() )   {
    log << MSG::DEBUG << "Found the IPublish interface" << endreq;
  }
  else {
    log << MSG::WARNING << "Unable to locate the IPublish interface." 
        << endreq;
  }*/
  sc = svclocator->service("IncidentSvc", m_incidentSvc, true );
  if( sc.isSuccess() )   {
    log << MSG::DEBUG << "Found the Incident interface" << endreq;
  }
  else {
    log << MSG::WARNING << "Unable to locate the Incident interface." 
        << endreq;
  }

}

DimCmdServer::~DimCmdServer() {
  StatusCode sc;
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimCmdServer");  
  delete [] m_nextcommand;
  if (m_histogramSvc) m_histogramSvc->release();

}

void DimCmdServer::commandHandler() {
  StatusCode sc;
  MsgStream log(m_msgsvc, "DimCmdServer");
  //! hardcoded string length limits

  m_nextcommand=getString();

  if ( m_incidentSvc ) {
     Incident incident("DimCmdServer","SAVE_HISTOS");
     Incident incident2("DimCmdServer","INSERT_HISTOS");
     Incident incident3("DimCmdServer","RECONFIGURE");
     if (strncmp(m_nextcommand,"save_histos",11)==0) m_incidentSvc->fireIncident(incident);
     if (strncmp(m_nextcommand,"insert_histos",13)==0) m_incidentSvc->fireIncident(incident2);
     if (strncmp(m_nextcommand,"reconfigure",11)==0) m_incidentSvc->fireIncident(incident3);
  }
  log << MSG::DEBUG << "received command " << m_nextcommand << endreq;
  if (strncmp(m_nextcommand,"/stat/",6)==0) {
    std::string  m_rootName;
    SmartDataPtr<DataObject> root(m_histogramSvc,m_rootName);
    std::string store_name = "Unknown";
    IRegistry* pReg = root->registry();
    IRegistry* pObj;
    if ( pReg )  {
      SmartIF<IService> isvc(pReg->dataSvc());
      if ( isvc )  {
        store_name = isvc->name();
      }
    }
    pObj=root->registry();
    SmartIF<IDataManagerSvc> mgr(m_histogramSvc);
    if ( mgr )    {
      myhisto=0;
	 
      sc=m_histogramSvc->retrieveObject(m_nextcommand,mydataobject);
      myhisto=dynamic_cast<AIDA::IHistogram*>(mydataobject);
      if ( sc.isSuccess() ) {
        log << MSG::DEBUG << "Histogram retrieved id: " << m_nextcommand << endreq;
        // now subscribe to it
        log << MSG::DEBUG << "Subscribing to service "<< m_nextcommand << endreq;
        
        ((IMonitorSvc*) m_publishSvc)->declareInfo(m_nextcommand,myhisto,myhisto->title(),m_histogramSvc);
      }
      else {
        log << MSG::DEBUG << "Problem retrieving histogram "
            << m_nextcommand  <<endreq;  
      }
    }
    else{
      log << MSG::DEBUG << "Can't get to the histogram service. "  << endreq;
    }
  }

  log << MSG::DEBUG << "Leaving commandHandler. "  << endreq;

  //  delete [] m_nextcommand;

} // end commandHandler
