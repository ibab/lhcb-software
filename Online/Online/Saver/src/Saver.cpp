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
  declareProperty("nodename",m_nodename);
  declareProperty("histogramname",m_histogramname);
  declareProperty("algorithmname",m_algorithmname);
  declareProperty("dimclientdns",m_dimclientdns);
  declareProperty("taskname",m_taskname);
  declareProperty("savedir",m_savedir);
  }

//------------------------------------------------------------------------------
StatusCode Saver::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  IIncidentSvc* incsvc;
  sc = service("IncidentSvc",incsvc);
  int priority=100;
  m_refreshtime=5;
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
//  msg << MSG::INFO << "executing saver....command " << command << endreq;

  char *service; 
  char *format;
  time_t rawtime;
  struct tm * timeinfo;
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
  DimInfoHistos* hinfo=0;
  DimInfoHistos* hinfo2d=0;
  DimInfoHistos* pinfo=0;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  int year=timeinfo->tm_year+1900;
  int day =timeinfo->tm_mday;
  int month=timeinfo->tm_mon+1;
  int hour=timeinfo->tm_hour;
  int min=timeinfo->tm_min;
  int sec=timeinfo->tm_sec;
  std::string daystr;
  std::string yearstr;
  std::string monthstr;
  std::string hourstr;
  std::string minstr;
  std::string secstr;
  std::string filename;
  std::stringstream yearsstr;
  std::stringstream daysstr;
  std::stringstream monthsstr;
  std::stringstream hoursstr;
  std::stringstream minsstr;
  std::stringstream secsstr;
  yearsstr << year;
  daysstr << day;
  monthsstr << month;
  hoursstr << hour;
  minsstr << min;
  secsstr << sec;
  yearsstr >> yearstr;
  daysstr >> daystr;
  monthsstr >> monthstr;
  hoursstr >> hourstr;
  minsstr >> minstr;
  secsstr >> secstr; 
  TFile *f=0;
  if (command=="SAVE_HISTOS") {
  //NB the ASCII timestamp format is year-month-dayThours:mins:secs
  //we replace ":" by "-" as you can't use it for windows filenames
  filename=m_savedir+"/"+m_taskname+"-"+yearstr+"-"+monthstr+"-"+daystr+"T"+hourstr+"-"+minstr+"-"+secstr+".root";
  
  for (int j=0; j<= (int)m_histogramname.size()-1;j++) {
 
     //j counts the histograms
     std::vector<TH1*> hist;
     std::vector<TH2*> hist2d;
     std::vector<TProfile*> histp;
     hSvcnames="H1D/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];
     hSvcnames2d="H2D/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];
     pSvcnames="HPD/"+m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j];
     commentSvcnames=m_nodename+"*"+m_algorithmname[j]+"/"+m_histogramname[j]+"/gauchocomment";
     
     DimClient::setDnsNode(m_dimclientdns.c_str());  
     DimBrowser dbr;  
     icount = 0;
     icount2d = 0;
     icountp = 0;
      // icount is the number of tasks publishing histogram nr j
     while ((( icount==0 )|(icount2d==0))|(icountp==0))
     {
        //look for the services at each iteration of eventloop - task may have died

	msg << MSG::DEBUG << "Looking for hSvcname: " << hSvcnames.c_str() << endreq;     
        dbr.getServices(hSvcnames.c_str());
        while( (type = dbr.getNextService(service, format)) )
        { 
	   hSvcname=service;
           msg << MSG::DEBUG << "Found service: " << hSvcname << endreq;     
	   icount++;

	}   
	if (icount==0) {
           msg << MSG::DEBUG << "Looking for hSvcname2d: " << hSvcnames2d.c_str() << endreq;     
           dbr.getServices(hSvcnames2d.c_str());
           while( (type = dbr.getNextService(service, format)) )
           { 	
	      hSvcname2d=service;
              msg << MSG::DEBUG << "Found 2D service: " << hSvcname2d << endreq;   
  	      icount2d++;
	   } 
	}  
	if (icount2d==0) {
           msg << MSG::DEBUG << "Looking for pSvcname: " << pSvcnames.c_str() << endreq;     
           dbr.getServices(pSvcnames.c_str());
           while( (type = dbr.getNextService(service, format)) )
           { 	
	      pSvcname=service;
              msg << MSG::DEBUG << "Found profile service: " << hSvcname2d << endreq;   
  	      icountp++;		 
	   } 
	} 
 	dbr.getServices(commentSvcnames.c_str());
        while( (type = dbr.getNextService(service, format)) )
        {               
	   commentSvcname=service;
           msg << MSG::DEBUG << "Found comment service: " << commentSvcname  <<endreq;     
	}
        if (((icount>0)|(icount2d>0))|(icountp>1)) break;      
        else lib_rtl_sleep(500);
     } 
  

     if (icount>0) {     
        hinfo = new DimInfoHistos(hSvcname,m_refreshtime);
     }
     if (icount2d>0) {
        hinfo2d = new DimInfoHistos(hSvcname2d,m_refreshtime);
     }
     if (icountp>0) {
        pinfo = new DimInfoHistos(pSvcname,m_refreshtime);
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
	      ttemp = new DimInfoTitle(commentSvcname,m_refreshtime);
	      lib_rtl_sleep(m_refreshtime*100);    	
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
	      ttemp = new DimInfoTitle(commentSvcname,m_refreshtime);
	      lib_rtl_sleep(m_refreshtime*100);    	
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
	      ttemp = new DimInfoTitle(commentSvcname,m_refreshtime);
	      lib_rtl_sleep(m_refreshtime*100);    	
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
