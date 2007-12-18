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
  : Algorithm(name, ploc), m_nodenames (), m_histogramname(),
  m_refreshtime(10),m_dimclientdns(),m_servername("hlt1101"){
  declareProperty("nodenames",m_nodenames);
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

  char *service; 
  char *format;
  std::string servicestr;
  int dimension;
  int type;
  //these tmph vectors have the index of the histogram name and contain the dim services found for that histogram
  std::vector<std::string> tmphSvcnames;	    
  std::vector<std::string> tmphSvcnames2d;	
  std::vector<std::string> tmppSvcnames;
  
  std::string hSvcnames;
  std::string hSvcnames2d;
  std::string pSvcnames;
  std::string commentSvcname;
  std::string adderhSvcnames;
  std::string adderhSvcnames2d;  
  std::string adderpSvcnames;  
  std::string addercommentSvcnames;  
  std::vector<DimInfoTitle*> commentfoundSvcnames;
  std::string task;
  m_procName = RTL::processName();
  
  int histosfound=0;
   
  DimClient::setDnsNode(m_dimclientdns.c_str());
  DimBrowser dbr; 
  if ((int)m_histogramname.size()==0) {
     //no histograms given in joboptions
     //get list of histograms from database
    std::string password="histeggia194";
     OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","ora01:1528/HISTOGRAMDB");
     if (HistDB==0) {
       msg << MSG::INFO << "Could not connect to Histogram database." <<  endreq;
     }
     else {
       for (int j=0;j<(int)m_taskname.size();j++) {
          std::string tmptask=m_taskname[j];
	  bool notfound=true;
	  for (int k=0;k<j;k++) {
	     if ((k!=j)&&(tmptask==m_taskname[k])) {
	        notfound=false;}
          if (!notfound) break;	     	
	  }  
	  if (notfound) {
	     /*
             std::vector<OnlineHistogram*> list;
	     //for v3r0
	     
	     std::vector<string>* ids;
	     std::vector<string>* types;
	     
             int x=HistDB->getHistogramsByTask(m_taskname[j],list,&ids,&types);
             if (x>0) {
                for (int i=0;i<=x-1;i++) {
	           std::string histname;
	           //only works from onlinhistdb v2r4 onwards    
	           //    histname =list[i]->hname();
	           histname=list[i]->identifier();
       	           msg << MSG::DEBUG << "Registered histogram found in DB " <<
	           histname.c_str() << endreq; 
	           m_histogramname.push_back(histname);
                }
             }
	     */		
          } 
	}   
     }   
  }

  std::string Svcname;
  for (int j=0;j<(int)m_nodenames.size();j++) {
  Svcname="H*D/"+m_nodenames[j]+"*";	
  dbr.getServices(Svcname.c_str());
  while( (type = dbr.getNextService(service, format)) )
  {               
	 servicestr=service; 
	// msg << MSG::DEBUG << "handling service " << servicestr << endreq;
	 std::string::size_type loc=servicestr.find(m_servername+"_Adder_",0);
	 if (loc == std::string::npos ) { 
	    //avoid adding the results of the adder to the sum
	    for (int i=0;i<(int)m_histogramname.size();i++) {
	       //loop over histogram names to find all services belonging to it		          	 
		   std::string::size_type found=m_histogramname[i].find("*",0);
		  	// msg << MSG::DEBUG << "Looking for i "<<i<<" histogram " << m_histogramname[i] << endreq;
		   if (found != std::string::npos ) {
              // wildcard found, need to find the individual histogram names		 
		      std::string::size_type first=servicestr.find("/");  
	          if (first != std::string::npos) {
		         std::string::size_type first_us=servicestr.find("_",first+1);
		         if (first_us != std::string::npos) {
		            std::string::size_type second_us=servicestr.find("_",first_us+1);
			        task=servicestr.substr(first_us+1,second_us-first_us-1);
			        if ((task==m_taskname[i])||(task=="Adder")) {
			           //valid taskname found
			           std::string::size_type second=servicestr.find("/",first+1); 
		               if (second > first) {
			              //second slash found
		                  std::string::size_type third=servicestr.find("/",second+1);
		                  if (third > second) {
			                 std::string algo=servicestr.substr(second+1,third-second-1);
					     if (algo!=m_algorithmname[i]) {
					        //look a slash further
						std::string::size_type fourth=servicestr.find("/",third+1);
						if (fourth > third) {
						   algo=servicestr.substr(third+1,fourth-third-1);
						   //set third to fourth so same code works in both cases
						   third=fourth;
						}   
					     }   
				             if (algo==m_algorithmname[i]) {
				                //valid algorithm found, now check if histogramname fragment without wildcard is found
		                                std::string histo=servicestr.substr(third+1);
				                std::string::size_type histofound=histo.find(m_histogramname[i].substr(0,found));
				               // msg << MSG::DEBUG << "Looking for " << m_histogramname[i].substr(0,found)<< " in " << histo << " histofound " << histofound<< endreq;	
				                if (histofound !=std::string::npos) {
				     	           //histogram found, save it 
				     	       //   msg << MSG::DEBUG << "found histo " << histo << endreq;	       
	                                           m_histogramfoundname.push_back(histo);
						   histosfound++;
	                                           //see if its unique
	                                           bool histnotexists=true;
	                                           if ((int)m_histogramuniquename.size()>0) {
	                                              for (int k=0;k<=(int)m_histogramuniquename.size()-1;k++) {
	                               	                 if (histo==m_histogramuniquename[k]) {
	                               	   	            histnotexists=false;
	                               	                    break;
	                               	                 }
	                                              }
	                                           }   
	                                           if (histnotexists) m_histogramuniquename.push_back(histo);	   
		                                      m_algorithmfoundname.push_back(m_algorithmname[i]);
		                                      m_taskfoundname.push_back(m_taskname[i]); 
		                                    //  msg << MSG::DEBUG << "pushing back svcfoundname " << servicestr << endreq;
					              Svcfoundname.push_back(servicestr);	         
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
	                       msg << MSG::DEBUG << "No wildcard found " << m_histogramname[i] << endreq;		 
                               m_histogramfoundname.push_back(m_histogramname[i]);
                               m_histogramuniquename.push_back(m_histogramname[i]);
	                       m_algorithmfoundname.push_back(m_algorithmname[i]);
	                       m_taskfoundname.push_back(m_taskname[i]);	  
			       Svcfoundname.push_back(servicestr);          
		            } 	
	                } //loop over histogramnames done
	            }
                }   
            }
  //now we have the services and the histogram names, we need to group the services per histogram

  if (histosfound>0) {
  msg << MSG::DEBUG << "Looking for " << histosfound << " histograms." << endreq;
  //msg << MSG::DEBUG << "Looking for " << m_histogramfoundname.size() << " histograms:" << endreq;
  for (int k=0;k<=(int)m_histogramuniquename.size()-1;k++) {
  	// msg << MSG::DEBUG << "Histogram " << k << " " << m_histogramuniquename[k] << endreq;
  	 icount=0;
	 icount2d=0;
	 icountp=0;
	 tmphSvcnames.clear();
	 tmphSvcnames2d.clear();
	 tmppSvcnames.clear();
  	 for (int i=0;i<=(int)m_histogramfoundname.size()-1;i++) {
  	 	if (m_histogramfoundname[i]==m_histogramuniquename[k]) {
	       if (Svcfoundname[i].substr(1,1)=="1") {
	       	  dimension=1;
	       }
	       else {	  
	          if (Svcfoundname[i].substr(1,1)=="2") {
	          	 dimension=2;
	          }
	          else dimension=3;
	       } 
	       switch (dimension) {
	       	  case 1:
	       	     tmphSvcnames.push_back(Svcfoundname[i]);
	       	     icount++;
	       	 //    msg << MSG::DEBUG << "Pushing back " << Svcfoundname[i] << " icount " << icount << endreq; 
	       	     break;
	       	  case 2:
	       	     tmphSvcnames2d.push_back(Svcfoundname[i]);
	       	     icount2d++;
	    //   	    msg << MSG::DEBUG << "Pushing back " << Svcfoundname[i] << " icount2d " << icount2d << endreq; 
	       	     break;
	       	  case 3:
	       	     tmppSvcnames.push_back(Svcfoundname[i]);
	       	     icountp++;
	     //  	    msg << MSG::DEBUG << "Pushing back " << Svcfoundname[i] << " icountp " << icountp << endreq; 
	       	     break;
	       	  default:
	       	     msg << MSG::DEBUG << "Unknown dimension " << dimension << endreq; 
	        }
  	 	 }   
  	  }
  	  hSvcname.push_back(tmphSvcnames);    
  	  hSvcname2d.push_back(tmphSvcnames2d); 
  	  pSvcname.push_back(tmppSvcnames); 	           
  	  nbof1dhistos.push_back(icount);
  	  nbof2dhistos.push_back(icount2d);
  	  nbofphistos.push_back(icountp);		
  }	 
 
 
  //the adder init creates dimservice objects
  //and creates an array of them
  //the adding is triggered by the dim timer
  
  std::string instance;
  std::string instance_node;
  float nboftasks;
  nboftasks=(int)m_histogramfoundname.size()/(int)m_histogramuniquename.size();

  for (int j=0; j<= (int)m_histogramuniquename.size()-1;j++) {
     //j counts the histograms
     //in this first loop we subscribe to the dimservices
     //do this so the result of the title has time to come back
  
     std::vector<DimInfoHistos*>	tmphinfo;
     std::vector<DimInfoHistos*>	tmphinfo2d;
     std::vector<DimInfoHistos*>	tmppinfo;
     DimInfoHistos * temp;
     DimInfoTitle * ttemp;     
     //update the DNS only once, when all new DimServices have been made
     DimServer::autoStartOff();

     if (nbof1dhistos[j]>0) {
           for (int i=0;i<=nbof1dhistos[j]-1;i++) { 
           	  temp = new DimInfoHistos(hSvcname[j][i],m_refreshtime); 
           	//  msg << MSG::DEBUG << "Created object: " << hSvcname[j][i] << endreq;        
	          tmphinfo.push_back(temp);
	          if (i==0) {
	          	 //do this first so the result of the title has time to come back
	          	 instance=hSvcname[j][i].substr(hSvcname[j][i].find_last_of("_")+1,1);
		         instance_node=hSvcname[j][i].substr(hSvcname[j][i].find("/")+1,hSvcname[j][i].find("_")-hSvcname[j][i].find("/")-1);
	          	 if (task=="Adder") {
			    commentSvcname=instance_node+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
	                 }
			 else {
			    commentSvcname=instance_node+"_"+m_taskfoundname[j]+"_"+instance+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
                         }
		  //   msg << MSG::DEBUG << "CommentSvcname: " <<  commentSvcname << endreq;   
	             ttemp = new DimInfoTitle(commentSvcname);
	             commentfoundSvcnames.push_back(ttemp);
	          }  	      
	       }
        }	
     if (nbof2dhistos[j]>0) {
           for (int i=0;i<=nbof2dhistos[j]-1;i++) {
           	 temp = new DimInfoHistos(hSvcname2d[j][i],m_refreshtime);        
	         tmphinfo2d.push_back(temp);
           	 if (i==0) {
	          	 instance=hSvcname2d[j][i].substr(hSvcname2d[j][i].find_last_of("_")+1,1);
		         instance_node=hSvcname2d[j][i].substr(hSvcname2d[j][i].find("/")+1,hSvcname2d[j][i].find("_")-hSvcname2d[j][i].find("/")-1);
	          	 if (task=="Adder") {
			    commentSvcname=instance_node+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
	                 }
			 else {
			    commentSvcname=instance_node+"_"+m_taskfoundname[j]+"_"+instance+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
                         }
		     ttemp = new DimInfoTitle(commentSvcname);
	             commentfoundSvcnames.push_back(ttemp);
	          } 

	  //       msg << MSG::DEBUG << "Created object: " << hSvcname2d[j][i] << endreq;   
	       }  
        }
     if (nbofphistos[j]>0) {
           for (int i=0;i<=nbofphistos[j]-1;i++) { 
           	  temp = new DimInfoHistos(pSvcname[j][i],m_refreshtime);         
	          tmppinfo.push_back(temp);
           	  if (i==0) {
	          	 instance=pSvcname[j][i].substr(pSvcname[j][i].find_last_of("_")+1,1);
		         instance_node=pSvcname[j][i].substr(pSvcname[j][i].find("/")+1,pSvcname[j][i].find("_")-pSvcname[j][i].find("/")-1);
	          	 if (task=="Adder") {
			    commentSvcname=instance_node+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
	                 }
			 else {
			    commentSvcname=instance_node+"_"+m_taskfoundname[j]+"_"+instance+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";
                         }
	             ttemp = new DimInfoTitle(commentSvcname);
	             commentfoundSvcnames.push_back(ttemp);
	          }

	    //     msg << MSG::DEBUG << "Created object: " << pSvcname[j][i] << endreq;  
	       }    
        }

	//update the array of dim buffers  
	hinfo.push_back(tmphinfo);  
	hinfo2d.push_back(tmphinfo2d);  
	pinfo.push_back(tmppinfo); 
  } //loop over j   

  for (int j=0; j<= (int)m_histogramuniquename.size()-1;j++) {
     //j counts the histograms 
     //in this second loop we set up the adder services for the results
     adderhSvcnames="H1D/"+m_servername+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j];
     adderhSvcnames2d="H2D/"+m_servername+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j];
     adderpSvcnames="HPD/"+m_servername+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j];
     addercommentSvcnames=m_servername+"_Adder_1/"+m_taskfoundname[j]+"/"+m_algorithmfoundname[j]+"/"+m_histogramuniquename[j]+"/gauchocomment";  
       char* temptitle=0;
       temptitle=commentfoundSvcnames[j]->getTitle();	
//       msg << MSG::DEBUG << "tempttitle " << temptitle << endreq;  
       if (nbof1dhistos[j]>0) {    
          hinfo[j][0]->declareTitleInfo(addercommentSvcnames,temptitle);  
      //    msg << MSG::DEBUG << "declaring Info: " << adderhSvcnames << " j " << j << " tempttitle " << temptitle << endreq;   
	      hinfo[j][0]->declareInfo(adderhSvcnames);
	      //initialise the sum
	      hinfo[j][0]->add(hinfo[j][0]);
        }
  	   
        if (nbof2dhistos[j]>0) {    
          hinfo2d[j][0]->declareTitleInfo(addercommentSvcnames,temptitle);  
  //         msg << MSG::DEBUG << "declaring Info: " << adderhSvcnames2d << " j " << j << " tempttitle " << temptitle << endreq;  
	      hinfo2d[j][0]->declareInfo(adderhSvcnames2d);
	      //initialise the sum
	      hinfo2d[j][0]->add2d(hinfo2d[j][0]);

        }
        if (nbofphistos[j]>0) {    
          pinfo[j][0]->declareTitleInfo(addercommentSvcnames,temptitle); 
  //        msg << MSG::DEBUG << "declaring Info: " << adderpSvcnames << " j " << j << " tempttitle " << temptitle << endreq;  
	      pinfo[j][0]->declareInfo(adderpSvcnames);
	      //initialise the sum
	      pinfo[j][0]->addp(pinfo[j][0]);

        }
  } //loop over j

  DimServer::start(m_procName.c_str());
    for (int j=0; j<= (int)m_histogramuniquename.size()-1;j++)  {
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
   
   msg << MSG::INFO << "Initialization completed. Starting DimTimer to add " << infohistos.size() << " histograms" << endreq;  
   tim= new Tim(m_refreshtime,infohistos);
  }
  else  {
     msg << MSG::WARNING << "No histograms found. Restart adder later. " << endreq;  
  }
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode Adder::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  //do nothing until a histogram gets updated	
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


