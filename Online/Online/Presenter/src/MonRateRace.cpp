#include "MonRateRace.h"

boost::mutex infoHandlerMutex;

MonRateRace::MonRateRace(const std::string & serviceName) :
  DimInfo(serviceName.c_str(), -1.0),
  m_value(0.0),
  m_title("")
{
  m_infoHandlerMutex = new boost::mutex();
}

void MonRateRace::infoHandler()
{ 
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock) {
std::cout << "got lock..." << std::endl;    
    char* cnt_comment  = (char*) getData();
    std::stringstream cntCommentStream;     
    for (int ptr = 2*sizeof(double); ptr < (int)(getSize()/sizeof(char)); ++ptr ){
      cntCommentStream << (char)cnt_comment[ptr];               
    }
    cntCommentStream << std::endl;  
    m_title = cntCommentStream.str();
    cntCommentStream.str("");  
    
    double* histoDimData;
     histoDimData = (double*) getData();
     m_value   = (double) histoDimData[0];
     infoHandlerLock.unlock();
  } else {
std::cout << "no lock..." << std::endl;  
  }
  
}
double MonRateRace::currentValue() // boost::recursive_mutex & dimMutex, boost::recursive_mutex & rootMutex
{
std::cout << "value: " << m_value << std::endl;  
    return m_value;    
}
std::string MonRateRace::title() // boost::recursive_mutex & dimMutex, boost::recursive_mutex & rootMutex
{
std::cout << "title: " << m_title << std::endl;  
    return m_title;
}

MonRateRace::~MonRateRace()
{
  if (m_infoHandlerMutex) { delete m_infoHandlerMutex; m_infoHandlerMutex = NULL;}  
}
