// $Id: DimPropServer.cpp,v 1.11 2008-08-04 07:12:38 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "DimPropServer.h"
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
#include "GaudiKernel/IHistogramSvc.h"
//#include "AIDA/IHistogram1D.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IMonitorSvc.h"
#include "AIDA/IAxis.h"

#ifdef WIN32
namespace wins {
#include <winsock.h>
}
#else
#include <unistd.h>
#endif


//EDS =EventDataService but used as generic name for anything on store
IHistogramSvc* EDS=0; 
IService* m_EDS;
IGauchoMonitorSvc* m_publishsvc;


DimPropServer::DimPropServer(std::string name, ISvcLocator* svclocator) : 
  DimRpc(name.c_str(),"C", "C") {
  StatusCode sc;
  m_publishsvc = 0;
  // get pointer to ServiceLocator
  m_svcloc = svclocator;    
  
  // get msg logging
  sc = svclocator->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimPropServer");  
  if ( sc.isFailure() ) { 
    log << MSG::FATAL << "Error locating MessageSvc." << endreq;
  }
  else {
    log << MSG::DEBUG << "MessageSvc located." << endreq;
  }


  // to walk the transient store  


  sc = svclocator->service("HistogramDataSvc",EDS,true); 
  if (sc.isSuccess()){    
    log << MSG::INFO << "Found the HistogramDataService" << endreq;
  }
  else {    
    log << MSG::FATAL << "Unable to locate the HistogramDataService" << endreq;
  }   
  
  //to traverse the transient store  
  sc = svclocator->service("MonitorSvc", m_publishsvc, true );
  if( sc.isSuccess() )   {
    log << MSG::INFO << "Found the IPublish interface" << endreq;
  }
  else {    
    log << MSG::FATAL << "Unable to locate the IPublish interface." << endreq;
  }  

  // get pointer to ApplicationMgr
  IProperty* appmgr;
  sc = svclocator->service("ApplicationMgr", appmgr);

  if( sc.isSuccess() ) {
    m_appmgr=appmgr;
    log << MSG::INFO << "Found ApplicationMgr"<< endreq;
  }
  else
    log << MSG::FATAL << "Unable to locate the ApplicationMgr" << endreq;

  // get pointer to AlgorithmFactory
  SmartIF<IAlgManager> algman ( IID_IAlgManager, appmgr );
  if( algman.isValid() ) {
    m_algman=algman;
    log << MSG::INFO << "Found the AlgoritmFactory" << endreq;
  }
  else {
    log << MSG::FATAL << "Unable to locate the AlgorithmFactory" << endreq;
  }

  
}


DimPropServer::~DimPropServer() {
  if (EDS) EDS->release();
}


void DimPropServer::rpcHandler() {
  StatusCode sc;
  MsgStream log(m_msgsvc, "DimPropServer");
  //! hardcoded string length limits
  char *nextRPCcommand;
  char* nextcommand=new char[100];
  char* nextalg=new char[100];
  char *nextprop;
  char *nextval;
  char* nextdata=new char[2000];
   
  char *ptr;    

  
  nextRPCcommand=getString();
  ptr=nextdata;

  IHistogramSvc* histoSvc();

  log << MSG::INFO << "received command " << nextRPCcommand << endreq;
  if (strcmp(nextRPCcommand,"list")==0) {
    int size=0;   
   
    /*get the list of algorithms & send them to pvss*/
    std::list<IAlgorithm*> aList = m_algman->getAlgorithms();
    std::list<IAlgorithm*>::iterator  nextAlg = aList.begin();
    for (;nextAlg != aList.end(); nextAlg++)  {    
         
      IAlgorithm* theIAlg;
      Algorithm* alg;
      std::string theName =(*nextAlg)->name();
      StatusCode result = m_algman->getAlgorithm( theName, theIAlg );
      if ( result.isSuccess( ) ) {
        log << MSG::INFO << " Next algorithm name: "<<theName<< endreq;
        //	      try (
        alg = dynamic_cast<Algorithm*>(theIAlg);
        //	      }
        //	      catch {
        //	         StatusCode result = StatusCode::FAILURE;	    
        //	      }
        //	      if ( result.isSuccess() ) {
        std::vector<Algorithm*>* theSubAlgs =alg->subAlgorithms();
        std::vector<Algorithm*>::iterator it;
        std::vector<Algorithm*>::iterator itend;
        itend = theSubAlgs->end();
        for ( it=theSubAlgs->begin();it!=itend;it++) {
	        // Algorithm* theSubAlgorithm = (*it);
          log << MSG::INFO << " Next sub algorithm: "<<(*it)->name()<< endreq;
        }
        //	      }	 
      }   
      strcpy(ptr,((*nextAlg)->name()).c_str());
      char tmp[100];
      strcpy(tmp,((*nextAlg)->name()).c_str());
      ptr+=strlen(tmp)+1;
      size+=strlen(tmp)+1; 
    } 

    log << MSG::INFO << " Size of list: "<<size<< " Setting data." << endreq;
    setData(nextdata,size);

  }
  else {   
    if (strcmp(nextRPCcommand,"wait")==0)   {
      log << MSG::INFO << " Wait found. Setting data to null. "<< endreq;   
      setData("");
    }
    else {        
      if ((strcmp(nextRPCcommand,"listhistos")==0)|| (strncmp(nextRPCcommand,"resethistos",11)==0)){
        int size=0;
        std::string  m_rootName;
        SmartDataPtr<DataObject> root(EDS,m_rootName);
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
	     	    	    
        SmartIF<IDataManagerSvc> mgr(EDS);
        if ( mgr )    {
          typedef std::vector<IRegistry*> Leaves;
          Leaves leaves;
          StatusCode sc = mgr->objectLeaves(pObj, leaves);
          if ( sc.isSuccess() )  {
            log << MSG::INFO << "Found leaves. "  << endreq;
            for ( Leaves::const_iterator i=leaves.begin(); 
                  i != leaves.end(); i++ )   {
              const std::string& id = (*i)->identifier();
              log << MSG::INFO << "Histogram found with id " << id 
                  << " char 6 "<< id.substr(6,1)	       <<endreq;
              char ch=id[6];
              if (isdigit(ch))
              {	     	       		   
                strcpy(ptr,id.c_str());
                char tmp[100];
                strcpy(tmp,id.c_str());
                ptr+=strlen(tmp)+1;
                size+=strlen(tmp)+1; 
	       
                //add histogram title	
                myhisto=0;
                sc=EDS->retrieveObject(id,mydataobject);
                if (sc.isSuccess()) {
                  myhisto=dynamic_cast<AIDA::IHistogram*>(mydataobject);
		  if ((strcmp(nextRPCcommand,"resethistos")==0)||(strstr(nextRPCcommand,myhisto->title().c_str())!=0)) {
                     log << MSG::INFO << " Resetting histogram : "
                      << myhisto->title()<<endreq;  
		      myhisto->reset();
		  }    
		  else {
		      log << MSG::INFO << " Histogram found with title : "
                      << myhisto->title()<<endreq;  
		  }       
                  strcpy(ptr,myhisto->title().c_str());
                  char tmp1[100];
                  strcpy(tmp1,myhisto->title().c_str());
                  ptr+=strlen(tmp1)+1;
                  size+=strlen(tmp1)+1;
                }
                else
                {
                  log << MSG::INFO << "Could not retrieve object from TES " 
                      << endreq;
                }
              }	   		   
            }
            setData(nextdata,size);
          }
          else{
            log << MSG::INFO << "No Histograms found on the transient store."
                << endreq;
          }               
        }	
        else {
          log << MSG::INFO << "Unable to go to the transient store. "
              << endreq;
        }
      }
      else {			     
        nextval=strchr(nextRPCcommand,'=');
        if (nextval != NULL ) {
          int clen=strlen(nextRPCcommand);
          int vlen=strlen(nextval);
          nextval=nextval+1;
          strncpy(nextcommand,nextRPCcommand,clen-vlen);
          nextcommand[clen-vlen]='\0';
          log << MSG::INFO << "algorithm.property to set = " 
              << nextcommand << endreq;
          log << MSG::INFO << "value to set = " << nextval << endreq;
        }
        else {
          strcpy(nextcommand,nextRPCcommand);
        }
        nextprop=strchr(nextcommand,'.');
        if (nextprop != NULL) {
          int clen=strlen(nextcommand);
          int plen=strlen(nextprop);
          nextprop=nextprop+1;
          strncpy(nextalg,nextcommand,clen-plen);
          nextalg[clen-plen]='\0';
          log << MSG::INFO << "algorithm to call = " << nextalg << endreq;
          log << MSG::INFO << "property  to call = " << nextprop << endreq;
        }
        else {
          strcpy(nextalg,nextcommand);
        }
        IProperty* iprop=0;
        IAlgorithm* ialg;
        if (strcmp(nextalg,"ApplicationMgr")==0)
          iprop=m_appmgr;
        else {  

          sc = m_algman->getAlgorithm(nextalg,ialg);
          if (sc.isSuccess()){
            SmartIF<IProperty>  algprop(ialg);
            iprop=algprop;
            log << MSG::INFO << "successfully set iprop for nextalg "  
                << nextalg << endreq;
          }
          else
            log << MSG::WARNING << "failed to get the Algorithm" << endreq;
        }
    
        if (iprop != 0) {
          if (nextprop != NULL) {
            if (nextval != NULL) {
              //               set specific property
              sc = iprop->setProperty(nextprop, nextval);
              ialg->initialize();
              if (sc.isSuccess()) {
                log << MSG::INFO << "set property data to value: " 
                    << nextval << endreq;
              }
              else {
                log << MSG::WARNING << "failed to set property to new value:"
                    << nextval << endreq;
              }
            }
            else {
              //               request specific property
              std::string value("test");
              int len;
              iprop->getProperty(nextprop, value);
              if (value != "test") {
                const char* cvalue=value.c_str();
                //! ad hoc hack to suppress last char from property
                len=strlen(cvalue);
                if (len>0) len=len-1;
                strncpy(nextdata,cvalue,len);
                setData(nextdata);
                log << MSG::INFO << "sent property data " 
                    << nextdata << endreq;
              }
              else {
                log << MSG::WARNING << "failed to get property for "
                    << nextprop << endreq;
              }
            }
          }
          else {
            if (nextval != NULL) {
              // set all properties, not possible
              log << MSG::WARNING << "wrong command, cannot set all properties"
                  << endreq;
            }
            else {
              //               request all properties
              char *propptr;
              int propsize=0;
              char proptmp[2000];
              char proptmpdata[2000];
	       
              log << MSG::INFO << "Listing all properties."<< endreq;

              propptr=nextdata;
              const std::vector<Property*> allprops = iprop->getProperties();
              for(std::vector<Property*>::const_iterator iter=allprops.begin();
                  iter != allprops.end(); iter++ ) {
                std::string propvalue="";
                std::string propname=(*iter)->name();		 
                iprop->getProperty(propname, propvalue);
                log << MSG::INFO << "next property name " 
                    << propname << "=" << propvalue << endreq;
                if (propvalue != "") {
                  strcpy(proptmpdata,propname.c_str());
                  strcat(proptmpdata,"=");             
                  strcat(proptmpdata,propvalue.c_str());
                  //      strcat(proptmpdata,";");
                  strcpy(propptr,proptmpdata);
                  strcpy(proptmp,proptmpdata);  
                  propptr+=strlen(proptmp)+1;
                  propsize+=strlen(proptmp)+1;
		     
                }
                else {
                  log << MSG::INFO << "obtained empty value for property "
                      << propname.c_str() << endreq;
                }
              }
              if (strcmp(nextdata,"")!=0) {
                setData(nextdata,propsize);
                log << MSG::INFO << "sent property data. length" 
                    << propsize	 << endreq;
              }
              else {
                log << MSG::WARNING << "failed to get properties for "
                    << nextalg << endreq;
              }
	  
            } //endif request all props	
            iprop=0;	
          } // endif iprop!==0
        } 
        else {
          log << MSG::INFO << "Setting data to null." << endreq;
          setData("");
        } // endif to make sense of nextcommand    
      }
    } 
  }
  log << MSG::INFO << "Leaving rpchandler. Deleting pointers. "  << endreq;

  delete [] nextdata;
  delete [] nextcommand;
  delete [] nextalg;

} // end rpcHandler

	
