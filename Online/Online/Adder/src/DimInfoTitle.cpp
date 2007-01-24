#include "DimInfoTitle.h"

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
DimInfoTitle::DimInfoTitle(std::string commentSvcname, int rtime)
  : DimInfo((commentSvcname).c_str(),rtime,-1.0),
    m_data(0),m_hasData(false){
 // std::cerr << "DimInfoTitle(" << commentSvcname << "). " << std::endl;
  m_rtime=rtime;
  int len = commentSvcname.length();
  std::string::size_type slash=commentSvcname.find_last_of("/",len);
  std::string m_servicetype=commentSvcname.substr(slash+1);
  if(       m_servicetype != "gauchocomment" )  {        
  	std::cerr << "DimInfoGetTitle(" << commentSvcname << "): Not a comment service. " 
        << std::endl;
    exit(1);
  }
  
}


DimInfoTitle::DimInfoTitle(std::string commentSvcname) : 
  DimInfo((commentSvcname).c_str(),-1.0){
//  std::cerr << "DimInfoTitle(" << commentSvcname << ") called with no refresh time. Using 5 s" << std::endl;
  DimInfoTitle(commentSvcname,5);
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
//std::cerr << "DimInfoTitle returning data: " << m_data << std::endl;
  if(m_hasData == true ) return m_data;
  return 0;
}



void DimInfoTitle::setTitleData(){     
  m_hasData=true;
}



