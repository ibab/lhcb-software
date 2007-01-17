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
#include "Adder.h"

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
#include "TAxis.h"
#include "TSystem.h"

// for adder 
#include "DimInfoHistos.h"
#include "BaseHistogram.h"
#include "GaudiPI/AIDA_ROOT/Histogram1D.h"
#include "GaudiPI/AIDA_ROOT/Histogram2D.h"
#include "GaudiPI/AIDA_ROOT/Profile1D.h"

// for online monitoring
#include "GaudiKernel/IMonitorSvc.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "RTL/rtl.h"

// Static Factory declaration
static const AlgFactory<Adder>  Factory;
const IAlgFactory& AdderFactory = Factory;

// Constructor
//------------------------------------------------------------------------------
Adder::Adder(const std::string& name, ISvcLocator* ploc)
  : Algorithm(name, ploc), m_nodename ("node00101"), m_histogramname(),
  m_refreshtime(10),m_dimclientdns(){
  declareProperty("nodename",m_nodename);
  declareProperty("histogramname",m_histogramname);
  declareProperty("refreshtime",m_refreshtime);
  declareProperty("dimclientdns",m_dimclientdns);
  }

//------------------------------------------------------------------------------
StatusCode Adder::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());

  sc = service("HistogramDataSvc", histoSvc, true );
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }

  msg << MSG::DEBUG << "Looking for " << m_histogramname.size() << " histograms." << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  msg << MSG::INFO << "executing adder...." << endreq;
  char *service; 
  char *format;
  int type;
  int icount;
  int icount2d;
  int icountp;
  int h2dsize;
  int hsize;
  int psize;
  std::vector<std::string> hSvcname;
  std::vector<std::string> hSvcname2d;
  std::vector<std::string> pSvcname;
  std::string hSvcnames;
  std::string hSvcnames2d;
  std::string pSvcnames;
  std::string servicestr;
  std::vector<DimInfoHistos*> hinfo;
  std::vector<DimInfoHistos*> hinfo2d;
  std::vector<DimInfoHistos*> pinfo;
  std::vector<AIDA::IHistogram1D*> h;
  std::vector<AIDA::IHistogram2D*> h2d;
  std::vector<AIDA::IProfile1D*> p;
  typedef AIDA_ROOT::BaseHistogram<TH1D,AIDA::IHistogram> rootpart;
  typedef AIDA_ROOT::BaseHistogram<TH2D,AIDA::IHistogram> rootpart2d;
  typedef AIDA_ROOT::BaseHistogram<TProfile,AIDA::IProfile> rootpartp;
  std::vector<rootpart*> base;
  std::vector<rootpart2d*> base2d;
  std::vector<rootpartp*> basep;
  
  for (int j=0; j<= (int)m_histogramname.size()-1;j++) {
     //j counts the histograms
     std::vector<TH1*> hist;
     std::vector<TH2*> hist2d;
     std::vector<TProfile*> histp;
     hSvcnames="H1D/"+m_nodename+"*/"+m_histogramname[j];
     hSvcnames2d="H2D/"+m_nodename+"*/"+m_histogramname[j];
     pSvcnames="HPD/"+m_nodename+"*/"+m_histogramname[j];
     
     DimClient::setDnsNode(m_dimclientdns.c_str());
  
  
     DimBrowser dbr;  
     icount = 0;
     icount2d = 0;
     icountp = 0;
     int jindex=0;
      // icount is the number of tasks publishing histogram nr j
     while ((( icount==0 )|(icount2d==0))|(icountp==0))
        {
        //look for the services at each iteration of eventloop - task may have died
        msg << MSG::DEBUG << "Looking for hSvcname: " << hSvcnames.c_str() << endreq;     
        dbr.getServices(hSvcnames.c_str());
        hSvcname.clear();
        while( (type = dbr.getNextService(service, format)) )
           { 
           servicestr=service;
	   std::string::size_type loc=servicestr.find("H1D/"+m_nodename+"_Adder_",0);
	   if (loc == std::string::npos ) {
	      hSvcname.push_back(servicestr);
              msg << MSG::DEBUG << "Found service: " << servicestr << endreq;     
	      icount++;
	      }
	   }   
	if (icount==0) {
           msg << MSG::DEBUG << "Looking for hSvcname2d: " << hSvcnames2d.c_str() << endreq;     
           dbr.getServices(hSvcnames2d.c_str());
           hSvcname2d.clear();

           while( (type = dbr.getNextService(service, format)) )
              { 	
	      servicestr=service;
	      std::string::size_type loc2d=servicestr.find("H2D/"+m_nodename+"_Adder_",0);
	         if (loc2d == std::string::npos ) {
	            hSvcname2d.push_back(servicestr);
                    msg << MSG::DEBUG << "Found 2D service: " << servicestr << endreq;   
  	            icount2d++;
	         }
	      }  
	   if (icount2d==0) {
              msg << MSG::DEBUG << "Looking for pSvcname: " << pSvcnames.c_str() << endreq;     
              dbr.getServices(pSvcnames.c_str());
              pSvcname.clear();

              while( (type = dbr.getNextService(service, format)) )
                 { 	
	         servicestr=service;
	         std::string::size_type locp=servicestr.find("HPD/"+m_nodename+"_Adder_",0);
	            if (locp == std::string::npos ) {
	               pSvcname.push_back(servicestr);
                       msg << MSG::DEBUG << "Found profile service: " << servicestr << endreq;   
  	               icountp++;
	            }
	         } 
	   } 
	}        
        if (((icount>1)|(icount2d>1))|(icountp>1)) break;      
        else lib_rtl_sleep(m_refreshtime*1000);
     }  
	     
     DimInfoHistos * temp;
     hinfo.clear();
     for (int i=0;i<=icount-1;i++) {
        temp = new DimInfoHistos(hSvcname[i],m_refreshtime);
        hinfo.push_back(temp);
     }
     hinfo2d.clear();
     for (int i=0;i<=icount2d-1;i++) {
        temp = new DimInfoHistos(hSvcname2d[i],m_refreshtime);
        hinfo2d.push_back(temp);
     }
     pinfo.clear();
     for (int i=0;i<=icountp-1;i++) {
        temp = new DimInfoHistos(pSvcname[i],m_refreshtime);
        pinfo.push_back(temp);
     }

     // convert DIM buffer to ROOT

    
     for (int i=0;i<=icount-1;i++) {
        TH1* hist1=0;
        int ntries=0;
        while (1)
           {
           hist1=0;

	   hist1= hinfo[i]->getHisto();
           if (hist1 != 0) {
	      msg << MSG::DEBUG << "Histogram "<< hist1->GetTitle() << " found." << endreq;   
	      hist.push_back(hist1);		 
	      break;
	   }    
           ntries++;
	   msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
           if(ntries==10) break;
           lib_rtl_sleep(m_refreshtime*500); 
        } 
     } 
     for (int i=0;i<=icount2d-1;i++) {
        TH2* hist1=0;
        int ntries=0;         
        while (1)
           {
           hist1=0;

	   hist1= hinfo2d[i]->get2DHisto();
           if (hist1 != 0) {
	      msg << MSG::DEBUG << "Histogram "<< hist1->GetTitle() << " found." << endreq;   
	      hist2d.push_back(hist1);		 
	      break;
	   }    
           ntries++;
	   msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
           if(ntries==10) break;
           lib_rtl_sleep(m_refreshtime*500); 
        } 
     }   
     for (int i=0;i<=icountp-1;i++) {
        TProfile* hist1=0;
        int ntries=0;         
        while (1)
           {
           hist1=0;

	   hist1= pinfo[i]->getpHisto();
           if (hist1 != 0) {
	      msg << MSG::DEBUG << "Histogram "<< hist1->GetTitle() << " found." << endreq;   
	      histp.push_back(hist1);		 
	      break;
	   }    
           ntries++;
	   msg << MSG::DEBUG << "No histogram found after " << ntries << " attempts." << endreq;
           if(ntries==10) break;
           lib_rtl_sleep(m_refreshtime*500); 
        } 
     } 
     std::string title;
     std::string title2d;
     std::string titlep;
     int numberOfBinsX=0;
     int numberOfBinsY=0;
     double lowerEdge=0;
     double upperEdge=0;
     double ylowerEdge=0;
     double yupperEdge=0;
     TAxis * axis; 
     TAxis * yaxis; 
     if (icount>0) {
        title = hist[1]->GetTitle(); 
        numberOfBinsX = hist[1]->GetNbinsX();
        axis = hist[1]->GetXaxis(); 
   	lowerEdge = (hist[1]->GetXaxis() )->GetXmin(); 
        upperEdge = (hist[1]->GetXaxis() )->GetXmax(); 
     }	
     if (icount2d>0) {
        title2d = hist2d[1]->GetTitle(); 
        numberOfBinsX = hist2d[1]->GetNbinsX();
	numberOfBinsY = hist2d[1]->GetNbinsY();
        axis = hist2d[1]->GetXaxis(); 
	yaxis = hist2d[1]->GetYaxis(); 
	lowerEdge = (hist2d[1]->GetXaxis() )->GetXmin(); 
        upperEdge = (hist2d[1]->GetXaxis() )->GetXmax(); 
	ylowerEdge = (hist2d[1]->GetYaxis() )->GetXmin(); 
        yupperEdge = (hist2d[1]->GetYaxis() )->GetXmax(); 
     }
     if (icountp>0) {
        titlep = hist[1]->GetTitle(); 
        numberOfBinsX = hist[1]->GetNbinsX();
        axis = hist[1]->GetXaxis(); 
   	lowerEdge = (hist[1]->GetXaxis() )->GetXmin(); 
        upperEdge = (hist[1]->GetXaxis() )->GetXmax(); 
     }		
 
     //check if the summed histogram was already booked, ie has an entry on the transient store
 
     std::string  m_rootName;
     SmartDataPtr<DataObject> root(histoSvc,m_rootName);
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
     bool booked = false;	     	    	    
     SmartIF<IDataManagerSvc> mgr(histoSvc);
     if ( mgr )    {
        typedef std::vector<IRegistry*> Leaves;
        Leaves leaves;
        StatusCode sc = mgr->objectLeaves(pObj, leaves);
        if ( sc.isSuccess() )  {
         //  msg << MSG::INFO << "Found leaves. "  << endreq;
           for ( Leaves::const_iterator i=leaves.begin(); 
              i != leaves.end(); i++ )   {
              const std::string& id = (*i)->identifier();
              msg << MSG::DEBUG << "Histogram found on TES with id " << id  <<endreq;
	      if (id=="/stat/adder"+m_histogramname[j]) booked = true;
           }
        }  
     } 
  
  
     //if ( ! axis->IsVariableBinSize()) {  
     // equal bins histograms

  
     // Create the AIDA histogram that will contain the summed result
     // and that will be published
     
     if (!booked) { 
        if (icount >0 ) { 
	   if (h.size() <= 0) {
	      hsize=0;
	   }   
	   else {
	      hsize=h.size();
	   } 
	   h.push_back(histoSvc->book("/stat/adder"+m_histogramname[j],j, m_histogramname[j], numberOfBinsX,lowerEdge, upperEdge));
           declareInfo("Summed_"+m_histogramname[j],h[hsize],"Summed_"+m_histogramname[j]);
           base.push_back(dynamic_cast<rootpart*>(h[hsize]));    
           if ( ! base[hsize] ) { 
              msg << MSG::ERROR << "Dynamic cast failed to get root part of aida h"  << endreq;       
	   }
           // representation gives us now the ROOT part of h[hsize]
           // sum is a ROOT object   
	   sum.push_back(dynamic_cast<TH1*>(&base[hsize]->representation()));
	   if ( ! sum[hsize] ) { 
	      msg << MSG::ERROR <<  "Dynamic cast failed from rootpart base into th1 sum" << endreq;       
	   }
	   j1D.push_back(j);


	 } 
	   	  
       if (icount2d >0 ) {   
           if (h2d.size() <= 0) {
	      h2dsize=0;
	   }   
	   else {
	      h2dsize=h2d.size();
	   } 
	   h2d.push_back(histoSvc->book("/stat/adder"+m_histogramname[j],j,m_histogramname[j], numberOfBinsX,lowerEdge, upperEdge, numberOfBinsY,ylowerEdge, yupperEdge));
	   declareInfo("Summed_"+m_histogramname[j],h2d[h2dsize],"Summed_"+m_histogramname[j]);
           base2d.push_back(dynamic_cast<rootpart2d*>(h2d[h2dsize]));    
           if ( ! base2d[h2dsize] ) { 
              msg << MSG::ERROR << "Dynamic cast failed to get root part of aida h2d"  << endreq;       
	   }
           // representation gives us now the ROOT part of h2d[h2dsize]
           // sum is a ROOT object   
	   sum2d.push_back(dynamic_cast<TH2*>(&base2d[h2dsize]->representation()));
	   if ( ! sum2d[h2dsize] ) { 
	      msg << MSG::ERROR <<  "Dynamic cast failed from rootpart base into th2 sum" << endreq;       
	   }

	   
	   j2D.push_back(j);
	}
	if (icountp >0 ) { 
	   if (p.size() <= 0) {
	      psize=0;
	   }   
	   else {
	      psize=p.size();
	   } 
	   p.push_back(histoSvc->bookProf("/stat/adder"+m_histogramname[j],j, m_histogramname[j], numberOfBinsX,lowerEdge, upperEdge));
           declareInfo("Summed_"+m_histogramname[j],p[psize],"Summed_"+m_histogramname[j]);
           basep.push_back(dynamic_cast<rootpartp*>(p[psize]));    
           if ( ! base[psize] ) { 
              msg << MSG::ERROR << "Dynamic cast failed to get root part of aida p"  << endreq;       
	   }
           // representation gives us now the ROOT part of p[psize]
           // sum is a ROOT object   
	   sump.push_back(dynamic_cast<TProfile*>(&basep[psize]->representation()));
	   if ( ! sump[psize] ) { 
	      msg << MSG::ERROR <<  "Dynamic cast failed from rootpart base into tprofile sum" << endreq;       
	   }
	   jpD.push_back(j);


	 } 

     }	 
     if (icount >0 ) { 
         //find the index of the sum
	 for (jindex=0;jindex<=j;jindex++) {
	    if (j1D[jindex]==j ) break;
	 } 
     	 
         if ( ! sum[jindex] ) {
            msg << MSG::ERROR <<  "no root histogram available for summing" << endreq;       
         }
         else {
            msg << MSG::INFO <<  "Summing..." << m_histogramname[j] << endreq;       
            sum[jindex]->Reset();
            for (int i=0;i<=icount-1;i++) {
               sum[jindex]->Add(hist[i],1.);
            }
         }
      }
      if (icount2d >0 ) { 
         //find the index of the sum
	 for (jindex=0;jindex<=j;jindex++) {
	    if (j2D[jindex]==j ) break;
	 } 
        if ( ! sum2d[jindex] ) {
           msg << MSG::ERROR <<  "no root histogram available for summing" << endreq;       
        }
        else {
           msg << MSG::INFO <<  "Summing..." << m_histogramname[j]  << endreq;       
           sum2d[jindex]->Reset();
           for (int i=0;i<=icount2d-1;i++) {
              sum2d[jindex]->Add(hist2d[i],1.);
           }
        }
      }	
      if (icountp >0 ) { 
         //find the index of the sum
	 for (jindex=0;jindex<=j;jindex++) {
	    if (jpD[jindex]==j ) break;
	 } 
     	 
         if ( ! sump[jindex] ) {
            msg << MSG::ERROR <<  "no root histogram available for summing" << endreq;       
         }
         else {
            msg << MSG::INFO <<  "Summing..." << m_histogramname[j] << endreq;       
            sump[jindex]->Reset();
            for (int i=0;i<=icountp-1;i++) {
               sump[jindex]->Add(histp[i],1.);
            }
         }
      }	  
     
 //   lib_rtl_sleep(m_refreshtime*1000); 
 
   }
 
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;

  return StatusCode::SUCCESS;
}
