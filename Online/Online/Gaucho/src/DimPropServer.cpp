//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimPropServer.cpp,v 1.1.1.1 2003-06-05 09:34:46 vanphil Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "DimPropServer.h"

DimPropServer::DimPropServer(char* thisnodename, ISvcLocator* svclocator) : 
    DimRpc(thisnodename,"C", "C") {
//! hardcoded char limit
  StatusCode sc;
  m_name = new char[100];
  strcpy(m_name,thisnodename);
// get pointer to ServiceLocator
  m_svcloc = svclocator;
// get msg logging
  sc = m_svcloc->service("MessageSvc", m_msgsvc);
  MsgStream log(m_msgsvc, "DimPropServer");
// get pointer to ApplicationMgr
  IProperty* appmgr;
  sc = svclocator->service("ApplicationMgr", appmgr);
  if( sc.isSuccess() ) {
	m_appmgr=appmgr;
    log << MSG::INFO << "Found ApplicationMgr"<< endreq;
  }
  else
	log << MSG::ERROR << "Unable to locate the ApplicationMgr" << endreq;
// get pointer to AlgorithmFactory
  SmartIF<IAlgManager> algman ( IID_IAlgManager, appmgr );
  if( algman.isValid() ) {
	m_algman=algman;
	log << MSG::INFO << "Found the AlgoritmFactory" << endreq;
  }
  else {
	log << MSG::ERROR << "Unable to locate the AlgorithmFactory" << endreq;
  }
}


DimPropServer::~DimPropServer() {
  delete [] m_name;
}


void DimPropServer::rpcHandler() {
  StatusCode sc;
  MsgStream log(m_msgsvc, "DimPropServer");
//! hardcoded string length limits
  char* nextRPCcommand=new char[100];
  char* nextcommand=new char[100];
  char* nextalg=new char[100];
  char* nextprop=new char[100];
  char* nextval=new char[100];
  char* nextdata=new char[1000];
  nextRPCcommand=getString();
  IProperty* iprop=0;
  log << MSG::INFO << "received command " << nextRPCcommand << endreq;
  nextval=strchr(nextRPCcommand,'=');
  if (nextval != NULL ) {
 	int clen=strlen(nextRPCcommand);
	int vlen=strlen(nextval);
	nextval=nextval+1;
    strncpy(nextcommand,nextRPCcommand,clen-vlen);
	nextcommand[clen-vlen]='\0';
    log << MSG::INFO << "algorithm.property to set = " << nextcommand << endreq;
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
//    nextalg=new char[clen-plen];
    strncpy(nextalg,nextcommand,clen-plen);
	nextalg[clen-plen]='\0';
    log << MSG::INFO << "algorithm to call = " << nextalg << endreq;
    log << MSG::INFO << "property  to call = " << nextprop << endreq;
//	int alen=strlen(nextalg);
//    log << MSG::INFO << "DimPropServer: INFO: string lengths = " << clen <<","<<plen<<","<<alen << endreq;
  }
  else {
    strcpy(nextalg,nextcommand);
  }
  if (strcmp(nextalg,"ApplicationMgr")==0)
    iprop=m_appmgr;
  else{  
    IAlgorithm* ialg;
    sc = m_algman->getAlgorithm(nextalg,ialg);
    if (sc.isSuccess()){
      SmartIF<IProperty>  algprop(ialg);
	  iprop=algprop;
	}
    else
      log << MSG::ERROR << "failed to get the Algorithm" << endreq;
  }
  if (iprop != 0) {
    if (nextprop != NULL) {
	  if (nextval != NULL) {
// set specific property
        sc = iprop->setProperty(nextprop, nextval);
		if (sc.isSuccess()) {
		  strcpy(nextdata,"ok");
		  setData(nextdata);
          log << MSG::INFO << "set property data to value: " << nextval << endreq;
		}
		else {
		  strcpy(nextdata,"error");
		  setData(nextdata);
          log << MSG::ERROR << "failed to set property to new value:" << nextval << endreq;
		}
      }
      else {
// request specific property
        std::string value("test");
	    int len;
        iprop->getProperty(nextprop, value);
        if (value != "test") {
// print out all chars to check
//        const char* cvalue="test"; // transmit was fine!
//        value="test";
//        const char* cvalue=value.c_str(); // transmit was fine!
		  const char* cvalue=value.c_str();
//		  int i,code;
//		  for (i=0;i<strlen(cvalue);i++){
//		    code=cvalue[i];
//            log << MSG::INFO<< cvalue[i] <<"="<< code <<endreq;
//		  }
//! ad hoc hack to suppress last char from property
          len=strlen(cvalue);
		  if (len>0) len=len-1;
          strncpy(nextdata,cvalue,len);
	      setData(nextdata);
          log << MSG::INFO << "sent property data " << nextdata << endreq;
		}
        else {
          log << MSG::ERROR << "failed to get property for "<< nextprop << endreq;
		}
	  }
	}
    else {
	  if (nextval != NULL) {
// set all properties, not possible
        log << MSG::ERROR << "wrong command, cannot set all properties"<< endreq;
	  }
	  else {
// request all properties
	    std::string value("");
	    strcpy(nextdata,"");
	    const std::vector<Property*> allprops = iprop->getProperties();
	    for( std::vector<Property*>::const_iterator iter = allprops.begin();
     	                   iter != allprops.end(); iter++ ) {
		  std::string propvalue="";
		  std::string propname=(*iter)->name();
          iprop->getProperty(propname, propvalue);
          if (propvalue != "") {
		    strcat(nextdata,propname.c_str());
		    strcat(nextdata,"=");
//! ad hoc hack to ignore last char from property
		    strncat(nextdata,propvalue.c_str(),strlen(propvalue.c_str())-1);
		    strcat(nextdata,";");
		  }
          else {
            log << MSG::ERROR << "obtained empty value for property "<< propname.c_str() << endreq;
		  }
		}
         if (strcmp(nextdata,"")!=0) {
//		  const char* cvalue=value.c_str();
//          log << MSG::INFO << "DimPropServer: INFO: sent property data " << cvalue << endreq;
//		  int i,code;
//		  for (i=0;i<strlen(cvalue);i++){
//		    code=cvalue[i];
//            log << MSG::INFO<< cvalue[i] <<"="<< code <<endreq;
//		  }
//          strncpy(nextdata,cvalue,strlen(cvalue)-1);
	      setData(nextdata);
          log << MSG::INFO << "sent property data " << nextdata << endreq;
		 }
        else
          log << MSG::ERROR << "failed to get properties for "<< nextalg << endreq;
	  } //endif request all props
	}
	} // endif iprop!==0
  else {
    log << MSG::ERROR << "received unknown command" << endreq;
    setData("");
  } // endif to make sense of nextcommand
  delete [] nextdata;
  delete [] nextval;
  delete [] nextprop;
  delete [] nextalg;
  delete [] nextcommand;
  delete [] nextRPCcommand;
} // end rpcHandler

	
