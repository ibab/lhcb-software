// $Id: MonRateRace.cpp,v 1.15 2010-05-16 18:09:40 robbep Exp $
#include "MonRateRace.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/thread/mutex.hpp>

#ifdef WIN32
#pragma warning(disable:4355) 
#endif

boost::mutex infoHandlerMutex;

MonRateRace::MonRateRace(const std::string & serviceName, 
                         pres::EffServiceType serviceType) :
  DimInfo(serviceName.c_str(), (double)std::numeric_limits<double>::min(), 
          this),
  m_value(0.0),
  m_title(pres::s_eff_init),
  m_rateIsValid(false),
  m_serviceType(serviceType) {
  m_infoHandlerMutex = new boost::mutex();
}

void MonRateRace::infoHandler() { 
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock) {
    if (pres::MonRate == m_serviceType) {   
      char* cnt_comment  = 0;
      cnt_comment = static_cast<char*>(getData());
      std::stringstream cntCommentStream;     
      for (int ptr = 2*sizeof(double); ptr < (int)(getSize()/sizeof(char)); 
           ++ptr ){
        cntCommentStream << static_cast<char>(cnt_comment[ptr]);
        if ( boost::all( cntCommentStream.str().c_str(), boost::is_print()) ) {
        } else {
        }
      }
      double* histoDimData = 0;
      histoDimData = static_cast<double*>(getData());
  
      if (std::numeric_limits<double>::min() == 
          static_cast<double>(*histoDimData)) {
        m_value = 0.0;
        m_rateIsValid = false;
        m_title = pres::s_eff_init;
      } else {    
        m_value = static_cast<double>(histoDimData[0]);     
        if ((std::numeric_limits<double>::min() < m_value) &&
            (std::numeric_limits<double>::max() > m_value) ) { 
          m_title = cntCommentStream.str();
          if ( boost::all( m_title.c_str(), boost::is_print()) ) {
            m_rateIsValid = true;
          } else {
            m_rateIsValid = false;
          }           
         } else { // something else...
           m_value = 0.0;
           m_title = pres::s_eff_init;
           m_rateIsValid = false;      
         }
      }
      cntCommentStream.str("");
    } else if (pres::TCKinfo == m_serviceType) {
      char* tckNickname  = 0;
      tckNickname = static_cast<char*>(getData());
      std::stringstream cntTckNickname;     
      for (int ptr = sizeof(int); ptr < (int)(getSize()/sizeof(char)); ++ptr ){
        cntTckNickname << static_cast<char>(tckNickname[ptr]);               
      }
      m_title = cntTckNickname.str();
      if ( boost::all( m_title.c_str(), boost::is_print()) ) {

        int* histoDimData = 0;
        histoDimData = static_cast<int*>(getData());

        m_tck = static_cast<int>(histoDimData[0]);
//        if ((std::numeric_limits<int>::min() < m_tck) &&
//            (std::numeric_limits<int>::max() > m_tck) ) {
          m_rateIsValid = true;
//        }
      } else {
        m_rateIsValid = false;
        m_tck = 0;
      }
      cntTckNickname.str("");
    }
  }
}

bool MonRateRace::isRateValid() {
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock) {
   return m_rateIsValid;
  } else {
    return false;
  }
}

double MonRateRace::currentValue() {
  double returnValue = 0.0;
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock && m_rateIsValid) {
    returnValue = m_value;
    infoHandlerLock.unlock();    
  }
  return returnValue;
}

int MonRateRace::currentTCK() {
  int returnValue = 0;
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock && m_rateIsValid) {
    returnValue = m_tck;
    infoHandlerLock.unlock();    
  }
  return returnValue;
}

std::string MonRateRace::title() {
  std::string returnTitle(pres::s_eff_init);
  boost::unique_lock<boost::mutex> infoHandlerLock(*m_infoHandlerMutex);
  if (infoHandlerLock && m_rateIsValid &&
      pres::s_eff_init != m_title) {
    returnTitle = m_title;
    infoHandlerLock.unlock();    
  }  
  return returnTitle;  
}

MonRateRace::~MonRateRace() {
  if (m_infoHandlerMutex) { 
    delete m_infoHandlerMutex ; 
    m_infoHandlerMutex = NULL;
  }  
}
