#include "Gaucho/DimInfoTCK.h"
#include "RTL/rtl.h"

#include <iostream>
#include <string>

DimInfoTCK::DimInfoTCK(std::string TCKSvcName) :
DimInfo((TCKSvcName).c_str(),-1) {
   m_hasData=false;
   m_TCK=0;
   m_data=0;
}

//destructor
DimInfoTCK::~DimInfoTCK() {  
}

void DimInfoTCK::infoHandler() 
{ 
  m_data = getInt();
  if (m_data!=-1) {
    std::cout << "DimInfoTCK TCK " << (unsigned int) m_data << std::endl;
     m_TCK=(unsigned int)m_data;
  }
 else std::cout << "DimInfoTCK TCK" <<(unsigned int) m_data << std::endl;   
  if ((m_data!=0)&&(m_data!=-1)) setTCK();
} 


unsigned int DimInfoTCK::getTCK() {
  int ntries =0;
  while (ntries<25) {
     if (m_hasData==true) {
     	   std::cout << "DimInfoTCK sending data. " << (unsigned int) m_data << std::endl;
	   return (unsigned int) m_data;
	}
	else { 
	   dim_wait();
	   std::cerr << "DimInfoRunNb waiting for data. " << std::endl;
	   lib_rtl_sleep(20000);
	   ntries++;
	}   
  }	
  return 999;
}


void DimInfoTCK::setTCK(){     
  m_hasData=true;
}



