//====================================================================
//  InsertDB.cpp
//--------------------------------------------------------------------
//
//  Package    : InsertDB
//
//  Description: This program inserts histogram metadata in the Online
//               histogram database; it can also get and publish the list of
//               histograms in a task that need to be added 
//  Author     : M.Pepe-Altarelli, E.van Herwijnen
//  Created    : 08/02/2007
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
#include "InsertDB.h"
#include "OnlineHistDB/OnlineHistDB.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "dis.hxx"
#include "dic.hxx"
#include <time.h>


// for online monitoring
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(InsertDB)

// Constructor
//------------------------------------------------------------------------------
InsertDB::InsertDB(const std::string& name, ISvcLocator* ploc) :
Algorithm(name, ploc), m_nodename(), m_dimclientdns(), m_topleveltaskname(),
m_taskname()
  {
  declareProperty("nodename",m_nodename);
  declareProperty("dimclientdns",m_dimclientdns);
  declareProperty("topleveltaskname",m_topleveltaskname);
  declareProperty("taskname",m_taskname);
  }

//------------------------------------------------------------------------------
StatusCode InsertDB::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  IIncidentSvc* incsvc;
  sc = service("IncidentSvc",incsvc);
  int priority=100;
  if (sc.isSuccess()) incsvc->addListener(this,"INSERT_HISTOS",priority);
  if (sc.isSuccess()) incsvc->addListener(this,"GET_HISTOS",priority);
//  if (sc.isSuccess()) incsvc->addListener(this,"INSERT_HISTOS");
  MsgStream msg(msgSvc(), name());


  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode InsertDB::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  msg << MSG::DEBUG << "executing InsertDB....command " << command << endreq;

  DimClient::setDnsNode(m_dimclientdns.c_str());  
  DimBrowser dbr;
  char *service;
  char *format;
  int type;
  int icount=0;
  int icount2d=0;
  int icountp=0;
  
  std::string hSvcname;
  std::string hSvcname2d;
  std::string hSvcnames;
  std::string hSvcnames2d;
  std::string pSvcname;
  std::string pSvcnames;
  std::string commentSvcnames;
  std::string commentSvcname;
  std::string taskname;
  std::string algorithmname;
  std::string histogramname;
      
  if ((command=="INSERT_HISTOS")||(command=="GET_HISTOS")) {
    std::string password="histeggia194";
    OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","ora01:1528/HISTOGRAMDB");
    if (HistDB==0) {
       msg << MSG::INFO << "Could not connect to Histogram database." <<  endreq;
    }
    else {
//    std::vector<OnlineHistogram*> list;
 //   int x=HistDB->getAnalysisHistograms(list);
 //   if (x>0) {
 //     for (int i=0;i<=x-1;i++) {
////	 msg << MSG::INFO << "analysis histogram " << list[i]->name() << endreq; 
//      }
//    }  
       if (command=="INSERT_HISTOS") {
       hSvcnames="H1D/"+m_nodename+"_"+m_topleveltaskname+"*";
       hSvcnames2d="H2D/"+m_nodename+"_"+m_topleveltaskname+"*";
       pSvcnames="HPD/"+m_nodename+"_"+m_topleveltaskname+"*";
       commentSvcnames=m_nodename+"_"+m_topleveltaskname+"*/gauchocomment";
       msg << MSG::INFO << "Looking for service: " << hSvcnames.c_str() << endreq;     
   
       dbr.getServices(hSvcnames.c_str());
       icount=0;
       icount2d=0;
       icountp=0;
       while( (type = dbr.getNextService(service, format)) )
       { 
	   std::vector<int> temp;
	   hSvcname=service;
           msg << MSG::INFO << "Found service: " << hSvcname << endreq;     
           std::string::size_type loc=hSvcname.find("H1D/"+m_nodename+"_Adder_",0);
	   if (loc != std::string::npos ) {  
              msg << MSG::INFO << "Found service: " << hSvcname  <<endreq;     
	      icount++;
	      for (int i=1;i<=(int)hSvcname.length();i++) {
	         if (hSvcname.substr(i,1)=="/") {
		    temp.push_back(i);
		 }	      	     
	      }    
              taskname=hSvcname.substr(temp[1]+1,hSvcname.substr(0,temp[2]).length()-temp[1]-1);
	      algorithmname=hSvcname.substr(temp[2]+1,hSvcname.substr(0,temp[3]).length()-temp[2]-1);  
	      histogramname=hSvcname.substr(temp[3]+1,hSvcname.substr(0,temp[4]).length()-temp[3]-1);
	      msg << MSG::INFO << "taskname "<<taskname<<" algorithmname "  << algorithmname << " histogramname " << histogramname << endreq;     
	    //  HistDB->declareTask(taskname);
	      msg << MSG::INFO << "HistDB->declareHistogram("<<taskname<<","<<algorithmname<<","<<histogramname<<",1)"  << endreq; 
            //  HistDB->declareHistogram(taskname,algorithmname,histogramname,1);
	   //   msg << MSG::INFO <<
	   //   "HistDB->declareHistByServiceName("<<hSvcname<<")"  << endreq; 
	      HistDB->declareHistByServiceName(hSvcname);

	   }
	} 
	if (icount==0) {  
           dbr.getServices(hSvcnames2d.c_str());

           while( (type = dbr.getNextService(service, format)) )
           { 
	      std::vector<int> temp;
	      hSvcname2d=service;
              msg << MSG::INFO << "Found service: " << hSvcname2d << endreq;     
              std::string::size_type loc=hSvcname2d.find("H2D/"+m_nodename+"_Adder_",0);
	      if (loc != std::string::npos ) {  
                 msg << MSG::INFO << "Found service: " << hSvcname  <<endreq;     
	         icount2d++;
	         for (int i=1;i<=(int)hSvcname2d.length();i++) {
	            if (hSvcname2d.substr(i,1)=="/") {
		       temp.push_back(i);
		    }	      	     
	         }    
                 taskname=hSvcname2d.substr(temp[1]+1,hSvcname2d.substr(0,temp[2]).length()-temp[1]-1);
	         algorithmname=hSvcname2d.substr(temp[2]+1,hSvcname2d.substr(0,temp[3]).length()-temp[2]-1);  
	         histogramname=hSvcname2d.substr(temp[3]+1,hSvcname2d.substr(0,temp[4]).length()-temp[3]-1);
	//      msg << MSG::INFO << "HistDB->declareTask("<<taskname<<")"  << endreq;     
	//      HistDB->declareTask(taskname);
	         msg << MSG::INFO <<
	      "HistDB->declareHistogram("<<taskname<<","<<algorithmname<<","<<histogramname<<",2)"  << endreq; 
         //     HistDB->declareHistogram(taskname,algorithmname,histogramname,2);
	//      msg << MSG::INFO <<
	//      "HistDB->declareHistByServiceName("<<hSvcname2d<<")"  << endreq; 
	         HistDB->declareHistByServiceName(hSvcname2d);
	      }
	   } 
	   if (icount2d==0) {
              dbr.getServices(pSvcnames.c_str());
  
              while( (type = dbr.getNextService(service, format)) )
              { 
	         std::vector<int> temp;
	         pSvcname=service;
                 msg << MSG::INFO << "Found service: " << pSvcname << endreq;     
	   
                 std::string::size_type loc=pSvcname.find("HPD/"+m_nodename+"_Adder_",0);
	         if (loc != std::string::npos ) {  
                    msg << MSG::INFO << "Found service: " << pSvcname  <<endreq;     
	            icountp++;
	            for (int i=1;i<=(int)pSvcname.length();i++) {
	               if (pSvcname.substr(i,1)=="/") {
		          temp.push_back(i);
		       }	      	     
	            }    
                    taskname=pSvcname.substr(temp[1]+1,pSvcname.substr(0,temp[2]).length()-temp[1]-1);
	            algorithmname=pSvcname.substr(temp[2]+1,pSvcname.substr(0,temp[3]).length()-temp[2]-1);  
	            histogramname=pSvcname.substr(temp[3]+1,pSvcname.substr(0,temp[4]).length()-temp[3]-1);
	//      msg << MSG::INFO << "HistDB->declareTask("<<taskname<<")"  << endreq;     
	//      HistDB->declareTask(taskname);
	            msg << MSG::INFO <<
	            "HistDB->declareHistogram("<<taskname<<","<<algorithmname<<","<<histogramname<<",9)"  << endreq; 
         //     HistDB->declareHistogram(taskname,algorithmname,histogramname,9);
	//      msg << MSG::INFO <<
	//      "HistDB->declareHistByServiceName("<<pSvcname<<")"  << endreq; 
	            HistDB->declareHistByServiceName(pSvcname);

	         }
              }
	   }
        }
        msg << MSG::INFO <<"HistDB->commit()"  << endreq; 
        HistDB->commit();
     }
     if (command=="GET_HISTOS") {
       std::vector<OnlineHistogram*> list;
       //fix this later, doesn't work for v2r4
     /*  int x=HistDB->getHistogramsByTask(m_taskname, list);
       if (x>0) {
          for (int i=0;i<=x-1;i++) {
	     std::string histname;
	 //only works from onlinhistdb v2r4 onwards    
	 //    histname =list[i]->hname();
	 histname=list[i]->identifier();
       	     msg << MSG::INFO << "Registered histogram found" <<
	     histname.c_str() << endreq; 
          }
       }  */     
     }
     }
     delete HistDB;
     command="";
  }	 
  msg << MSG::DEBUG <<"Waiting for command to insert histogram data into DB."  << endreq; 
  dim_wait();
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void InsertDB::handle(const Incident& inc) {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Got incident " << inc.type() << " from " << inc.source() <<endmsg;
  command=inc.type();

}



//------------------------------------------------------------------------------
StatusCode InsertDB::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;

  return StatusCode::SUCCESS;
}
