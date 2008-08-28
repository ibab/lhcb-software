#include "RateExtractor.h"
#include "Gaucho/MonRate.h"

#include <iostream>
#include <sstream>


RateExtractor::RateExtractor(int counterId,
                             MonRate * pMonRate)
     : m_pMonRate(pMonRate),
       m_counterId(counterId),
       m_oldTime(0),
       m_newTime(0),
       m_counterOldValue(0),
       m_counterNewValue(0)
{
}

RateExtractor::~RateExtractor()
{
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// improve thi naming comvention by getting counter signification
// from MonRate when it will provide it.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
std::string RateExtractor::makeServiceName(std::string nameHeader)
{
  std::stringstream streamName;
  streamName << nameHeader << "/rateFromCounter#" << m_counterId;
  
  std::string name = streamName.str();
  
  std::cout << "###########################################" << std::endl;
  std::cout << "###########################################" << std::endl;
  std::cout << "m_counterId = " << m_counterId << std::endl;
  std::cout << "name = " << name << std::endl;
  std::cout << "###########################################" << std::endl;
  std::cout << "###########################################" << std::endl;
  
  return name;
}

void RateExtractor::publishService(std::string nameHeader)
{
  std::string serviceName = makeServiceName(nameHeader);
  
  RatePublisher::publishService(serviceName);
  
  std::cout << " FOR COUNTER " << m_counterId;// << " OF MonRate " << m_pMonRate->...();
  std::cout << std::endl;  
}

int RateExtractor::getCounterFromMonRate()
{
  if(!m_pMonRate)
    return -1;
    
  TProfile * profile = m_pMonRate->profile();
  
  return profile->GetBinContent(8 + m_counterId);
}

std::string RateExtractor::getCommentFromMonRate()
{
  if(!m_pMonRate)
    return "";
    
  TProfile * profile = m_pMonRate->profile();
    
//  return profile->GetBinLabel(8 + m_counterId);  

  std::stringstream comment;
  comment << "Comment for rate from counter #" << m_counterId;
  
  return comment.str();
}

bool RateExtractor::extractData(longlong time)
{
  bool success = true;
  bool anyUpdate = false;
  
  /*====================================================*/
  /*======= RATE CALCULATION ===========================*/
  /*====================================================*/
  /* get the new counter value
   */
  m_counterOldValue = m_counterNewValue;
  m_counterNewValue = getCounterFromMonRate();
 
  //  info() << "Got the counters" << endreq;
  /* get the new time interval
   */
  m_oldTime = m_newTime;
  m_newTime = time;
    
  double oldValue = getValue();
  
  double newValueForRate = m_counterNewValue - m_counterOldValue;
  newValueForRate *= 1000000.0;
  newValueForRate /= (double)(m_newTime - m_oldTime);
  
  //std::cout << "NEW CALCULATED VALUE FOR RATE " << m_counterId << " IS " << newValueForRate << std::endl;
  setValue( newValueForRate );
    
  if(oldValue != getValue())
    anyUpdate = true;
  /*====================================================*/
  /*====================================================*/
	
  /*====================================================*/
  /*======= COMMENT UPDATE =============================*/
  /*====================================================*/
  std::string comment = getCommentFromMonRate();
    
  if( getComment() != comment)
  { 
    setComment(comment);
    anyUpdate = true;
  }
  /*====================================================*/
  /*====================================================*/
  
  /*====================================================*/
  /*======= SERVICE UPDATE =============================*/
  /*====================================================*/
  if(anyUpdate)
    updateService();
  else
    success = false;
  /*====================================================*/
  /*====================================================*/
  
  return success;
}
