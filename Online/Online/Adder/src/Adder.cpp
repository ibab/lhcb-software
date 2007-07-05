//====================================================================
//  Adder.cpp
//--------------------------------------------------------------------
//
//  Package    : Adder
//
//  Description: This program adds histograms published by tasks using
//               the Gaucho MonitorSvc and publishes them 
//
//  Author     : M.Pepe-Altarelli, E.van Herwijnen
//  Created    : 6/11/2006
//  Changes    : 31/01/2007. Rewrite to avoid using AIDA or Root.
//
//====================================================================
// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "RTL/rtl.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

// for adder 
#include "DimInfoTitle.h"
#include "Adder.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Adder)


// Constructor
//------------------------------------------------------------------------------
Adder::Adder(const std::string& name, ISvcLocator* ploc)
  : Algorithm(name, ploc), m_nodename ("node00101"), m_histogramname(),
  m_refreshtime(10),m_dimclientdns(),m_servername("node00101"){
  declareProperty("nodename",m_nodename);
  declareProperty("histogramname",m_histogramname);
  declareProperty("algorithmname",m_algorithmname);
  declareProperty("taskname",m_taskname);
  declareProperty("refreshtime",m_refreshtime);
  declareProperty("dimclientdns",m_dimclientdns);
  declareProperty("servername",m_servername);
  }

//------------------------------------------------------------------------------
StatusCode Adder::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to initialize Algorithm" << endreq;
    return sc;
  }

  svcsize=m_histogramname.size();
  msg << MSG::INFO << "Looking for " << svcsize << " histograms." << endreq;
  counter=0;

  hinfo.clear();
  hinfo2d.clear();
  pinfo.clear();
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  char *service; 
  char *format;
  int type;

  std::string hSvcnames;
  std::string hSvcnames2d;
  std::string pSvcnames;
  std::string commentSvcnames;
  std::string adderhSvcnames;
  std::string adderhSvcnames2d;  
  std::string adderpSvcnames;  
  std::string addercommentSvcnames;  
  std::string servicestr;

  //counter keeps track of the number of iterations over the event loop
  //the adder main program creates dimservice objects
  //and creates an array of them
  //the adding is triggered by the dim timer
  
  counter++;


  for (int j=0; j<= (int)m_histogramname.size()-1;j++) {
     //j counts the histograms

     hSvcnames="H1D/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];
     hSvcnames2d="H2D/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];
     pSvcnames="HPD/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];    
     adderhSvcnames="H1D/"+m_servername+"_Adder_1/"+m_taskname[j]+"/"+m_algorithmname[j]+"/"+m_histogramname[j];
     adderhSvcnames2d="H2D/"+m_servername+"_Adder_1/"+m_taskname[j]+"/"+m_algorithmname[j]+"/"+m_histogramname[j];
     adderpSvcnames="HPD/"+m_servername+"_Adder_1/"+m_taskname[j]+"/"+m_algorithmname[j]+"/"+m_histogramname[j];
     addercommentSvcnames=m_servername+"_Adder_1/"+m_taskname[j]+"/"+m_algorithmname[j]+"/"+m_histogramname[j]+"/gauchocomment";  
     std::vector<std::string> tmphSvcnames;	    
     std::vector<std::string> tmphSvcnames2d;	
     std::vector<std::string> tmppSvcnames;
     std::vector<DimInfoHistos*>	tmphinfo;
     std::vector<DimInfoHistos*>	tmphinfo2d;
     std::vector<DimInfoHistos*>	tmppinfo;
     DimInfoHistos * temp;

     commentSvcnames=m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j]+"/gauchocomment";
 //    std::vector<std::string> tmpcommentSvcnames;
     std::string tmpcommentSvcnames;	     
     if (counter==1) {
        //do this once, at startup of the adder to see if Dim services
	//exist for the histograms given in the joboptions
	if (j==0) {	   
	   DimClient::setDnsNode(m_dimclientdns.c_str());
           DimBrowser dbr;  
	}
        msg << MSG::DEBUG << "Looking for hSvcname: " << hSvcnames.c_str() << endreq;     
        dbr.getServices(hSvcnames.c_str());
	icount=0;
	icount2d=0;
	icountp=0;

        while( (type = dbr.getNextService(service, format)) )
        {               
	   servicestr=service;
	   std::string::size_type loc=servicestr.find("H1D/"+m_nodename+"_Adder_",0);
	   if (loc == std::string::npos ) {  
	      tmphSvcnames.push_back(servicestr);
     //         msg << MSG::DEBUG << "Found service: " << servicestr  <<endreq;     
	      icount++;
	   }
	}  
        msg << MSG::DEBUG << "Looking for hSvcname2d: " << hSvcnames2d.c_str() << endreq;     
        dbr.getServices(hSvcnames2d.c_str());
        while( (type = dbr.getNextService(service, format)) )
        { 	
	   servicestr=service;
	   std::string::size_type loc2d=servicestr.find("H2D/"+m_nodename+"_Adder_",0);
	   if (loc2d == std::string::npos ) {
	      tmphSvcnames2d.push_back(servicestr);
         //     msg << MSG::DEBUG << "Found 2D service: " << servicestr << endreq;   
	      icount2d++;  	         
	   }
	}   
        msg << MSG::DEBUG << "Looking for pSvcname: " << pSvcnames.c_str() << endreq;     
        dbr.getServices(pSvcnames.c_str());
        while( (type = dbr.getNextService(service, format)) )
        { 	
	   servicestr=service;
	   std::string::size_type locp=servicestr.find("HPD/"+m_nodename+"_Adder_",0);
	   if (locp == std::string::npos ) {
	      tmppSvcnames.push_back(servicestr);
	//      msg << MSG::DEBUG << "Found profile service: " << servicestr << endreq;   
	      icountp++;  	            
	    }
	 } 
	 nbof1dhistos.push_back(icount);
         nbof2dhistos.push_back(icount2d);
         nbofphistos.push_back(icountp); 
	 hSvcname.push_back(tmphSvcnames);
         hSvcname2d.push_back(tmphSvcnames2d);
         pSvcname.push_back(tmppSvcnames);
	 dbr.getServices(commentSvcnames.c_str());
         while( (type = dbr.getNextService(service, format)) )
         {               
	   servicestr=service;
	   std::string::size_type loc=servicestr.find(m_nodename+"_Adder_",0);
	   if (loc == std::string::npos ) {  
	      tmpcommentSvcnames=servicestr;
              msg << MSG::DEBUG << "Found comment service: " << servicestr  <<endreq;     
	      break;
	   }
	} 


        if (icount>0) {
           for (int i=0;i<=icount-1;i++) { 
	      temp = new DimInfoHistos(hSvcname[j][i],m_refreshtime);      
	      tmphinfo.push_back(temp);
	   //   msg << MSG::DEBUG << "Created object: " << hSvcname[j][i] << endreq;      
	      
	   }
        }	
	if (icount2d>0) {
           for (int i=0;i<=icount2d-1;i++) { 
	      temp = new DimInfoHistos(hSvcname2d[j][i],m_refreshtime);        
	      tmphinfo2d.push_back(temp);
	   //   msg << MSG::DEBUG << "Created object: " << hSvcname2d[j][i] << endreq;       
	   }  
        }
	if (icountp>0) {
           for (int i=0;i<=icountp-1;i++) { 
	      temp = new DimInfoHistos(pSvcname[j][i],m_refreshtime);         
	      tmppinfo.push_back(temp);
	  //    msg << MSG::DEBUG << "Created object: " << pSvcname[j][i] << endreq;       

	   }    
        }
	//update the array of dim buffers  
	hinfo.push_back(tmphinfo);  
	hinfo2d.push_back(tmphinfo2d);  
	pinfo.push_back(tmppinfo); 
        DimInfoTitle * ttemp=0;
        char* temptitle=0;
        if (icount>0) {
	   //   ttemp = new DimInfoTitle(commentSvcname[j][0]);
	      ttemp = new DimInfoTitle(tmpcommentSvcnames);
	      temptitle=ttemp->getTitle();	     
              hinfo[j][0]->declareTitleInfo(addercommentSvcnames,temptitle);  
	      hinfo[j][0]->declareInfo(adderhSvcnames);
	      //initialise the sum
	      hinfo[j][0]->add(hinfo[j][0]);
        }
  	   
        if (icount2d>0) {
	   //   ttemp = new DimInfoTitle(commentSvcname[j][0]);
	      ttemp = new DimInfoTitle(tmpcommentSvcnames);
	      temptitle=ttemp->getTitle();	     
              hinfo2d[j][0]->declareTitleInfo(addercommentSvcnames,temptitle);  
	      hinfo2d[j][0]->declareInfo(adderhSvcnames2d);
	      //initialise the sum
	      hinfo2d[j][0]->add2d(hinfo2d[j][0]);

        }
        if (icountp>0) {
	    //  ttemp = new DimInfoTitle(commentSvcname[j][0]);
	      ttemp = new DimInfoTitle(tmpcommentSvcnames);
	      temptitle=ttemp->getTitle();	     
              pinfo[j][0]->declareTitleInfo(addercommentSvcnames,temptitle); 
	      pinfo[j][0]->declareInfo(adderpSvcnames);
	      //initialise the sum
	      pinfo[j][0]->addp(pinfo[j][0]);

        }
     } //loop over counter
  } //loop over j

  if (counter==1) { 
    for (int j=0; j<= (int)m_histogramname.size()-1;j++)  {
	if (nbof1dhistos[j]>0) {     
	      infohistos.push_back(hinfo[j]);
	   }
	if (nbof2dhistos[j]>0) {
	      infohistos.push_back(hinfo2d[j]); 
	}
	if (nbofphistos[j]>0) {	   	 
	      infohistos.push_back(pinfo[j]);
	}
      }

   
     msg << MSG::DEBUG << "Making Tim for " << infohistos.size() << " histograms" << endreq;  
     tim= new Tim(m_refreshtime,infohistos);
  } 	
  
 // lib_rtl_sleep(m_refreshtime*100); 
  dim_wait();
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  return StatusCode::SUCCESS;
}


