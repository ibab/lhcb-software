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
//  Changes    : 27/11/2008 EvH make this into a main program
//
//====================================================================
// Include files


#include "GaudiKernel/Bootstrap.h"
#include "OnlineHistDB/OnlineHistDB.h"
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


int main(int argc, char *argv[]) {

   for (int i=1;i<argc;i++) {
      if ((strcmp(argv[i],"-h")==0)||(strcmp(argv[i],"--help")==0)){   
        std::cout << "Usage: InsertDB [OPTION]" <<std::endl; 
	std::cout << "Declares histograms published by the top level Adder on the EFF to the histogram database.\n" <<std::endl; 
        std::cout << "Optional arguments:" <<std::endl; 
	std::cout << "  -d, --dns              Dim DNS name where histograms are published to. Default: hlt01" <<std::endl;
	std::cout << "  -s, --slice            Partition slice. Default: PART*\n " <<std::endl;
        std::cout << "Searches for histograms published by Dim services of the following formats:" <<std::endl; 
        std::cout << "MonH1*/+nodename+_Adder_1/*" <<std::endl; 
        std::cout << "MonH2*/+nodename+_Adder_1/*" <<std::endl; 
        std::cout << "MonP1*/+nodename+_Adder_1/*\n" <<std::endl; 
        std::cout << "Report bugs to eric.van.herwijnen@cern.ch" <<std::endl;
        return 0;
     }  
     else { 
        if ((strcmp(argv[i],"-s")==0)||(strcmp(argv[i],"--slice")==0)){   
           i++;
           m_nodename=argv[i];
           continue;
        } 
	else {
           if ((strcmp(argv[i],"-d")==0)||(strcmp(argv[i],"--dns")==0)){   
              i++;
              m_dimclientdns=argv[i];
              continue;
           } 	   
           else {
              std::cout << "InsertDB: Invalid option " <<argv[i]<< std::endl;
              std::cout << "Try `InsertDB --help` for more information." << std::endl;
              return 0;
           }
        }
     }
   }  
   if (m_nodename=="") m_nodename="PART*"; 
   if (m_dimclientdns=="") m_dimclientdns="hlt01"; 
 
   DimClient::setDnsNode(m_dimclientdns.c_str());  
   DimBrowser dbr;

   std::string password="histeggia194";
   OnlineHistDB *HistDB = new OnlineHistDB(password,"HIST_WRITER","ora01:1528/HISTOGRAMDB");
   if (HistDB==0) {
       std::cout << "Could not connect to Histogram database." << std::endl;
   }
   else { 
       hSvcnames="MonH1*/"+m_nodename+"_*";
       hSvcnames2d="MonH2*/"+m_nodename+"_*";
       pSvcnames="MonP1*/"+m_nodename+"_*";
       std::cout << "Looking for service: " << hSvcnames.c_str() << " using DNS: "<< m_dimclientdns<< std::endl;     
   
       dbr.getServices(hSvcnames.c_str());
       icount=0;
       icount2d=0;
       icountp=0;
       while( (type = dbr.getNextService(service, Dimformat)) )
       { 
	   std::vector<int> temp;
	   hSvcname=service;
           std::cout << "Found service: " << hSvcname << std::endl;     
           std::string::size_type loc=hSvcname.find("MonH1*/"+m_nodename+"_Adder_",0);
	   if (loc != std::string::npos ) {  
              std::cout << "Found service: " << hSvcname  <<std::endl;     
	      icount++;
	      for (int i=1;i<=(int)hSvcname.length();i++) {
	         if (hSvcname.substr(i,1)=="/") {
		    temp.push_back(i);
		 }	      	     
	      }    
	      HistDB->declareHistByServiceName(hSvcname);
	   }
	} 
	std::cout << "Looking for service: " << hSvcnames2d.c_str() << " using DNS: "<< m_dimclientdns<< std::endl;     
        dbr.getServices(hSvcnames2d.c_str());

        while( (type = dbr.getNextService(service, Dimformat)) )
        { 
	      std::vector<int> temp;
	      hSvcname2d=service;
              std::cout << "Found service: " << hSvcname2d << std::endl;     
              std::string::size_type loc=hSvcname2d.find("MonH2*/"+m_nodename+"_Adder_",0);
	      if (loc != std::string::npos ) {  
                 std::cout << "Found service: " << hSvcname  <<std::endl;     
	         icount2d++;
	         for (int i=1;i<=(int)hSvcname2d.length();i++) {
	            if (hSvcname2d.substr(i,1)=="/") {
		       temp.push_back(i);
		    }	      	     
	         }    
	         HistDB->declareHistByServiceName(hSvcname2d);
	      }
         } 
         std::cout << "Looking for service: " << pSvcnames.c_str() << " using DNS: "<< m_dimclientdns<< std::endl;     
              dbr.getServices(pSvcnames.c_str());
  
         while( (type = dbr.getNextService(service, Dimformat)) )
         { 
	         std::vector<int> temp;
	         pSvcname=service;
                 std::cout << "Found service: " << pSvcname << std::endl;     	   
                 std::string::size_type loc=pSvcname.find("MonP1*/"+m_nodename+"_Adder_",0);
	         if (loc != std::string::npos ) {  
                    std::cout << "Found service: " << pSvcname  <<std::endl;     
	            icountp++;
	            for (int i=1;i<=(int)pSvcname.length();i++) {
	               if (pSvcname.substr(i,1)=="/") {
		          temp.push_back(i);
		       }	      	     
	            }    
	            HistDB->declareHistByServiceName(pSvcname);
	         }
         }
       std::cout << "Committing changes to Hist DB" << std::endl;
       HistDB->commit();

     }
     delete HistDB;
  //}	
  return 0; 
}

