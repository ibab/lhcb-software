#include "Gaucho/DimInfoRunNb.h"
#include "RTL/rtl.h"

#include <iostream>
#include <string>

DimInfoRunNb::DimInfoRunNb(std::string RunNbSvcName,std::string slice) :
DimInfo((RunNbSvcName).c_str(),-1) {
  // std::cerr << "DimInfoRunNbSvc " << RunNbSvcName << " created " << std::endl;
   m_hasData=false;
   m_runNumber=0;
   m_data=0;
   m_slice=slice;
}

//destructor
DimInfoRunNb::~DimInfoRunNb() {  
}

void DimInfoRunNb::infoHandler() 
{ 
  m_data = getInt();
  if (m_data !=0) {
     if ((m_data!=m_runNumber)&&(m_data!=-1)) {
       // std::cout << "DimInfoRun runnumber " << m_data << std::endl;
        std::string saversvc=m_slice+"_Saver_1/";
        if (m_runNumber !=0) {
        //  std::cout << "run change old runNumber= " << m_runNumber << std::endl;  
          //need to check that these savesets are complete
          //for fast run changes
          // DimClient::sendCommand(saversvc.c_str(),"save_histos");
        }     
     }
   //  else std::cout << "DimInfoRun runnumber " << m_data << std::endl;  
   //  std::cout << "setting runNumber to m_data =" << m_data << std::endl;  	
     m_runNumber=m_data;
  }    
  if ((m_data!=0)&&(m_data!=-1)) setRunNb();
} 


int DimInfoRunNb::getRunNb() {
  int ntries =0;
  if (m_data==0) m_data=m_runNumber;
  while (ntries<25) {
     if (m_hasData==true) {
     	  // std::cout << "DimInfoRun sending data. " << m_data << std::endl;
	   return m_data;
	}
	else { 
	//   dim_wait();
	//   std::cout << "DimInfoRunNb waiting for data. " << std::endl;
	   lib_rtl_sleep(1000);
	   ntries++;
	}   
  }	
  return 999;
}


void DimInfoRunNb::setRunNb(){     
  m_hasData=true;
}



