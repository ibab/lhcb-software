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
#include "dis.hxx"
#include "OnlineHistDB/OnlineHistDB.h"

#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

// for adder 
#include "DimInfoTitle.h"
#include "Adder.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(Adder)


// Constructor
//------------------------------------------------------------------------------
Adder::Adder(const std::string& name, ISvcLocator* ploc)
  : Algorithm(name, ploc), m_nodeNames(), m_histogramName(), m_algorithmName(), m_taskName(),
  m_refreshTime(10),m_dimClientDns(),m_serverName("hlt1101"){
  declareProperty("nodenames",m_nodeNames);
  declareProperty("histogramname",m_histogramName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("taskname",m_taskName);
  declareProperty("refreshtime",m_refreshTime);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("servername",m_serverName);
  }

//------------------------------------------------------------------------------
StatusCode Adder::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); 
  // must be executed first
  // the initialize method looks for and subscribes to the DimServices corresponding to
  // the histograms listed in the joboptions
  // if no joboptions given, it looks in the histogram db
  // then it starts the Dim Timer which does the addition when
  // the DimServices change
  MsgStream msg(msgSvc(), name());
  
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to initialize Algorithm." << endreq;
    return sc;
  }

  sc=findDimServices();
  if( !sc.isSuccess() )   {
    msg << MSG::FATAL << "Unable to find Dim Services." << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::execute() {
  //------------------------------------------------------------------------------  
  MsgStream         msg( msgSvc(), name() );
  //do nothing until a histogram gets updated	
  if (m_histosFound>0) { dim_wait(); }
  else { 
    lib_rtl_sleep(20000);
    StatusCode sc=findDimServices();
    if( !sc.isSuccess() )   {
      msg << MSG::FATAL << "Unable to find Dim Services." << endreq;
    return sc;
    }
  }  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  //here we need to delete the objects in m_infoHistos
  if (m_histosFound>0) {
     for (int i=0;i<(int)m_infoHistos.size();i++) {
       for (int j=0; j<(int)m_infoHistos[i].size();j++)  {
         delete m_infoHistos[i][j];
       }   
     }
     delete tim;
  }     
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode Adder::findDimServices() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "Looking for Dim services..." << endreq;
  char *service; 
  char *format;
  std::string servicestr;
  int dimension;
  int type;
  //these tmph vectors have the index of the histogram name and contain the dim services found for that histogram
  std::vector<std::string> tmphSvcNames;	    
  std::vector<std::string> tmphSvcNames2d;	
  std::vector<std::string> tmppSvcNames;
  
  std::string m_hSvcNames;
  std::string m_hSvcNames2d;
  std::string m_pSvcNames;
  std::string commentSvcName;
  std::string adderhSvcNames;
  std::string adderhSvcNames2d;  
  std::string adderpSvcNames;  
  std::string adderCommentSvcNames;  
  std::vector<DimInfoTitle*> commentFoundSvcNames;
  std::string task;
  m_procName = RTL::processName();
  
  m_histosFound=0;
   
  DimClient::setDnsNode(m_dimClientDns.c_str());
  DimBrowser dbr; 
  if ((int)m_histogramName.size()==0) {
    //no histograms given in joboptions
    //get list of histograms from database
    std::string password="histeggia194";
    OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","ora01:1528/HISTOGRAMDB");
    if (HistDB==0) {
      msg << MSG::INFO << "Could not connect to Histogram database." <<  endreq;
    }
    else {
      for (int j=0;j<(int)m_taskName.size();j++) {
        std::string tmptask=m_taskName[j];
	bool notfound=true;
	for (int k=0;k<j;k++) {
	  if ((k!=j)&&(tmptask==m_taskName[k])) {
	    notfound=false;}
          if (!notfound) break;	     	
	}  
	if (notfound) {
          std::vector<OnlineHistogram*> list;
	  //for v3r0	       
          int x=HistDB->getHistogramsByTask(m_taskName[j],&list);
          for (int i=0;i<x;i++) {
	    std::string histname;  
	    histname=list[i]->identifier();	    
	    msg << MSG::DEBUG << "Registered histogram found in DB: " <<histname.c_str() << endreq; 
	    m_histogramName.push_back(histname);
          }
        } 
      }
      delete HistDB;   
    }   
  }

  std::string svcName;
  for (int l=0;l<(int)m_nodeNames.size();l++) {
    //l loops over the nodes
    svcName="H*D/"+m_nodeNames[l]+"*";	
    dbr.getServices(svcName.c_str());
    msg << MSG::DEBUG << "looking for services " << svcName << endreq;
    while( (type = dbr.getNextService(service, format)) ){               
      servicestr=service; 
      msg << MSG::DEBUG << "handling service " << servicestr << endreq;
      std::string::size_type loc=servicestr.find(m_serverName+"_Adder_",0);
      if (loc == std::string::npos ) { 
        //avoid adding the results of the adder to the sum
        for (int i=0;i<(int)m_histogramName.size();i++) {
	  //loop over histogram names to find all services belonging to it		          	 
	  std::string::size_type found=m_histogramName[i].find("*",0);
	  msg << MSG::DEBUG << "Looking for i "<<i<<" histogram " << m_histogramName[i] << endreq;
	  if (found != std::string::npos ) {
            // wildcard found, need to find the individual histogram names		 
	    std::string::size_type first=servicestr.find("/");  
	    if (first != std::string::npos) {
	      std::string::size_type first_us=servicestr.find("_",first+1);
	      if (first_us != std::string::npos) {
	        std::string::size_type second_us=servicestr.find("_",first_us+1);
	        task=servicestr.substr(first_us+1,second_us-first_us-1);
	        if ((task==m_taskName[i])||(task=="Adder")) {
	          //valid taskname found
	          std::string::size_type second=servicestr.find("/",first+1); 
		  if (second > first) {
		    //second slash found
		    std::string::size_type third=servicestr.find("/",second+1);
		    if (third > second) {
		      std::string algo=servicestr.substr(second+1,third-second-1);
		      if (algo!=m_algorithmName[i]) {
		        //look a slash further
		        std::string::size_type fourth=servicestr.find("/",third+1);
		        if (fourth > third) {
		          algo=servicestr.substr(third+1,fourth-third-1);
		          //set third to fourth so same code works in both cases
		          third=fourth;
		        }   
	              }   
	              if (algo==m_algorithmName[i]) {
		        //valid algorithm found, now check if histogramname fragment without wildcard is found
		        std::string histo=servicestr.substr(third+1);
		        std::string::size_type histofound=histo.find(m_histogramName[i].substr(0,found));
		        msg << MSG::DEBUG << "Looking for " << m_histogramName[i].substr(0,found)<< " in " << histo << " histofound " << histofound<< endreq;	
		        if (histofound !=std::string::npos) {
			  //histogram found, save it 
			  msg << MSG::DEBUG << "found histo " << histo << endreq;	       
	                  m_histogramFoundName.push_back(histo);
			  m_histosFound++;
	                  //see if its unique
	                  bool histnotexists=true;
	                  if ((int)m_histogramUniqueName.size()>0) {
	                    for (int k=0;k<=(int)m_histogramUniqueName.size()-1;k++) {
	                      if (histo==m_histogramUniqueName[k]) {
	                        histnotexists=false;
	                        break;
	                      }
	                    }
	                  }   
	                  if (histnotexists) {
			    m_histogramUniqueName.push_back(histo);	   
		            m_algorithmFoundName.push_back(m_algorithmName[i]);
		            m_taskFoundName.push_back(m_taskName[i]); 
			    m_svcFoundName.push_back(servicestr);	         
			  }
			  else {
			    //histogrammname doesn't match, reiterate
			    continue;				      
			  }
		        }
		        else {
			  //algorithmname doesn't match, reiterate
			  continue;
		        }
		      }
		      else {
		        //no third slash found in service name
		        msg << MSG::DEBUG << "No third slash found in service " << servicestr << ". Ignoring." << endreq;
		        break;								
		      }     
		    }
		    else {
		      //no second slash found in service name
		      msg << MSG::DEBUG << "No second slash found in service " << servicestr << ". Ignoring." << endreq;
		      break;
		    }
		  }
		  else {
		    //taskname doesn't match, reiterate
		    continue;			  
		  }
	        }
	        else {
		  //underscore of UTGID not found
		  //not a histogram service
		  msg << MSG::DEBUG << "No underscore found in UTGID  in service " << servicestr << ". Ignoring." << endreq;
		  break;
	        }
	      }			  
	    }
	    else {
	      msg << MSG::DEBUG << "No wildcard found " << m_histogramName[i] << endreq;		 
              m_histogramFoundName.push_back(m_histogramName[i]);
              m_histogramUniqueName.push_back(m_histogramName[i]);
	      m_algorithmFoundName.push_back(m_algorithmName[i]);
	      m_taskFoundName.push_back(m_taskName[i]);	  
	      m_svcFoundName.push_back(servicestr);          
	    } 	
	  }
        } //loop over histogramnames done (i)
      } 
     }   
   } //end loop over l (nodes)
  //now we have the services and the histogram names, we need to group the services per histogram

  if (m_histosFound>0) {
    msg << MSG::DEBUG << "Looking for " << m_histosFound << " histograms." << endreq;
    //msg << MSG::DEBUG << "Looking for " << m_histogramFoundName.size() << " histograms:" << endreq;
    for (int k=0;k<=(int)m_histogramUniqueName.size()-1;k++) {
      msg << MSG::DEBUG << "Histogram " << k << " " << m_histogramUniqueName[k] << endreq;
      m_iCount=0;
      m_iCount2d=0;
      m_iCountp=0;
      tmphSvcNames.clear();
      tmphSvcNames2d.clear();
      tmppSvcNames.clear();
      for (int i=0;i<=(int)m_histogramFoundName.size()-1;i++) {
        if (m_histogramFoundName[i]==m_histogramUniqueName[k]) {
          if (m_svcFoundName[i].substr(1,1)=="1") {
            dimension=1;
          }
          else {	  
	    if (m_svcFoundName[i].substr(1,1)=="2") {
	      dimension=2;
	    }
	    else dimension=3;
          } 
          switch (dimension) {
	    case 1:
	      tmphSvcNames.push_back(m_svcFoundName[i]);
	      m_iCount++;
	      //    msg << MSG::DEBUG << "Pushing back " << m_svcFoundName[i] << " m_iCount " << m_iCount << endreq; 
	      break;
	    case 2:
	      tmphSvcNames2d.push_back(m_svcFoundName[i]);
	      m_iCount2d++;
	      //   	    msg << MSG::DEBUG << "Pushing back " << m_svcFoundName[i] << " m_iCount2d " << m_iCount2d << endreq; 
	      break;
	    case 3:
	      tmppSvcNames.push_back(m_svcFoundName[i]);
	      m_iCountp++;
	      //  	    msg << MSG::DEBUG << "Pushing back " << m_svcFoundName[i] << " m_iCountp " << m_iCountp << endreq; 
	      break;
	    default:
	    msg << MSG::DEBUG << "Unknown dimension " << dimension << endreq; 
          }
        }   
      }
      m_hSvcName.push_back(tmphSvcNames);    
      m_hSvcName2d.push_back(tmphSvcNames2d); 
      m_pSvcName.push_back(tmppSvcNames); 	           
      m_nbOf1DHistos.push_back(m_iCount);
      m_nbOf2DHistos.push_back(m_iCount2d);
      m_nbOfPHistos.push_back(m_iCountp);		
    }	   
    //the adder init creates dimservice objects
    //and creates an array of them
    //the adding is triggered by the dim timer
  
    std::string instance;
    std::string instanceNode;
    float nbOfTasks;
    if (m_histogramUniqueName.size()>0) nbOfTasks=(int)m_histogramFoundName.size()/(int)m_histogramUniqueName.size();
    else nbOfTasks=0;

    for (int j=0; j< (int)m_histogramUniqueName.size();j++) {
      //j counts the histograms
      //in this first loop we subscribe to the dimservices
      //do this so the result of the title has time to come back
  
      std::vector<DimInfoHistos*>	tmphInfo;
      std::vector<DimInfoHistos*>	tmphInfo2D;
      std::vector<DimInfoHistos*>	tmppInfo;
      DimInfoHistos * temp;
      DimInfoTitle * ttemp;     
      //update the DNS only once, when all new DimServices have been made
      DimServer::autoStartOff();

      if (m_nbOf1DHistos[j]>0) {
        for (int i=0;i<m_nbOf1DHistos[j];i++) { 
          temp = new DimInfoHistos(m_hSvcName[j][i],m_refreshTime); 
          //  msg << MSG::DEBUG << "Created object: " << m_hSvcName[j][i] << endreq;        
	  tmphInfo.push_back(temp);
	  if (i==0) {
	    //do this first so the result of the title has time to come back
	    instance=m_hSvcName[j][i].substr(m_hSvcName[j][i].find_last_of("_")+1,1);
	    instanceNode=m_hSvcName[j][i].substr(m_hSvcName[j][i].find("/")+1,m_hSvcName[j][i].find("_")-m_hSvcName[j][i].find("/")-1);
	    if (task=="Adder") {
	      commentSvcName=instanceNode+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
	    }
	    else {
	      commentSvcName=instanceNode+"_"+m_taskFoundName[j]+"_"+instance+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
            }
	    //   msg << MSG::DEBUG << "commentSvcName: " <<  commentSvcName << endreq;   
	    ttemp = new DimInfoTitle(commentSvcName);
	    commentFoundSvcNames.push_back(ttemp);
	  }  	      
        }
      }	
      if (m_nbOf2DHistos[j]>0) {
        for (int i=0;i<m_nbOf2DHistos[j];i++) {
          temp = new DimInfoHistos(m_hSvcName2d[j][i],m_refreshTime);        
	  tmphInfo2D.push_back(temp);
          if (i==0) {
	    instance=m_hSvcName2d[j][i].substr(m_hSvcName2d[j][i].find_last_of("_")+1,1);
	    instanceNode=m_hSvcName2d[j][i].substr(m_hSvcName2d[j][i].find("/")+1,m_hSvcName2d[j][i].find("_")-m_hSvcName2d[j][i].find("/")-1);
	    if (task=="Adder") {
	      commentSvcName=instanceNode+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
	    }
	    else {
	      commentSvcName=instanceNode+"_"+m_taskFoundName[j]+"_"+instance+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
            }
	    ttemp = new DimInfoTitle(commentSvcName);
	    commentFoundSvcNames.push_back(ttemp);
	  } 
	  //       msg << MSG::DEBUG << "Created object: " << m_hSvcName2d[j][i] << endreq;   
        }  
      }
      if (m_nbOfPHistos[j]>0) {
        for (int i=0;i<m_nbOfPHistos[j];i++) { 
          temp = new DimInfoHistos(m_pSvcName[j][i],m_refreshTime);         
	  tmppInfo.push_back(temp);
          if (i==0) {
	    instance=m_pSvcName[j][i].substr(m_pSvcName[j][i].find_last_of("_")+1,1);
	    instanceNode=m_pSvcName[j][i].substr(m_pSvcName[j][i].find("/")+1,m_pSvcName[j][i].find("_")-m_pSvcName[j][i].find("/")-1);
	    if (task=="Adder") {
	      commentSvcName=instanceNode+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
	    }
	    else {
	      commentSvcName=instanceNode+"_"+m_taskFoundName[j]+"_"+instance+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";
            }
	    ttemp = new DimInfoTitle(commentSvcName);
	    commentFoundSvcNames.push_back(ttemp);
	  }
	  //     msg << MSG::DEBUG << "Created object: " << m_pSvcName[j][i] << endreq;  
        }    
      }
      //update the array of dim buffers  
      m_hInfo.push_back(tmphInfo);  
      m_hInfo2D.push_back(tmphInfo2D);  
      m_pInfo.push_back(tmppInfo); 
    } //loop over j   

    for (int j=0; j< (int)m_histogramUniqueName.size();j++) {
      //j counts the histograms 
      //in this second loop we set up the adder services for the results
      adderhSvcNames="H1D/"+m_serverName+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j];
      adderhSvcNames2d="H2D/"+m_serverName+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j];
      adderpSvcNames="HPD/"+m_serverName+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j];
      adderCommentSvcNames=m_serverName+"_Adder_1/"+m_taskFoundName[j]+"/"+m_algorithmFoundName[j]+"/"+m_histogramUniqueName[j]+"/gauchocomment";  
      char* temptitle=0;
      temptitle=commentFoundSvcNames[j]->getTitle();	
      if (m_nbOf1DHistos[j]>0) {    
        m_hInfo[j][0]->declareTitleInfo(adderCommentSvcNames,temptitle);  
        m_hInfo[j][0]->declareInfo(adderhSvcNames);
        //initialise the sum
        m_hInfo[j][0]->add(m_hInfo[j][0]);
      }
     
      if (m_nbOf2DHistos[j]>0) {    
        m_hInfo2D[j][0]->declareTitleInfo(adderCommentSvcNames,temptitle);  
        m_hInfo2D[j][0]->declareInfo(adderhSvcNames2d);
        //initialise the sum
        m_hInfo2D[j][0]->add2d(m_hInfo2D[j][0]);
      }
      if (m_nbOfPHistos[j]>0) {    
        m_pInfo[j][0]->declareTitleInfo(adderCommentSvcNames,temptitle); 
        m_pInfo[j][0]->declareInfo(adderpSvcNames);
        //initialise the sum
        m_pInfo[j][0]->addp(m_pInfo[j][0]);
      }
    } //loop over j
      
    for (int j=0; j<(int)m_histogramUniqueName.size();j++)  {
      if (m_nbOf1DHistos[j]>0) {     
        m_infoHistos.push_back(m_hInfo[j]);
      }
      if (m_nbOf2DHistos[j]>0) {
        m_infoHistos.push_back(m_hInfo2D[j]); 
      }
      if (m_nbOfPHistos[j]>0) {	   	 
        m_infoHistos.push_back(m_pInfo[j]);
      }
      //we can now delete the objects inside commentFoundSvcNames
      delete commentFoundSvcNames[j];
    } 
    DimServer::start(m_procName.c_str());
    msg << MSG::INFO << "Initialization completed. Starting DimTimer to add " << m_infoHistos.size() << " histograms" << endreq;  
    tim= new Tim(m_refreshTime,m_infoHistos);
  }
  else  {
    msg << MSG::INFO << "No histograms found. Will try again in 20 secs. " << endreq;  
  }   
  return StatusCode::SUCCESS;
}
