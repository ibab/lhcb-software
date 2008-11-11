#include "Gaucho/RateExtractor.h"
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
// improve this naming comvention by getting counter signification
// from MonRate when it will provide it.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
std::string RateExtractor::makeServiceName(std::string nameHeader)
{
  std::string comment = getCommentFromMonRate();
  
  
  char * c = new char[comment.length()+1];
  strcpy(c, comment.c_str());
  
  std::string name = "";
  char * part = NULL;
  
  part = strtok(c, " ");
  while(part != NULL)
  {
    name += part;
    part = strtok(NULL, " ");
  }
  
  if(name == "")
  {

    std::stringstream streamName;
    streamName << nameHeader << "/rateFromCounter#" << m_counterId;
 //    name = streamName.str();
   name="";
  }else{
    name = nameHeader + "/" + name;
  }
  
  COUT_DEBUG("###########################################")
  COUT_DEBUG("counter #" << m_counterId << " >>> " << name);
  COUT_DEBUG("###########################################");
  
  return name;
}

void RateExtractor::publishService(std::string nameHeader)
{
  setValue(0);
  if (getCommentFromMonRate()!="") {
     setComment(getCommentFromMonRate());
  
     std::string serviceName = makeServiceName(nameHeader);
  
     RatePublisher::publishService(serviceName);
  
   //  COUT_DEBUG(" FOR COUNTER " << m_counterId);
  }   
}

double RateExtractor::getCounterFromMonRate()
{
  if(!m_pMonRate)
    return -1;
    
  TProfile * profile = m_pMonRate->profile();
  
  double numberOfProcesses = profile->GetBinEntries(4);  
  //double counterMean = profile->GetBinContent(8 + m_counterId);
  double counterMean = m_pMonRate->binCont[8 + m_counterId];
  //the 8th is the gps time
  if (m_counterId==0) return counterMean;
  else return numberOfProcesses * counterMean;
}

longlong RateExtractor::getCycleLengthFromMonRate()
{
 // TProfile * profile = m_pMonRate->profile();
 // return (longlong)profile->GetBinContent(7);  
  return (longlong)m_pMonRate->binCont[7];
}

std::string RateExtractor::getCommentFromMonRate()
{
  if(!m_pMonRate)
    return "";
    
  TProfile * profile = m_pMonRate->profile();  
  std::string comment(profile->GetXaxis()->GetBinLabel(8 + m_counterId));
  
  // COUT_DEBUG("Comment for rate from counter #" << m_counterId << " = " << comment);
   
   return comment;

//  std::stringstream comment;
  
//  comment << "Comment for rate from counter #" << m_counterId;
  

  
//  return comment.str();
}

//bool RateExtractor::extractData(longlong time, int nbElapsedCycles)
bool RateExtractor::extractData(longlong time, int nbElapsedCycles)
{
  bool success = true;
  bool anyUpdate = false;
  
  if(nbElapsedCycles <= 0)
    return false;
  
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
  
  double deltaT = (double)(m_newTime - m_oldTime);
  //this number is the difference between two averages, we set it to 1
 // nbElapsedCycles=1;
// COUT_DEBUG("m_counterOldValue " << m_counterOldValue << " m_counterNewValue " << m_counterNewValue );
  deltaT += (double)(nbElapsedCycles * getCycleLengthFromMonRate());    
  double newValueForRate = m_counterNewValue - m_counterOldValue;
  newValueForRate *= 1000000.0;
  newValueForRate /= deltaT;
  
//  COUT_DEBUG("NEW CALCULATED VALUE FOR RATE " << m_counterId << " IS " << newValueForRate);
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
