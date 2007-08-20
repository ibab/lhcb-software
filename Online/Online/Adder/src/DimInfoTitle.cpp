#include "DimInfoTitle.h"
#include "RTL/rtl.h"

#include <iostream>
#include <string>
#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep win::Sleep
#else
# define mysleep usleep
#endif

//constructor
DimInfoTitle::DimInfoTitle(std::string commentSvcname)
  : DimInfo((commentSvcname).c_str(),-1),
    m_data(0),m_hasData(false){
//  std::cerr << "DimInfoTitle(" << commentSvcname << "). " << std::endl;
  int len = commentSvcname.length();
  std::string::size_type slash=commentSvcname.find_last_of("/",len);
  std::string m_servicetype=commentSvcname.substr(slash+1);
  if(       m_servicetype != "gauchocomment" )  {        
  	std::cerr << "DimInfoGetTitle(" << commentSvcname << "): Not a comment service. " 
        << std::endl;
    exit(1);
  }
  
}



//destructor
DimInfoTitle::~DimInfoTitle() {  
}

void DimInfoTitle::infoHandler() 
{ 
  m_data = (char*) getData();
//  std::cerr << "DimInfoTitle data found: " << m_data << std::endl;
  setTitleData();
} 


char* DimInfoTitle::getTitle() {

  while (1) {
     if(m_hasData == true ) {
	 //  std::cerr << "DimInfoTitle sending data. " << m_data << std::endl;
	   return m_data;
	}
	else dim_wait();
//	std::cerr << "DimInfoTitle waiting for data. " << std::endl;
  }	
  return 0;
}



void DimInfoTitle::setTitleData(){     
  m_hasData=true;
}



