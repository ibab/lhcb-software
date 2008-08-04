// $Id: DimCmdServer.cpp,v 1.11 2008-08-04 07:12:38 evh Exp $

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
#include "OnlineHistDB/OnlineHistDB.h"

//HDS =HistogramDataService but used as generic name for anything on store
static IHistogramSvc* HDS = 0; 
//static IService* m_HDS = 0;
static char *nextcommand = 0;

DimCmdServer::DimCmdServer(std::string name, ISvcLocator* svclocator) : 
  DimCommand(name.c_str(),"C"), m_incidentSvc(0) {
  StatusCode sc;

  m_publishsvc = 0;
  // get pointer to ServiceLocator
  m_svcloc = svclocator;

  // get msg logging
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimCmdServer");  

  log << MSG::INFO << "Creating DimCmdServer for thisnodename " << name 
      << endreq;

  // to walk the transient store  
  sc = svclocator->service("HistogramDataSvc",HDS,true); 
  if (sc.isSuccess()){
    log << MSG::INFO << "Found the HistogramDataService" << endreq;
  }
  else {
    log << MSG::WARNING << "Unable to locate the HistogramDataService" 
        << endreq;
  }

  //to traverse the transient store
  sc = svclocator->service("MonitorSvc", m_publishsvc, true );
  if( sc.isSuccess() )   {
    log << MSG::INFO << "Found the IPublish interface" << endreq;
  }
  else {
    log << MSG::WARNING << "Unable to locate the IPublish interface." 
        << endreq;
  }
  sc = svclocator->service("IncidentSvc", m_incidentSvc, true );
  if( sc.isSuccess() )   {
    log << MSG::INFO << "Found the Incident interface" << endreq;
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
  delete [] nextcommand;
  if (HDS) HDS->release();

}

void DimCmdServer::commandHandler() {
  StatusCode sc;
  MsgStream log(m_msgsvc, "DimCmdServer");
  //! hardcoded string length limits

  nextcommand=getString();

  if ( m_incidentSvc ) {
     Incident incident("DimCmdServer","SAVE_HISTOS");
     Incident incident2("DimCmdServer","INSERT_HISTOS");
     if (strncmp(nextcommand,"save_histos",11)==0) m_incidentSvc->fireIncident(incident);
     if (strncmp(nextcommand,"insert_histos",13)==0){
        m_incidentSvc->fireIncident(incident2);
     }
  }
  log << MSG::INFO << "received command " << nextcommand << endreq;
  if (strncmp(nextcommand,"/stat/",6)==0) {
    std::string  m_rootName;
    SmartDataPtr<DataObject> root(HDS,m_rootName);
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
    SmartIF<IDataManagerSvc> mgr(HDS);
    if ( mgr )    {
      myhisto=0;
	 
      sc=HDS->retrieveObject(nextcommand,mydataobject);
      myhisto=dynamic_cast<AIDA::IHistogram*>(mydataobject);
      if ( sc.isSuccess() ) {
        log << MSG::INFO << "Histogram retrieved id: " << nextcommand << endreq;
        // now subscribe to it
        log << MSG::INFO << "Subscribing to service "<< nextcommand << endreq;
        
        ((IMonitorSvc*) m_publishsvc)->declareInfo(nextcommand,myhisto,myhisto->title(),HDS);
      }
      else {
        log << MSG::INFO << "Problem retrieving histogram "
            << nextcommand  <<endreq;  
      }
    }
    else{
      log << MSG::INFO << "Can't get to the histogram service. "  << endreq;
    }
  }

  log << MSG::INFO << "Leaving commandHandler. "  << endreq;

  //  delete [] nextcommand;

} // end commandHandler

	
