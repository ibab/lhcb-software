//====================================================================
//  InsertDB.cpp
//--------------------------------------------------------------------
//
//  Package    : InsertDB
//
//  Description: This program inserts hlt histogram metadata in the Online
//               histogram database and makes pages
//          
//  Author     : E.van Herwijnen
//  Created    : 30/11/2009
//  Changes    : first version eff & mf only
//
//====================================================================
// Include files


#include "GaudiKernel/Bootstrap.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"

#include "RTL/rtl.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "dis.hxx"
#include "dic.hxx"
#include <time.h>

#include "InsertDB.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(InsertDB)

InsertDB::InsertDB(const std::string& name, ISvcLocator* ploc)
      :GaudiHistoAlg(name, ploc)
{  
  declareProperty("PartitionName",m_partitionname="FEST"); 
  declareProperty("TaskName",m_taskname="GauchoJob"); 
  declareProperty("AdderName",m_addername="_Adder_1"); 
  declareProperty("DNS",m_dimclientdns="mona08"); 
  declareProperty("HLTDBFolder",m_hltdbfolder=""); 
  declareProperty("HLTType",m_nickname="Physics_320Vis_300L0_10Hlt1_Aug09"); 
  declareProperty("MonitoringTasks",m_monitoringtasks); 
  declareProperty("Alleys",m_alleys);
  declareProperty("Pages",m_subfolders);
}   
   
StatusCode InsertDB::initialize() {
   //------------------------------------------------------------------------------
   StatusCode sc = Algorithm::initialize(); // must be executed first
 // return 0; 
 icount=0;
 MsgStream msg(msgSvc(), name());
 msg << MSG::INFO<< "Initializing."  << endreq; 
 if (m_partitionname=="") m_partitionname="FEST"; 
 if (m_taskname=="") m_taskname="GauchoJob";
 if (m_addername=="") m_addername="_Adder_1";
 if (m_dimclientdns=="") m_dimclientdns="mona08"; 
 mtaskcount=0;
 
 if ((int)m_alleys.size()==0) {
    //these are the alleys and alley algorithm names
    //could be put in a joboption but hardcoded for the moment
    m_alleys.push_back("AlignVelo");
    m_alleysearchstring.push_back("Hlt1AlignVelo");
    m_alleys.push_back("DiHadron");
    m_alleysearchstring.push_back("Hlt1DiHadron");
    m_alleys.push_back("DiMuon");
    m_alleysearchstring.push_back("Hlt1DiMuon");
    m_alleys.push_back("Electron");
    m_alleysearchstring.push_back("Hlt1Electron");
    m_alleys.push_back("Global");
    m_alleysearchstring.push_back("Hlt1Global");
    m_alleys.push_back("GlobalMonitor");
    m_alleysearchstring.push_back("HltGlobalMonitor");
    m_alleys.push_back("Hadron");
    m_alleysearchstring.push_back("Hlt1Hadron");
    m_alleys.push_back("Incident");
    m_alleysearchstring.push_back("Hlt1Incident");
    m_alleys.push_back("L0");
    m_alleysearchstring.push_back("Hlt1L0");
    m_alleys.push_back("Lumi");
    m_alleysearchstring.push_back("Hlt1Lumi");
    m_alleys.push_back("MuTrack");
    m_alleysearchstring.push_back("Hlt1MuTrack");
    m_alleys.push_back("Muon");
    m_alleysearchstring.push_back("Hlt1Muon");
    m_alleys.push_back("ODINTechnical");
    m_alleysearchstring.push_back("Hlt1ODINTechnical");
    m_alleys.push_back("ODINPhysics");
    m_alleysearchstring.push_back("Hlt1ODINPhysics");
    m_alleys.push_back("Photon");
    m_alleysearchstring.push_back("Hlt1Photon");
    m_alleys.push_back("SingleHadron");
    m_alleysearchstring.push_back("Hlt1SingleHadron");
    m_alleys.push_back("SingleMuon");
    m_alleysearchstring.push_back("Hlt1SingleMuon");
    m_alleys.push_back("SoftDiHadron");
    m_alleysearchstring.push_back("Hlt1SoftDiHadron");
    m_alleys.push_back("Tell1Error");
    m_alleysearchstring.push_back("Hlt1Tell1Error");
    m_alleys.push_back("Velo");
    m_alleysearchstring.push_back("Hlt1Velo");
    m_alleys.push_back("XPress");
    m_alleysearchstring.push_back("Hlt1XPress");
    m_alleys.push_back("Hlt2");
    m_alleysearchstring.push_back("Hlt2");
    m_DMalleys.push_back("GlobalMonitor");
    m_DMalleysearchstring.push_back("HltGlobalMonitor");
 }   
 
 
 return StatusCode::SUCCESS;
}

StatusCode InsertDB::execute() {
   MsgStream msg(msgSvc(), name());
   if (icount==0) {
     doEFF();
     doEFFHltExperts();
     for (int i=0;i<(int)m_monitoringtasks.size();i++) {
       doMF(m_monitoringtasks[i]);
       mtaskcount++;
     }  
   }  
   else {
     msg << MSG::INFO<< "Done." << endreq;
     exit(0);
   }
   icount++;
   return StatusCode::SUCCESS;
}

StatusCode InsertDB::finalize() {
   return StatusCode::SUCCESS;
} 


void InsertDB::doEFF() {
   //doEFF analyzes histograms published by Moore to the top level Adder
   MsgStream msg(msgSvc(), name());
   bool ok=true;

   DimClient::setDnsNode(m_dimclientdns.c_str());  
   DimBrowser dbr;
   
   msg << MSG::INFO<< "Connecting to Online HistDB" <<endreq; 

   std::string password="HISTEGGIA194";
   OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
   if (HistDB==0) {
       msg << MSG::INFO<< "Could not connect to Histogram database." << endreq;
   }
   else { 
       HistDB->setDebug(3);
       //lines contains the lines found. a line is a vector with histogram names inside it.
       std::map <std::string, std::vector<std::string> > lines;
       std::vector<std::string> pages;
       std::vector<std::string> histograms;
       
       hSvcnames="MonH1D/"+m_partitionname+m_addername+"/*";
       hSvcnames2d="MonH2D/"+m_partitionname+m_addername+"/*";
       pSvcnames="MonP1/"+m_partitionname+m_addername+"/*";
       rSvcnames=m_partitionname+m_addername+"/*monRate/*-flag";
   
       dbr.getServices(hSvcnames.c_str());
       
       // find the histograms and rates per line and declare them to the histdb
       // 1d histograms
       while( (type = dbr.getNextService(service, Dimformat)) )
       { 
	   hSvcname=service;
	   std::string prefix = "MonH1D/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	   std::string rest=hSvcname.substr(prefix.length());
	   std::string::size_type first_slash=rest.find("/",0);
	   if (first_slash != std::string::npos ) {
	     std::string line = rest.substr(0,first_slash);
	     std::string histoname = rest.substr(first_slash+1);
	     histograms.clear();
	     if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
             }
             histograms.push_back(histoname);
	     lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(hSvcname);
	} 

        //2d histograms
        dbr.getServices(hSvcnames2d.c_str());
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	    hSvcname2d=service;
	    std::string prefix = "MonH2D/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	    std::string rest=hSvcname2d.substr(prefix.length());
	    std::string::size_type first_slash=rest.find("/",0);
	    if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
            } 
	    HistDB->declareHistByServiceName(hSvcname2d);
	}	
	dbr.getServices(pSvcnames.c_str());
        //profile histograms
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	    pSvcname=service;
	    std::string prefix = "MonP1/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	      std::string rest=pSvcname.substr(prefix.length());
	      std::string::size_type first_slash=rest.find("/",0);
	      if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(pSvcname);
         }
	 
/*       disable rates for the moment
         until Giacomo's API has support for rates
	 msg << MSG::INFO<< "Looking for rate service: " << rSvcnames.c_str() << " using DNS: "<< m_dimclientdns<< endreq;     
         dbr.getServices(rSvcnames.c_str());
  
         while( (type = dbr.getNextService(service, Dimformat)) )
         { 
	      rSvcname=service;   	   
	      std::string prefix = m_partitionname+m_addername+"/"+m_taskname+"/MonitorSvc/monRate/"; 
	      std::string rest=rSvcname.substr(prefix.length());
	      std::string::size_type first_slash=rest.find("/",0);
	      if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(rSvcname);
         }
	 */
	
	//the following code is very ad-hoc, generalizations welcome
	//but at least it saves clicking about in the presenter 
	//now make a page per line and add the histograms & rates to the page 
	std::string pagename;
	std::vector<std::string > pagenames;
	pagenames.push_back("1_Odin-L0-Hlt1");
	pagenames.push_back("2_Hlt1alley-line");
	pagenames.push_back("3_Time&Memory");
	std::vector<int> nbofhistosonpages;
	nbofhistosonpages.push_back(3);
	nbofhistosonpages.push_back(3);
	nbofhistosonpages.push_back(4);
	std::string alley;
	std::map <std::string, std::vector<std::string> >::const_iterator j;
 
	for (j=lines.begin(); j!=lines.end();j++) {
           for (int ia=0;ia<(int)m_DMalleys.size();ia++) {
	      std::string::size_type found_alley=j->first.find(m_DMalleysearchstring[ia],0);
	      if (found_alley !=std::string::npos) {
	         alley=m_DMalleys[ia];
		 break;
	      }	 
	   }
	   //j->first contains the name of the line
	   //we should only have HltGlobalMonitor
	   for (int ip=0;ip<(int)pagenames.size();ip++) {
              pagename=m_hltdbfolder+"/HLT/"+m_nickname+"/DataManager/"+pagenames[ip];
	      std::string hlt1line=j->first;
	     // msg << MSG::INFO<< "Page name " <<pagename << endreq; 
	      OnlineHistPage* evsize = HistDB->getPage(pagename);
	      evsize->removeAllHistograms();
              int nbofhistosonpage = nbofhistosonpages[ip];

              //set up the coordinates of the histograms on the page	   
              double xmargin = 0.03;
              double ymargin = 0.03;
              int nx = (int) ceil(sqrt(nbofhistosonpage));
              int ny = nx;

              if (nx <= 0) { nx = 1; }
              if (ny <= 0) { ny = 1; }
              double x1, y1, x2, y2;
              double dx, dy;

              dy = 1/double(ny);
              dx = 1/double(nx);
         	   	 	      	     
	      int zy=0;
	      int zx=0;
	      std::string doc="";
	      std::vector<std::string>::const_iterator k;
	      for (k=j->second.begin(); k!=j->second.end(); k++) {	    

	         std::string tname=*k;
		 std::string::size_type found=NULL;
		 int fx=1;
	         if (ip==0) {
		       doc= "top left: number of accepted events by ODIN per ODIN trigger type.\ntop right: number of accepted events by L0 per L0 trigger alley.\nbottom: number of accepted events by Hlt1 per Hlt1 trigger alley.";
		       found=tname.find("ODIN trigger type",0);
		       if (found ==std::string::npos) {
		          found=tname.find("L0 channel",0);
		          if (found ==std::string::npos) {
		             found=tname.find("Hlt1 Alleys",0);
		             if (found ==std::string::npos) {
			     continue;
			     }
			     else {zx=0;zy=1;fx=2;}
			  }
			  else {zx=1;zy=0;}
		       }
		       else {zx=0;zy=0;}	    
		  }
		  if (ip==1) {
		       found=tname.find("HltLines Inclusive",0);
		       doc="top-left: Number of accepted events in Hlt1 per Hlt1 alley.(same plot as on bottom previous page)\ntop-right: Correlation matrix of accepted events in Hlt1 per Hlt1 line (horizontal and  vertical axis show the different Hlt1 lines, the color represents the number of events accepted by  this line).\nbottom: Number of accepted events by Hlt1 per Hlt1 line (zoom in)";
		       if (found ==std::string::npos) {
		          found=tname.find("HltLines Correlations",0);
		          if (found ==std::string::npos) {
		            found=tname.find("# positive HltLines",0);
		            if (found ==std::string::npos) {
			    continue;
			    }
			    else {zx=0;zy=0;}
			  }
			  else  {zx=1;zy=0;}
		       } 
		       else {zx=0;zy=1;fx=2;}      
		   }
		   if (ip==2) {
		       found=tname.find("Virtual memory",0);
		       if (found==std::string::npos) {
		          found=tname.find("Virtual memory",0);
		          if (found ==std::string::npos) {
		             found=tname.find("average time per event",0);
		             if (found ==std::string::npos) {
			         found=tname.find("time per event",0);
		                 if (found ==std::string::npos) {
			            continue;
			         }
				 else {zx=1;zy=1;}				 
			     }
			     else {zx=0;zy=1;}
			  }
			  else  {zx=1;zy=0;}
		       } 
		       else  {zx=0;zy=0;}	        
		    }
		      
	            std::string histogramname="GauchoJob/"+hlt1line+"/"+tname;
	            OnlineHistogram* h=HistDB->getHistogram(histogramname);
	            //this code is to place the histogram in the right place on the page
	            ok = true;	          
		    x1 = zx*dx + xmargin;
		    x2 = x1 + fx*dx - xmargin;       
	            y2 = 1 - zy*dy - ymargin; 
	            y1 = y2 - dy + ymargin;
	            //add the histogram to the page
	            ok &= (NULL !=evsize->addHistogram(h,x1,y1,x2,y2));
                    HistogramDisplayOptions(h);
	         }	
	         if (ok) {
		    //set the comments
		    evsize->setDoc(doc);
		    evsize->save(); 
		 }
	         else abort();  	
	   }
       }	      

     //  msg << MSG::INFO << "doEFF: committing changes to Hist DB" << endreq;
       HistDB->commit();
     }
     delete HistDB;
}


void InsertDB::doEFFHltExperts() {
   //doEFF analyzes histograms published by Moore to the top level Adder
   MsgStream msg(msgSvc(), name());
   bool ok=true;

   DimClient::setDnsNode(m_dimclientdns.c_str());  
   DimBrowser dbr;
   
   msg << MSG::INFO<< "Connecting to Online HistDB" <<endreq; 

   std::string password="HISTEGGIA194";
   OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
   if (HistDB==0) {
       msg << MSG::INFO<< "Could not connect to Histogram database." << endreq;
   }
   else { 
       HistDB->setDebug(3);
       //lines contains the lines found. a line is a vector with histogram names inside it.
       std::map <std::string, std::vector<std::string> > lines;
       std::vector<std::string> pages;
       std::vector<std::string> histograms;
       
       hSvcnames="MonH1D/"+m_partitionname+m_addername+"/*";
       hSvcnames2d="MonH2D/"+m_partitionname+m_addername+"/*";
       pSvcnames="MonP1/"+m_partitionname+m_addername+"/*";
       rSvcnames=m_partitionname+m_addername+"/*monRate/*-flag";
   
       dbr.getServices(hSvcnames.c_str());
       
       // find the histograms and rates per line and declare them to the histdb
       // 1d histograms
       while( (type = dbr.getNextService(service, Dimformat)) )
       { 
	   hSvcname=service;
	   std::string prefix = "MonH1D/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	   std::string rest=hSvcname.substr(prefix.length());
	   std::string::size_type first_slash=rest.find("/",0);
	   if (first_slash != std::string::npos ) {
	     std::string line = rest.substr(0,first_slash);
	     std::string histoname = rest.substr(first_slash+1);
	     histograms.clear();
	     if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
             }
             histograms.push_back(histoname);
	     lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(hSvcname);
	} 

        //2d histograms
        dbr.getServices(hSvcnames2d.c_str());
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	    hSvcname2d=service;
	    std::string prefix = "MonH2D/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	    std::string rest=hSvcname2d.substr(prefix.length());
	    std::string::size_type first_slash=rest.find("/",0);
	    if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
            } 
	    HistDB->declareHistByServiceName(hSvcname2d);
	}	
	dbr.getServices(pSvcnames.c_str());
        //profile histograms
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	    pSvcname=service;
	    std::string prefix = "MonP1/"+m_partitionname+m_addername+"/"+m_taskname+"/"; 
	      std::string rest=pSvcname.substr(prefix.length());
	      std::string::size_type first_slash=rest.find("/",0);
	      if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(pSvcname);
         }
	 
/*       disable rates for the moment
         until Giacomo's API has support for rates
	 msg << MSG::INFO<< "Looking for rate service: " << rSvcnames.c_str() << " using DNS: "<< m_dimclientdns<< endreq;     
         dbr.getServices(rSvcnames.c_str());
  
         while( (type = dbr.getNextService(service, Dimformat)) )
         { 
	      rSvcname=service;   	   
	      std::string prefix = m_partitionname+m_addername+"/"+m_taskname+"/MonitorSvc/monRate/"; 
	      std::string rest=rSvcname.substr(prefix.length());
	      std::string::size_type first_slash=rest.find("/",0);
	      if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(rSvcname);
         }
	 */
	 
	//now make a page per line and add the histograms & rates to the page 
	std::string pagename;
	std::string alley;
	std::map <std::string, std::vector<std::string> >::const_iterator j;
 
	for (j=lines.begin(); j!=lines.end();j++) {
           for (int ia=0;ia<(int)m_alleys.size();ia++) {
	      std::string::size_type found_alley=j->first.find(m_alleysearchstring[ia],0);
	      if (found_alley !=std::string::npos) {
	         alley=m_alleys[ia];
		 break;
	      }	 
	   }
	   //j->first contains the name of the line
           pagename=m_hltdbfolder+"/HLT/"+m_nickname+"/HltExperts/"+alley+"/"+j->first;
	   std::string hlt1line=j->first;
	   OnlineHistPage* evsize = HistDB->getPage(pagename);
	   evsize->removeAllHistograms();
           int nbofhistosonpage = j->second.size();

           //set up the coordinates of the histograms on the page	   
           double xmargin = 0.03;
           double ymargin = 0.03;
           int nx = (int) ceil(sqrt(nbofhistosonpage));
           int ny = nx;

           if (nx <= 0) { nx = 1; }
           if (ny <= 0) { ny = 1; }
           double x1, y1, x2, y2;
           double dx, dy;

           dy = 1/double(ny);
           dx = 1/double(nx);
         	   	 	      	     
	   int zy=0;
	   int zx=0;
	   std::vector<std::string>::const_iterator k;
	   for (k=j->second.begin(); k!=j->second.end(); k++) {	    

	      std::string tname=*k;
	      std::string histogramname="GauchoJob/"+hlt1line+"/"+tname;
	      OnlineHistogram* h=HistDB->getHistogram(histogramname);
	      //this code is to place the histogram in the right place on the page
	      ok = true;	          
	      if (zx < nx ) {
                    x1 = zx*dx + xmargin;
                    x2 = x1 + dx - xmargin;
		    zx ++;
	      }
	      else {
	         zy++;
		 zx=0;
		 x1 = zx*dx + xmargin;
                 x2 = x1 + dx - xmargin;
		 zx ++;
	      }	      
	      y2 = 1 - zy*dy - ymargin; 
	      y1 = y2 - dy + ymargin;
	      //add the histogram to the page
	      ok &= (NULL !=evsize->addHistogram(h,x1,y1,x2,y2));
              HistogramDisplayOptions(h);
	   }	
	   if (ok) evsize->save();
	   else abort();
	}    	

     //  msg << MSG::INFO << "doEFFHLTExperts: committing changes to Hist DB" << endreq;
       HistDB->commit();
     }
     delete HistDB;
}


void InsertDB::doMF(std::string monitoringtask) {
   //analyse monitoring tasks given in the joboptions
   MsgStream msg(msgSvc(), name());
   bool ok=true;
 
   DimClient::setDnsNode(m_dimclientdns.c_str());  
   DimBrowser dbr;
   
   msg << MSG::INFO<< "Connecting to Online HistDB" <<endreq; 

   std::string password="HISTEGGIA194";
   OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
   if (HistDB==0) {
       msg << MSG::INFO<< "Could not connect to Histogram database." << endreq;
   }
   else { 
       HistDB->setDebug(3);
       std::map <std::string, std::vector<std::string> > lines;
       std::vector<std::string> pages;
       std::vector<std::string> histograms;
       
       hSvcnames="MonH1D/"+m_partitionname+"_*_"+monitoringtask+"_00/*";
       hSvcnames2d="MonH2D/"+m_partitionname+"_*_"+monitoringtask+"_00/*";
       pSvcnames="MonP1*/"+m_partitionname+"_*_"+monitoringtask+"_00/*";
       rSvcnames=m_partitionname+"_*_"+monitoringtask+"_00/*monRate/*-flag";
   
       dbr.getServices(hSvcnames.c_str());       
       // find the histograms and rates per line and declare them to the histdb
       //1d histograms
       while( (type = dbr.getNextService(service, Dimformat)) )
       { 
       
 	   hSvcname=service;
	   std::string::size_type first_us=hSvcname.find("_",0);
	   std::string::size_type second_us=hSvcname.find("_",first_us+1);
	   m_nodename=hSvcname.substr(first_us+1,second_us-first_us-1);
	   std::string prefix = "MonH1D/"+m_partitionname+"_"+m_nodename+"_"+monitoringtask+"_00/"; 
	   std::string rest=hSvcname.substr(prefix.length()); 
	   std::string::size_type first_slash=rest.find("/",0);
	   if (first_slash != std::string::npos ) {
	     std::string line = rest.substr(0,first_slash);
	     std::string histoname = rest.substr(first_slash+1);
	     histograms.clear();
	     if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
             }
             histograms.push_back(histoname);
	     lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(hSvcname);
	} 
        //2d histograms
        dbr.getServices(hSvcnames2d.c_str());
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	   hSvcname2d=service;
	   std::string::size_type first_us=hSvcname2d.find("_",0);
	   std::string::size_type second_us=hSvcname2d.find("_",first_us+1);
	   m_nodename=hSvcname.substr(first_us+1,second_us-first_us-1);
	   std::string prefix = "MonH2D/"+m_partitionname+"_"+m_nodename+"_"+monitoringtask+"_00/"; 
	   std::string rest=hSvcname.substr(prefix.length());
	   std::string::size_type first_slash=rest.find("/",0);
	   if (first_slash != std::string::npos ) {
	     std::string line = rest.substr(0,first_slash);
	     std::string histoname = rest.substr(first_slash+1);
	     histograms.clear();
	     if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
             }
             histograms.push_back(histoname);
	     lines[line]=histograms;		       	 	      	     
           }
	    HistDB->declareHistByServiceName(hSvcname2d);
	}
	//profile historams	
	dbr.getServices(pSvcnames.c_str());
        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	   pSvcname=service;
	   std::string::size_type first_us=pSvcname.find("_",0);
	   std::string::size_type second_us=pSvcname.find("_",first_us+1);
	   m_nodename=hSvcname.substr(first_us+1,second_us-first_us-1);
	   std::string prefix = "MonP1/"+m_partitionname+"_"+m_nodename+"_"+monitoringtask+"_00/"; 
	   std::string rest=hSvcname.substr(prefix.length());
	   std::string::size_type first_slash=rest.find("/",0);	      
	   if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	         //map entry exists; retrieve it
	         histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(pSvcname);
         }
	 
  /*     disable rates for the moment; the following code needs to be fixed, see code above 
         dbr.getServices(rSvcnames.c_str());
  

         while( (type = dbr.getNextService(service, Dimformat)) )
         { 
	      rSvcname=service;
	      std::string prefix = m_partitionname+m_addername+"/"+m_taskname+"/MonitorSvc/monRate/"; 
	      std::string rest=rSvcname.substr(prefix.length());
	      std::string::size_type first_slash=rest.find("/",0);
	      if (first_slash != std::string::npos ) {
	      std::string line = rest.substr(0,first_slash);
	      std::string histoname = rest.substr(first_slash+1);
	      msg << MSG::INFO << "Found service: " << rSvcname << " line " <<	     line << " histogramname " << histoname << endreq;
	      histograms.clear();
	      if (lines.find(line)!= lines.end()) {
	        //map entry exists; retrieve it
	        histograms=lines[line];
              }
              histograms.push_back(histoname);
	      lines[line]=histograms;		       	 	      	     
           }
	   HistDB->declareHistByServiceName(rSvcname);
         }
	 */
	//now make a page per line and add the histograms & rates to the page 
	std::string pagename;
	std::map <std::string, std::vector<std::string> >::const_iterator j;
        std::vector<OnlineHistogram*> list;
	int nh=HistDB->getHistogramsByTask(monitoringtask,&list);
	
	for (j=lines.begin(); j!=lines.end();j++) {
	   std::string subfolder;
	   bool subfolderfound=false;
	   for (int si=0;si<(int)m_subfolders[mtaskcount].size();si++) {
	      //need to organize the histograms in subfolders as there are a priori no alleys
	      std::vector<std::string> histogramsonthispage;
	      std::vector<std::string>::const_iterator f;
	      for (f=j->second.begin(); f!=j->second.end(); f++) { 
	         std::string fname=*f;
	         std::string::size_type folderfound=fname.find(m_subfolders[mtaskcount][si],0);
	         if (folderfound !=std::string::npos) {
		   subfolder=m_subfolders[mtaskcount][si] ;
		   subfolderfound=true;
		   histogramsonthispage.push_back(fname);
		 }  
	      }	  
              pagename=m_hltdbfolder+"/"+monitoringtask+"/"+m_nickname+ "/" +j->first + "/" + subfolder  ;
	      std::string hlt1line=j->first;
	      OnlineHistPage* evsize = HistDB->getPage(pagename);
	      evsize->removeAllHistograms();
              int nbofhistosonpage = histogramsonthispage.size();
	   
              double xmargin = 0.03;
              double ymargin = 0.03;
              int nx = (int) ceil(sqrt(nbofhistosonpage));
              int ny = nx;

              if (nx <= 0) { nx = 1; }
              if (ny <= 0) { ny = 1; }
              double x1, y1, x2, y2;
              double dx, dy;

              dy = 1/double(ny);
              dx = 1/double(nx);
         	   	 	      	     
	      int zy=0;
	      int zx=0;
	      std::vector<std::string>::const_iterator k;
	      int cnt=0;
	      for (k=histogramsonthispage.begin(); k!=histogramsonthispage.end(); k++) {	  
	         cnt++;	     
	         std::string tname=*k;
	         OnlineHistogram* h=NULL;
	         bool hfound=false;

	         for (int kn=0;kn<nh;kn++) {
	            std::string::size_type histofound=list[kn]->identifier().find(tname,0);
		    if (histofound !=std::string::npos) {
		       h=list[kn];
		       hfound=true;
		       break;
	            } 
		    if (hfound) break;
	         }

	         //this code is to place the histogram in the right place on the page
	         ok = true;	          
	         if (zx < nx ) {
                    x1 = zx*dx + xmargin;
                    x2 = x1 + dx - xmargin;
		    zx ++;
	         }
	         else {
	            zy++;
		    zx=0;
		    x1 = zx*dx + xmargin;
                    x2 = x1 + dx - xmargin;
		    zx ++;
	         }	      
	         y2 = 1 - zy*dy - ymargin; 
	         y1 = y2 - dy + ymargin;
	         ok &= (NULL !=evsize->addHistogram(h,x1,y1,x2,y2));
		 HistogramDisplayOptions(h);
	      }	
	      if (ok) evsize->save();
	      else abort();
	   }
	}    	

     //  msg << MSG::INFO << "doMF: committing changes to Hist DB" << endreq;
       HistDB->commit();
     }
     delete HistDB; 
}


void InsertDB::HistogramDisplayOptions(OnlineHistogram* h) {
   //set stats box on for all histos
   int option;
   std::string optionsx;
   std::string optionsy;
   option=1;
   h->setDisplayOption("STATS",&option);
   
   //errors
   std::string::size_type found=h->identifier().find("error",0);
   if (found !=std::string::npos) {
      optionsx="error type"; h->setDisplayOption("LABEL_X", &optionsx);   
   } 
   
   //rejected events
   found=h->identifier().find("log",0);
   if (found !=std::string::npos) {
      optionsx="algo"; h->setDisplayOption("LABEL_X", &optionsx);  
      optionsy="# rejected events"; h->setDisplayOption("LABEL_Y", &optionsy);  
   }    
   
   //timing
   found=h->identifier().find("walltime",0);
   if (found !=std::string::npos) {
      optionsx="time (log ms)"; h->setDisplayOption("LABEL_X", &optionsx);  
      optionsy="# events"; h->setDisplayOption("LABEL_Y", &optionsy);  
   }    
   
   h->saveHistoSetDisplayOptions();
   
}
