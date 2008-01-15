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
#include "Saver.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "TApplication.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TAxis.h"
#include "TSystem.h"
#include <time.h>


// for saver
#include "DimInfoHistos.h"
#include "DimInfoTitle.h"

// for online monitoring
#include "GaudiKernel/IMonitorSvc.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RTL/rtl.h"


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Saver)

// Constructor
//------------------------------------------------------------------------------
Saver::Saver(const std::string& name, ISvcLocator* ploc) : Algorithm(name, ploc)
  {
  declareProperty("nodename",m_nodeName);
  declareProperty("histogramname",m_histogramName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("taskname",m_taskName);
  declareProperty("savedir",m_saveDir);
  }

//------------------------------------------------------------------------------
StatusCode Saver::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  IIncidentSvc* incsvc;
  sc = service("IncidentSvc",incsvc);
  int priority=100;
  m_refreshTime=5;
  if (sc.isSuccess()) incsvc->addListener(this,"SAVE_HISTOS",priority);
  MsgStream msg(msgSvc(), name());

  sc = service("HistogramDataSvc", histoSvc, true );
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Saver::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );

  char *service; 
  char *format;
  time_t rawTime;
  struct tm *timeInfo;
  int type;
  int icount=0;
  int icount2d=0;
  int icountp=0;
  std::string hSvcName;
  std::string hSvcName2d;
  std::string hSvcNames;
  std::string hSvcNames2d;
  std::string pSvcName;
  std::string pSvcNames;
  std::string commentSvcNames;
  std::string commentSvcName;
  std::string filename;
  char year[5];
  char month[3];
  char day[3];
  char hour[3];
  char min[3];
  char sec[3];
  DimInfoHistos* hinfo=0;
  DimInfoHistos* hinfo2d=0;
  DimInfoHistos* pinfo=0;
  rawTime=time(NULL);
  timeInfo = localtime(&rawTime);
  strftime(year,sizeof(year),"%Y",timeInfo);
  strftime(month,sizeof(month),"%m",timeInfo);
  strftime(day,sizeof(day),"%d",timeInfo);
  strftime(hour,sizeof(hour),"%H",timeInfo);
  strftime(min,sizeof(min),"%M",timeInfo);
  strftime(sec,sizeof(sec),"%S",timeInfo);
  TFile *f=0;
  if (command=="SAVE_HISTOS") {
  //NB the ISO extended timestamp format is yearmonthdayThoursminssecs
  //can't use separators as ":" is no good for windows filenames
  //stick strictly to this standard so boost can parse it
  filename=m_saveDir+"/"+m_taskName+year+month+day+"T"+hour+min+sec+".root";  
  msg << MSG::DEBUG << "Saving to filename: " <<filename.c_str() << endreq;  
  for (int j=0; j<= (int)m_histogramName.size()-1;j++) { 
    //j counts the histograms
    std::vector<TH1*> hist;
    std::vector<TH2*> hist2d;
    std::vector<TProfile*> histp;
    hSvcNames="H1D/"+m_nodeName+"*"+m_algorithmName[j]+"/"+m_histogramName[j];
    hSvcNames2d="H2D/"+m_nodeName+"*"+m_algorithmName[j]+"/"+m_histogramName[j];
    pSvcNames="HPD/"+m_nodeName+"*"+m_algorithmName[j]+"/"+m_histogramName[j];
    commentSvcNames=m_nodeName+"*"+m_algorithmName[j]+"/"+m_histogramName[j]+"/gauchocomment";
     
    DimClient::setDnsNode(m_dimClientDns.c_str());  
    DimBrowser dbr;  
    icount = 0;
    icount2d = 0;
    icountp = 0;
     // icount is the number of tasks publishing histogram nr j
    while ((( icount==0 )|(icount2d==0))|(icountp==0))
    {
      //look for the services at each iteration of eventloop - task may have died
      msg << MSG::DEBUG << "Looking for hSvcName: " << hSvcNames.c_str() << endreq;     
      dbr.getServices(hSvcNames.c_str());
      while( (type = dbr.getNextService(service, format)) )
      { 
	hSvcName=service;
        msg << MSG::DEBUG << "Found service: " << hSvcName << endreq;     
	icount++;
      }   
      if (icount==0) {
        msg << MSG::DEBUG << "Looking for hSvcName2d: " << hSvcNames2d.c_str() << endreq;     
        dbr.getServices(hSvcNames2d.c_str());
        while( (type = dbr.getNextService(service, format)) )
        { 	
	  hSvcName2d=service;
          msg << MSG::DEBUG << "Found 2D service: " << hSvcName2d << endreq;   
  	  icount2d++;
	} 
      }  
      if (icount2d==0) {
        msg << MSG::DEBUG << "Looking for pSvcName: " << pSvcNames.c_str() << endreq;     
        dbr.getServices(pSvcNames.c_str());
        while( (type = dbr.getNextService(service, format)) )
        { 	
	  pSvcName=service;
          msg << MSG::DEBUG << "Found profile service: " << hSvcName2d << endreq;   
  	  icountp++;		 
	} 
      } 
      dbr.getServices(commentSvcNames.c_str());
      while( (type = dbr.getNextService(service, format)) )
      {               
	commentSvcName=service;
        msg << MSG::DEBUG << "Found comment service: " << commentSvcName  <<endreq;     
      }
      if (((icount>0)|(icount2d>0))|(icountp>1)) break;      
      else lib_rtl_sleep(500);
    } 
  

    if (icount>0) {     
      hinfo = new DimInfoHistos(hSvcName,m_refreshTime);
    }
    if (icount2d>0) {
      hinfo2d = new DimInfoHistos(hSvcName2d,m_refreshTime);
    }
    if (icountp>0) {
      pinfo = new DimInfoHistos(pSvcName,m_refreshTime);
    }

    // convert DIM buffer to ROOT


    if (icount>0) {
      TH1* hist1=0;
      int ntries=0;
      DimInfoTitle * ttemp;
      char* temptitle=0;  
      while (1)
      {
        hist1=0;
	hist1= hinfo->getHisto();
        if (hist1 != 0) {
	  ttemp = new DimInfoTitle(commentSvcName,m_refreshTime);
	  lib_rtl_sleep(m_refreshTime*100);    	
	  temptitle =ttemp->getTitle();
          if (temptitle!=0) { 		 
	    hist1->SetTitle(temptitle);
	  }

	  msg << MSG::DEBUG << "Histogram "<< hist1->GetTitle() << " found." << endreq;   
	  if ( 0 == f ) {
	    f = new TFile(filename.c_str(),"create"); 
	  }
	  hist1->Write();
	 
	  break;
	}  	     
        ntries++;
	msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
        if(ntries==10) break;
        lib_rtl_sleep(500); 
      } 
    } 
    if (icount2d>0) {
      TH2* hist2=0;
      int ntries=0;  
      DimInfoTitle * ttemp;
      char* temptitle=0;         
      while (1)
      {
        hist2=0;
        hist2= hinfo2d->get2DHisto();
        if (hist2 != 0) {
	  ttemp = new DimInfoTitle(commentSvcName,m_refreshTime);
	  lib_rtl_sleep(m_refreshTime*100);    	
	  temptitle =ttemp->getTitle();
          if (temptitle!=0) { 		 
	    hist2->SetTitle(temptitle);
	  }
	  msg << MSG::DEBUG << "Histogram "<< hist2->GetTitle() << " found." << endreq;   
	  if ( 0 == f ) {	      
	    f = new TFile(filename.c_str(),"create");
	  }   
	  hist2->Write();		 
	  break;
        }    
        ntries++;
        msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
        if(ntries==10) break;
        lib_rtl_sleep(500); 
      } 
    }   

    if (icountp>0) {
      TProfile* histp=0;
      int ntries=0;
      DimInfoTitle * ttemp;
      char* temptitle=0;           
      while (1)
      {
        histp=0;
        histp= pinfo->getPDHisto();
        if (histp != 0) {
	  ttemp = new DimInfoTitle(commentSvcName,m_refreshTime);
	  lib_rtl_sleep(m_refreshTime*100);    	
	  temptitle =ttemp->getTitle();
          if (temptitle!=0) { 		 
	    histp->SetTitle(temptitle);
	  }
	  msg << MSG::DEBUG << "Histogram "<< histp->GetTitle() << " found." << endreq;   
	  if ( 0 == f ) {	      
	    f = new TFile(filename.c_str(),"create");
	  }   
	  histp->Write();		 
	  break;
        }    
        ntries++;
        msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
        if(ntries==10) break;
        lib_rtl_sleep(500); 
      } 
    }   	       
  }
    f->Close();
    delete f;
    delete hinfo;
    delete hinfo2d;
    delete pinfo;
    command="";
  }
 
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
void Saver::handle(const Incident& inc) {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Got incident " << inc.type() << " from " << inc.source() <<endmsg;
  command=inc.type();

}



//------------------------------------------------------------------------------
StatusCode Saver::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
//  delete f;
  return StatusCode::SUCCESS;
}
