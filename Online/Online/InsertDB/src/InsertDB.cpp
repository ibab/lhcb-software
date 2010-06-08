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
#include "GaudiKernel/Timing.h"

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
  declareProperty("MFOnly",m_mfonly=false);
  declareProperty("EFFOnly",m_effonly=false);


  declareProperty("doGlobalPages",m_doGlobalPages=false);
  declareProperty("doLinePages",m_doLinePages=false);
  declareProperty("doAlgorithmPages",m_doAlgorithmPages=false);
  declareProperty("doLumiPages",m_doLumiPages=false);
  declareProperty("doExpertPages",m_doExpertPages=false);

  declareProperty("GlobalPages",m_globalPages);
  declareProperty("GlobalDocu", m_globalDocu );
  declareProperty("LinePages",m_linePages);
  declareProperty("LineDocu", m_lineDocu );

  declareProperty("LumiPages",m_lumiPages);
  declareProperty("LumiDocu", m_lumiDocu );
  declareProperty("LumiLowPages",m_lumiLowPages);
  declareProperty("LumiLowDocu", m_lumiLowDocu );

  declareProperty("hlt1AlgMonName",m_hlt1AlgMonName);
  declareProperty("hlt1AlgMuPrepPages",m_hlt1AlgMuPrepPages);
  declareProperty("hlt1AlgMuPrepDocu",m_hlt1AlgMuPrepDocu);
}   
   
StatusCode InsertDB::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  // return 0; 
  icount=0;
  MsgStream msg(msgSvc(), name());
  info()<< "Initializing."  << endreq; 
  if (m_partitionname=="") m_partitionname="FEST"; 
  if (m_taskname=="") m_taskname="GauchoJob";
  if (m_addername=="") m_addername="_Adder_1";
  if (m_dimclientdns=="") m_dimclientdns="mona08"; 
  mtaskcount=0;
  
  return StatusCode::SUCCESS;
}

StatusCode InsertDB::execute() {

  debug()<<"execute ..."<<endmsg;

  MsgStream msg(msgSvc(), name());
  if (icount==0) {

    bool histosFromEFF=true;

    if(m_doGlobalPages){
      always()<<"***          do the global monitoring            *** "<<endmsg;
      std::string prefix = m_hltdbfolder+"/Shift/";
      fillPages(prefix,m_globalPages, m_globalDocu,histosFromEFF);
    }
    if(m_doLinePages){
      always()<<"***          do the HLT1 Line monitoring         *** "<<endmsg;
      std::string prefix = m_hltdbfolder+"/"+m_nickname;
      fillLinePages(prefix,m_linePages, m_lineDocu);

      //HARD CODE TRHE CORRELATIONS - FOR THE MOMENT
      std::map <std::string, std::vector<std::string> >  corrPage;
      std::vector<std::string> histList;
      histList.push_back("HltGlobalMonitor/Hlt1Alleys Correlations");
      histList.push_back("HltGlobalMonitor/Hlt2Alleys Correlations");
      corrPage.insert(std::pair<std::string, std::vector<std::string> > 
		      ("HLT correlations",histList));
      std::vector<std::string> corrDoc;
      corrDoc.push_back("top: correlations between HLT1 Lines.\nbottom: correlations between HLT2 Lines.");
      prefix = m_hltdbfolder+"/"+m_nickname+"/";
      fillPages(prefix,corrPage, corrDoc,true);
    }
    if(m_doAlgorithmPages){
      always()<<"***          do the HLT1 Algorithm monitoring         *** "<<endmsg;
      //  always()<<"not yet there.."<<endmsg;
      histosFromEFF=false;
      std::string prefix = m_hltdbfolder+"/"+m_nickname+"/HLT1/AlgorithmMonitoring/"+m_hlt1AlgMonName+"/";
      fillPages(prefix,m_hlt1AlgMuPrepPages,m_hlt1AlgMuPrepDocu,histosFromEFF,"HltExpertMon" );
    }
    if(m_doLumiPages){
      always()<<"***          do the Lumi monitoring         *** "<<endmsg;
      //  always()<<"not yet there.."<<endmsg;
      histosFromEFF=false;
      std::string prefix = m_hltdbfolder+"/Random/";
      fillPages(prefix,m_lumiPages,m_lumiDocu,histosFromEFF,"LumiDAQMon" );
      prefix = m_hltdbfolder+"/L0Trigger/";
      fillPages(prefix,m_lumiLowPages,m_lumiLowDocu,histosFromEFF,"LumiLowMon" );
    }

    always()<<"&&&&&&&&&&&&&&&&&&"<<endmsg;


    //     always()<<"(0)"<<endmsg;
    //     doEFF();
    //    always()<<"(a)"<<endmsg;
    //doEFFNew();
    //    always()<<"(b)"<<endmsg;

    //if(0<m_monitoringtasks.size()) doMFNew(m_monitoringtasks[0]);
    //else info()<<"nio moni task set .."<<endmsg;
    //  always()<<"(c)"<<endmsg;

    //    bool m_doEFF = false;
    //bool m_doMF = false;

    // if(m_doEFF)  doEFF();
    //      if(m_doMF) {
    //        for (int i=0;i<(int)m_monitoringtasks.size();i++) {
    // 	 doMF(m_monitoringtasks[i]);
    // 	 mtaskcount++;
    //        }
    //     }
     
    //  if (!m_mfonly) { 
    //           doEFF();
    // 	  //doEFFHltExperts();
    //      }
    //      if (!m_effonly) {	    
    //         for (int i=0;i<(int)m_monitoringtasks.size();i++) {
    //           doMF(m_monitoringtasks[i]);
    //           mtaskcount++;
    //         }  
    //      }
     
  }  
  else {
    info()<< "Done." << endreq;
    exit(0);
  }
  icount++;
  return StatusCode::SUCCESS;
}

StatusCode InsertDB::finalize() {
  return StatusCode::SUCCESS;
} 




	
	
void InsertDB::placeHistograms(OnlineHistPage* page, 
			       std::vector<OnlineHistogram*>& histos,
			       std::string doc )
{
  
  int nbofhistosonpage = histos.size();
  if(0== nbofhistosonpage ) {
    warning()<<"nno histos:  "<<nbofhistosonpage    <<endmsg;
    return;
  }
  if(nbofhistosonpage > 49){
    warning()<<"too many histograms for this page, #histos: "<<nbofhistosonpage 
	     <<" !!! to not place any histograms! "<<endmsg;
    return;
  }

  double xmargin = 0.03;
  double ymargin = 0.03;
  int nx = (int) ceil(sqrt(nbofhistosonpage));
  int ny = nx;

  if (nx <= 0) { nx = 1; }
  if (ny <= 0) { ny = 1; }
  
  debug()<<"createm page of "<<nx<<" by "<<ny<<" histograms"<<endmsg;  
  
  double x1, y1, x2, y2;
  double dx, dy;
	      
  dy = 1/double(ny);
  dx = 1/double(nx);
  
<<<<<<< InsertDB.cpp
  int zy=0;
  int zx=0;
  bool ok=true;
  std::vector<OnlineHistogram*>::iterator itHist;
  for(itHist = histos.begin(); itHist!=histos.end();++itHist){
    
    OnlineHistogram* h=*itHist;
    info()<<"place histogram: "<<h->identifier()<<endmsg;    

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
    
    //    info() << "doMF: adding histogram " << h->hname() 
    //	<< " to page " << pagename << endreq;
    ok &= (NULL !=page->addHistogram(h,x1,y1,x2,y2));
    HistogramDisplayOptions(h);
  }	
  if (ok) {
    page->save();
    
    //the job options parser replaces "\n" with "\\n"
    //-->revert this change
    std::string doc2 = ReplaceString("\\n","\n",doc);
    page->setDoc(doc2);
  }
  else abort();

  return;
=======
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
              int nx = (int) ceil(sqrt(float(nbofhistosonpage)));
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
		 std::string::size_type found=0;
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
>>>>>>> 1.9
}





void InsertDB::fillPages(std::string pageNamePrefix,
			 std::map <std::string, std::vector<std::string> > pages,
			 std::vector<std::string> documentation,
			 bool fromEFF,
			 std::string taskName) 
{

  //doEFF analyzes histograms published by Moore to the top level Adder
  info()<<"start fillPages, pagen location prefix is: "<<pageNamePrefix <<endmsg;

 if( pages.size() != documentation.size() ){
    warning()<<"# pages: "<<pages.size() 
	     <<" and # documentations:  "<<documentation.size()
	     <<" unequal !!!! quit function !"<<endmsg; 
    return;
  }
  
 //  DimClient::setDnsNode(m_dimclientdns.c_str());  
//   DimBrowser dbr;
   
  info()<< "Connecting to Online HistDB" <<endreq; 

  std::string password="HISTEGGIA194";
  OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
  if (HistDB==0) {
    info()<< "Could not connect to Histogram database. Return." << endreq;
    return;
  }
 
  HistDB->setDebug(3);
  //lines contains the lines found. a line is a vector with histogram names inside it.
  std::map <std::string, std::vector<std::string> > lines;
  
 
<<<<<<< InsertDB.cpp
  getHistsFromDB(HistDB, lines, fromEFF, taskName);

  int nPage=0;
  std::map <std::string, std::vector<std::string> >::const_iterator mapIter;
  for (mapIter=pages.begin(); mapIter!=pages.end();mapIter++) {
    
    info()<<"iterate over map, process page: "<<mapIter->first<<endmsg;

    // if("all" == mapIter->first ){
//       info()<<"create 1 page per algorithm containing all histograms"<<endmsg;
//       verbose()<<"total number of algorithms in DB:" <<lines.size() <<endmsg;
   
//       std::map <std::string, std::vector<std::string> >::const_iterator j;
//       for (j=lines.begin(); j!=lines.end();j++) {
// 	std::string page=j->first; 
// 	std::string fullPagename=pageNamePrefix+page;
// 	info()<<"fullPagename "<<fullPagename<<endmsg;
// 	OnlineHistPage* onlinePage = HistDB->getPage(fullPagename);
// 	onlinePage->removeAllHistograms();
// 	std::vector<OnlineHistogram*> onlineHists;
// 	std::vector<std::string>::const_iterator k;
// 	for (k=j->second.begin(); k!=j->second.end(); k++) {
// 	  std::string algorithm = j->first;
// 	  std::string hist = *k;
// 	  std::string fullName = algorithm+"/"+hist;
// 	  std::string histogramname="GauchoJob/"+fullName;
// 	  verbose()<<"histogramname"<<histogramname <<endmsg;
// 	  OnlineHistogram* h=HistDB->getHistogram(histogramname);
// 	  onlineHists.push_back(h);
// 	  debug()<<"onlineHists.size() "<<onlineHists.size() <<endmsg;
// 	}
// 	info() << "created vector of online histos of size: "<<onlineHists.size()<<endreq;
// 	placeHistograms(onlinePage,onlineHists,documentation[0]);
// 	HistDB->commit();
//       }
//       info()<<"loop over pages finished."<<endmsg;
//       delete HistDB; 
//       return;
//     }//end if "all"

    std::string page=mapIter->first;
    std::string fullPagename=pageNamePrefix+page;
    //std::string fullPagename="/Shift/"+page;
    OnlineHistPage* onlinePage = HistDB->getPage(fullPagename);
    onlinePage->removeAllHistograms();

    std::vector<OnlineHistogram*> onlineHists;
    
    std::vector<std::string>::const_iterator histName;
    for (histName=mapIter->second.begin(); 
	 histName!=mapIter->second.end(); histName++) {
	  
      info()<<"now loop the histos, process: "<<*histName <<endmsg;

      verbose()<<"total number of algorithms in DB:" <<lines.size() <<endmsg;

      std::map <std::string, std::vector<std::string> >::const_iterator j;
      for (j=lines.begin(); j!=lines.end();j++) {
	std::string h1=j->first;
	
	verbose()<<"j->first() "<<h1<<endmsg;
	
	std::vector<std::string>::const_iterator k;
	for (k=j->second.begin(); k!=j->second.end(); k++) {
	  std::string h2=*k;
	  verbose()<<"j->second iter:"<<h2<<"..."<<endmsg;
	   
	  std::string algorithm = j->first;
	  std::string hist = *k;

	  std::string fullName = algorithm+"/"+hist;
	  verbose()<<"fullName "<<fullName <<endmsg;
	   
	  //    if( "all" == *histName
	  // 	       && "HltGlobalMonitor" == h1 ) continue;

	  if( fullName == *histName ){
	    debug()<<"*****************"<<endmsg;
	    debug()<<"j->first()"<<h1<<" and j->second() "<<h2<<endmsg;
	    debug()<<"*****************"<<endmsg;
	    
	    //@@JA 
	    std::string histogramname=taskName+"/"+fullName;//h1+"/"+h2;
	    //std::string histogramname="LumiDAQMon/"+fullName;//h1+"/"+h2;

	    debug()<<"histogramname "<<histogramname <<endmsg;

	    OnlineHistogram* h=NULL;
	    h=HistDB->getHistogram(histogramname);
	    if(NULL != h ) onlineHists.push_back(h);
	    debug()<<"onlineHists.size() "<<onlineHists.size() <<endmsg;
	  }//identify correct hist

	}//iterate second
      }//iterate first
    }//end loop histName
    
    info() << "created vector of online histos of size: "<<onlineHists.size()<<endreq;
	  
    always()<<std::string(documentation[nPage].c_str())<<endmsg;


      std::string docc = " ----  Histograms are expected to be empty in Pass through mode ----\ntop left: number of accepted events by ODIN per ODIN trigger type.\ntop right: number of accepted events by L0 per L0 trigger alley.\nbottom-left: number of accepted events by Hlt1 per Hlt1 trigger alley.\nbottom-right: number of accepted events by Hlt2 per Hlt2 trigger alley.";


    
    placeHistograms(onlinePage,onlineHists,std::string(documentation[nPage]));
    //placeHistograms(onlinePage,onlineHists,docc);
    nPage++;  
    
    //@ja uncommnet when happy
    info() << "doMF: committing changes to Hist DB" << endreq;
    HistDB->commit();
    // 	}
    
    
  }//end loop pages
  info()<<"loop over pages finished."<<endmsg;

  delete HistDB; 
  
  
  return;
=======
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
           int nx = (int) ceil(sqrt(float(nbofhistosonpage)));
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
>>>>>>> 1.9
}


void InsertDB::fillLinePages(std::string pageNamePrefix,
			     std::map <std::string, std::vector<std::string> > pages,
			     std::vector<std::string> documentation ) 
{

  info()<<"start fillLinePages, pagen location prefix is: "<<pageNamePrefix <<endmsg;


  info()<< "Connecting to Online HistDB" <<endreq; 

  std::string password="HISTEGGIA194";
  OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","HISTDB");
  if (HistDB==0) {
    info()<< "Could not connect to Histogram database. Return." << endreq;
    return;
  }
 
  HistDB->setDebug(3);
  //lines contains the lines found. a line is a vector with histogram names inside it.
  std::map <std::string, std::vector<std::string> > lines;
  getHistsFromDB(HistDB, lines, true );
  
  std::map <std::string, std::vector<std::string> >::const_iterator j;
  for (j=lines.begin(); j!=lines.end();j++) {
    
    debug()<<"now find the alley belonging to "<<std::string(j->first) <<endmsg;
    std::string alley = "Other";
    std::string hlt = "";
    std::map <std::string, std::vector<std::string> >::const_iterator mapIter;
    for (mapIter=pages.begin(); mapIter!=pages.end();mapIter++) {
      std::vector<std::string>::const_iterator histName;
      for (histName=mapIter->second.begin(); 
	   histName!=mapIter->second.end(); histName++) {
	
	//check if HLT1 or HLT2
	if(j->first.find("Hlt1")!=std::string::npos) hlt = "HLT1/LineMonitoring/";
	if(j->first.find("Hlt2")!=std::string::npos) hlt = "HLT2/LineMonitoring/";
	debug()<<"hlt = "<<hlt<<endmsg;
	
	//sort lines in alleys 
	if(j->first.find(*histName)!=std::string::npos){
	  alley = mapIter->first;
	  debug()<<"alley = "<<alley<<endmsg;
	}
	
      }
    }//end loop alley-line map
    debug()<<"alley found! : "<<alley<<endmsg;
    std::string page=j->first;
    hlt=hlt+alley;
    std::string fullPagename=pageNamePrefix+"/"+hlt+"/"+page;
    info()<<"fullPagename "<<fullPagename<<endmsg;
    OnlineHistPage* onlinePage = HistDB->getPage(fullPagename);
    onlinePage->removeAllHistograms();
    std::vector<OnlineHistogram*> onlineHists;
    std::vector<std::string>::const_iterator k;
    for (k=j->second.begin(); k!=j->second.end(); k++) {
      std::string algorithm = j->first;
      std::string hist = *k;
      std::string fullName = algorithm+"/"+hist;
      std::string histogramname="GauchoJob/"+fullName;
      verbose()<<"histogramname"<<histogramname <<endmsg;
      OnlineHistogram* h=HistDB->getHistogram(histogramname);
      
      onlineHists.push_back(h);    
      debug()<<"onlineHists.size() "<<onlineHists.size() <<endmsg;
    }
    info() << "created vector of online histos of size: "<<onlineHists.size()<<endreq;
    
    //sort the vector to make line histos uniform
    std::sort(onlineHists.begin(),onlineHists.end(),HistoComp::isLess2 );
    
    //default documentation only for "time, rejection, eror, candidates"
    std::string doc = "none";
    if(4 == onlineHists.size() ) doc = documentation[0];

    placeHistograms(onlinePage,onlineHists,doc);
    HistDB->commit();
  }
  info()<<"loop over pages finished."<<endmsg;
  delete HistDB; 
  return;
}//end fillLinePages


void InsertDB::HistogramDisplayOptions(OnlineHistogram* h) {
  //set stats box on for all histos
  int option;
  std::string optionsx;
  std::string optionsy;
  std::string drawOpts = "h";  
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
    optionsx="algo"; 
    h->setDisplayOption("LABEL_X", &optionsx);  
    optionsy="# rejected events"; 
    h->setDisplayOption("LABEL_Y", &optionsy);  
  }    
   
  //timing
  found=h->identifier().find("average time",0);
  if (found !=std::string::npos) {
    optionsx="time since run start /min "; 
    h->setDisplayOption("LABEL_X", &optionsx);  
    optionsy="average time per event / ms"; 
    h->setDisplayOption("LABEL_Y", &optionsy);
    drawOpts="e";
  }    
  found=h->identifier().find("Virtual memory",0);
  if (found !=std::string::npos) {
    optionsx="time since run start / min"; 
    h->setDisplayOption("LABEL_X", &optionsx);  
    optionsy="average memory used"; 
    h->setDisplayOption("LABEL_Y", &optionsy);
    drawOpts="e";
  } 
  found=h->identifier().find("time per event",0);
  if (found !=std::string::npos) {
    optionsx="log(HLT CPU time) / log(ms)"; 
    h->setDisplayOption("LABEL_X", &optionsx);  
    //optionsy="average time per event / ms"; 
    //h->setDisplayOption("LABEL_Y", &optionsy);
    drawOpts="e";
  }    
  found=h->identifier().find("Alleys Correlations",0);
  if (found !=std::string::npos) {
    drawOpts="colz";   
  }
  found=h->identifier().find("Hlt CPU time vs raw event size",0);
  if (found !=std::string::npos) {
    optionsx="raw event size"; 
    h->setDisplayOption("LABEL_X", &optionsx);  
    optionsy="log(HLT CPU time) / log(ms)"; 
    h->setDisplayOption("LABEL_Y", &optionsy);
    drawOpts="e";
  }   

  //lumi
  found=h->identifier().find("In_",0);
  if (found !=std::string::npos) {
    int opt=1;
    h->setDisplayOption("LOGY",&opt);
  } 
  //log scale in Hlt1PA lines
  found=h->identifier().find("Hlt1 PA Lines",0);
  if (found !=std::string::npos) {
    int opt=1;
    h->setDisplayOption("LOGY",&opt);
  } 

  h->setDisplayOption("DRAWOPTS",&drawOpts);

  
  h->saveHistoSetDisplayOptions();
   
}


void InsertDB::getHistsFromDB(OnlineHistDB *HistDB,
			      std::map <std::string, std::vector<std::string> >& lines,
			      bool fromEFF,
			      std::string monitoringtask ){


  DimClient::setDnsNode(m_dimclientdns.c_str());  
  DimBrowser dbr;
  
  if(fromEFF){

    info()<<"take histos from EFF..."<<endmsg;
    
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
    info()<<"took histograms from EFF, # taken: "<<lines.size() <<endmsg;
  
  }//end in fromEFF
  else{
    info()<<"take histos from MF ..."<<endmsg;
   //  //@@JA!!!!!!!!! 
//     //@@JA propagate task down to here, not hardcode..
//     info()<<"before: monitoringtask is: "<<monitoringtask<<endmsg;
//     std::string monitoringtask = "HltExpertMon";
//     //    std::string monitoringtask = "LumiDAQMon";

    info()<<"monitoringtask is: "<<monitoringtask<<endmsg;

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
	  //   info()<< "histoname " << histoname << " line " << line << " rest " << rest <<endreq;
	  histograms.clear();
	  if ((lines.find(line)!= lines.end()) && (lines.find(line)->first == line)){
	    //   	info()<< "lines.find(line)->first " << lines.find(line)->first << " line " << line << endreq;
	    //map entry exists; retrieve it, only if they have the same name
	    histograms=lines[line];
	  }
	  histograms.push_back(histoname);
	  // info()<< "putting Histo " << histoname << " in line " << line<< endreq;
	  lines[line]=histograms;		       	 	      	     
	}
	// info()<< "declaring Histo from service " << hSvcname << endreq;
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
	  if ((lines.find(line)!= lines.end()) && (lines.find(line)->first == line)){
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
	  if ((lines.find(line)!= lines.end()) && (lines.find(line)->first == line)){
	    //map entry exists; retrieve it
	    histograms=lines[line];
	  }
	  histograms.push_back(histoname);
	  lines[line]=histograms;		       	 	      	     
	}
	HistDB->declareHistByServiceName(pSvcname);
      }
  
    info()<<"took histograms from MF, # taken: "<<lines.size() <<endmsg;
  }//end from MF

  return;
}


//@JA TODO	 
/*       disable rates for the moment
         until Giacomo's API has support for rates
	 info()<< "Looking for rate service: " << rSvcnames.c_str() << " using DNS: "<< m_dimclientdns<< endreq;     
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
<<<<<<< InsertDB.cpp
*/



std::string InsertDB::ReplaceString(const std::string &stringSearchString, 
				     const std::string &stringReplaceString, 
				     std::string stringStringToReplace)
{
        std::string::size_type pos = stringStringToReplace.find(stringSearchString, 0);
        int intLengthSearch = stringSearchString.length();
        int intLengthReplacment = stringReplaceString.length();
=======
	 */
	//now make a folder per line and add the histograms & rates to pages 
	std::string pagename;
	std::map <std::string, std::vector<std::string> >::const_iterator j;
        std::vector<OnlineHistogram*> list;
	int nh=HistDB->getHistogramsByTask(monitoringtask,&list);
	
	for (j=lines.begin(); j!=lines.end();j++) {
	  // std::string subfolder;
	 //  bool subfolderfound=false;
	  // for (int si=0;si<(int)m_subfolders[mtaskcount].size();si++) {
	      //need to organize the histograms in subfolders as there are a priori no alleys
	    /*  std::vector<std::string> histogramsonthispage;
	      std::vector<std::string>::const_iterator f;
	      for (f=j->second.begin(); f!=j->second.end(); f++) { 
	         std::string fname=*f;
	         std::string::size_type folderfound=fname.find(m_subfolders[mtaskcount][si],0);
	         if (folderfound !=std::string::npos) {
		   subfolder=m_subfolders[mtaskcount][si] ;
		   subfolderfound=true;
		   histogramsonthispage.push_back(fname);
		 }  
	      }	  */
            //  pagename=m_hltdbfolder+"/"+monitoringtask+"/"+m_nickname+ "/" +j->first + "/" + subfolder  ;
	      
	      pagename=m_hltdbfolder+"/"+monitoringtask+"/"+m_nickname+ "/" +j->first + "/" + j->first  ;
	    //  msg << MSG::INFO<< "pagename " << pagename << " j->first " << j->first << endreq;
	      
	      std::string hlt1line=j->first;
	    //  msg << MSG::INFO << " j->first " << j->first << endreq;
	      if (j->first.size()==0) continue;
	      OnlineHistPage* evsize = HistDB->getPage(pagename);
	      evsize->removeAllHistograms();
            //  int nbofhistosonpage = histogramsonthispage.size();
	      int nbofhistosonpage = j->second.size();
              double xmargin = 0.03;
              double ymargin = 0.03;
              int nx = (int) ceil(sqrt(float(nbofhistosonpage)));
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
//	      for (k=histogramsonthispage.begin(); k!=histogramsonthispage.end(); k++) {	  
	      for (k=j->second.begin(); k!=j->second.end(); k++) {	  
	         cnt++;	 
		 //just *k is not enough, need the full identifier  
	         std::string tname=monitoringtask+"/"+j->first+"/"+*k;
		// msg << MSG::INFO << " tname " << tname << endreq;
	         OnlineHistogram* h=NULL;
		 
	         bool hfound=false;

	         for (int kn=0;kn<nh;kn++) {
		   // msg << MSG::INFO << " list[kn]->identifier() " << list[kn]->identifier() << " tname " << tname << endreq;
	            std::string::size_type histofound=list[kn]->identifier().find(tname,0);
		    if (histofound !=std::string::npos) {
		       h=list[kn];
		       hfound=true;
		       break;
	            } 
	         }
		 if (!hfound) continue;
                 
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
		// msg << MSG::INFO << "doMF: adding histogram " << h->hname() << " to page " << pagename << endreq;
	         ok &= (NULL !=evsize->addHistogram(h,x1,y1,x2,y2));
		 HistogramDisplayOptions(h);
	      }	
	      if (ok) {
	         evsize->save();
	      }
              else abort();
	  // }
	}    	

       msg << MSG::INFO << "doMF: committing changes to Hist DB" << endreq;
       HistDB->commit();
     }
     delete HistDB; 
}
>>>>>>> 1.9

        while(std::string::npos != pos)
        {
                stringStringToReplace.replace(pos, intLengthSearch, stringReplaceString);
                pos = stringStringToReplace.find(stringSearchString, pos + intLengthReplacment);
        }

        return stringStringToReplace;
}
