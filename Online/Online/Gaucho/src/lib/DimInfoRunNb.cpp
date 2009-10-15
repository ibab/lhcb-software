#include "Gaucho/DimInfoRunNb.h"
#include "RTL/rtl.h"

#include <iostream>
#include <string>

DimInfoRunNb::DimInfoRunNb(std::string RunNbSvcName) :
DimInfo((RunNbSvcName).c_str(),-1) {
   m_hasData=false;

}

//destructor
DimInfoRunNb::~DimInfoRunNb() {  
}

void DimInfoRunNb::infoHandler() 
{ 
  m_data = getInt();
  if (m_data!=0) setRunNb();
} 


int DimInfoRunNb::getRunNb() {
  int ntries =0;
  while (ntries<25) {
     if(m_hasData == true ) {
     	 //  std::cerr << "DimInfoRunNb sending data. " << m_data << std::endl;
	   return m_data;
	}
	else { 
	   dim_wait();
	//   std::cerr << "DimInfoRunNb waiting for data. " << std::endl;
	   sleep(2);
	   ntries++;
	}   
  }	
  return 0;
}


void DimInfoRunNb::setRunNb(){     
  m_hasData=true;
}



