//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimCmdServer.cpp,v 1.1 2005-03-22 16:39:17 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "Gaucho/DimCmdServer.h"
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





//HDS =HistogramDataService but used as generic name for anything on store
IHistogramSvc* HDS=0; 
IService* m_HDS;
char *nextcommand;


DimCmdServer::DimCmdServer(char* thisnodename, ISvcLocator* svclocator) : 
    DimCommand(thisnodename,"C") {
//! hardcoded char limit
  StatusCode sc;
  m_name = new char[100];
  m_nodename=new char[50];
  strcpy(m_name,thisnodename);
  
  m_publishsvc = 0;
// get pointer to ServiceLocator
  m_svcloc = svclocator;    
  
// get msg logging
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimCmdServer");  
 

// to walk the transient store  


  sc = svclocator->service("HistogramDataSvc",HDS,true); 
  if (sc.isSuccess()){    
    log << MSG::INFO << "Found the HistogramDataService" << endreq;
  }
  else {    
    log << MSG::ERROR << "Unable to locate the HistogramDataService" << endreq;
  }   
  
 //to traverse the transient store  
  sc = svclocator->service("MonitorSvc", m_publishsvc, true );
  if( sc.isSuccess() )   {
    log << MSG::INFO << "Found the IPublish interface" << endreq;
  }
    else {    
    log << MSG::ERROR << "Unable to locate the IPublish interface." << endreq;
  }  
  
}


DimCmdServer::~DimCmdServer() {
  StatusCode sc;
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimCmdServer");  
  delete [] m_name;
  delete [] nextcommand;
  delete [] m_nodename;
  if (HDS) HDS->release();

}


void DimCmdServer::commandHandler() {
  StatusCode sc;
  MsgStream log(m_msgsvc, "DimCmdServer");
//! hardcoded string length limits

  nextcommand=getString();  

  gethostname(m_nodename,50);
  //get hostname only (slc)
  char * pch;
  pch = strtok(m_nodename,".");


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
	 sc=HDS->retrieveObject(nextcommand,myhisto);
	 if ( sc.isSuccess() ) {
            log << MSG::INFO << "Histogram retrieved id: "  <<nextcommand << endreq;
            // now subscribe to it	                  
	    log << MSG::INFO << "Subscribing to service "<<nextcommand<< endreq;	       
            m_publishsvc->declareInfo(nextcommand,myhisto,myhisto->title(),HDS);    		   
         }
         else {
	   log << MSG::INFO << "Problem retrieving histogram "  << nextcommand  <<endreq;  
         }			   		   		   
     }
     else{
	        log << MSG::INFO << "Can't get to the histogram service. "  << endreq;
     }               

  }

  log << MSG::INFO << "Leaving commandHandler. "  << endreq;

//  delete [] nextcommand;

//  delete [] m_nodename;

} // end commandHandler

	
