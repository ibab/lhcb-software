#include "RateExtractor.h"
#include "Gaucho/MonRate.h"

#include <iostream>
#include <sstream>


RateExtractor::RateExtractor(int counterId,
                             MonRate * pMonRate)
     : m_counterId(counterId),
       m_pMonRate(pMonRate),
       m_counterOldValue(0),
       m_counterNewValue(0),
       m_oldTime(0),
       m_newTime(0)
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
  
  COUT_DEBUG("###########################################")
  COUT_DEBUG("###########################################");
  COUT_DEBUG("m_counterId = " << m_counterId);
  COUT_DEBUG("name = " << name);
  COUT_DEBUG("###########################################");
  COUT_DEBUG("###########################################");
  
  return name;
}

void RateExtractor::publishService(std::string nameHeader)
{
  std::string serviceName = makeServiceName(nameHeader);
  
  RatePublisher::publishService(serviceName);
  
  COUT_DEBUG(" FOR COUNTER " << m_counterId);
}

double RateExtractor::getCounterFromMonRate()
{
  if(!m_pMonRate)
    return -1;
    
  TProfile * profile = m_pMonRate->profile();
  
  return (double)profile->GetBinContent(8 + m_counterId);
}

longlong RateExtractor::getCycleLengthFromMonRate()
{
  TProfile * profile = m_pMonRate->profile();
  
  return (longlong)profile->GetBinContent(7);
}

std::string RateExtractor::getCommentFromMonRate()
{
  if(!m_pMonRate)
    return "";
    
//  TProfile * profile = m_pMonRate->profile();  
//  return profile->GetBinLabel(8 + m_counterId);  

  std::stringstream comment;
  
  comment << "Comment for rate from counter #" << m_counterId;
  
  COUT_DEBUG("Comment for rate from counter #");
  
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
  
  double deltaT = (double)(m_newTime - m_oldTime + getCycleLengthFromMonRate());
  
  double newValueForRate = m_counterNewValue - m_counterOldValue;
  newValueForRate *= 1000000.0;
  newValueForRate /= deltaT;
  
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
