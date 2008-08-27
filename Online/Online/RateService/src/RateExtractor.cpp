#include "RateExtractor.h"
#include "Gaucho/MonRate.h"

#include <iostream>

int min(int x, int y)
{
  if(x < y) return x;
  return y;
}

RateExtractor::RateExtractor(std::string rateId,
                             MonRate * pMonRate)
     : m_pMonRate(pMonRate),
       m_rateId(rateId),
       //m_valueService(0),
       //m_commentService(0),
       //m_cComment(0),
       m_oldTime(0),
       m_newTime(0),
       m_counterOldValue(0),
       m_counterNewValue(0)
{
  strcpy(m_data.comment, "\0");
  m_commentSize = 1;
}

RateExtractor::~RateExtractor()
{
  if(m_data.comment) delete[] m_data.comment;
  
  //if(m_valueService) delete m_valueService;
  //if(m_commentService) delete m_commentService;
  if(m_structuredService) delete m_structuredService;
}

void RateExtractor::reshapeComment()
{
}


std::string RateExtractor::makeServiceName(std::string nameHeader)
{
  return nameHeader+"/"+m_rateId;
}

/*
std::string RateExtractor::makeValueServiceName(std::string nameHeader)
{
  return nameHeader+"/"+m_rateId;
}

std::string RateExtractor::makeCommentServiceName(std::string nameHeader)
{
  return makeValueServiceName(nameHeader) + "/comment";
}*/

void RateExtractor::publishServices(std::string nameHeader)
{
  //m_valueService = new DimService(makeValueServiceName(nameHeader).c_str(), m_rateValue);
  //m_commentService = new DimService(makeCommentServiceName(nameHeader).c_str(), m_cComment);
  
  char * tmpFormat = new char[s_rateServiceFormat.length()+1];
  strcpy(tmpFormat, s_rateServiceFormat.c_str());
  
  std::cout << "CREATING SERVICE " << makeServiceName(nameHeader) << " FORMAT = " << tmpFormat;
  std::cout << " FOR COUNTER " << m_rateId;// << " OF MonRate " << m_pMonRate->...();
  std::cout << std::endl;

  m_structuredService = new DimService(makeServiceName(nameHeader).c_str(),
                                       tmpFormat,
				       (void*)&m_data,
				       sentDataSize());

  delete tmpFormat;
}

int RateExtractor::sentDataSize()
{
  return sizeof(double) + m_commentSize * sizeof(char);
}

bool RateExtractor::extractData(longlong time)
{
  bool success = true;
  bool anyUpdate = false;
  
  try
  {
    /*====================================================*/
    /*======= RATE CALCULATION ===========================*/
    /*====================================================*/
    /* get the new counter value
     */
    m_counterOldValue = m_counterNewValue;
    m_counterNewValue = m_pMonRate->counter(m_rateId);

  //  info() << "Got the counters" << endreq;
    /* get the new time interval
     */
    m_oldTime = m_newTime;
    m_newTime = time;
    
    double oldValue = m_data.value;
  
    m_data.value = 1000*(m_counterNewValue - m_counterOldValue);
    m_data.value /= (m_newTime - m_oldTime);
    
    if(oldValue != m_data.value)
      anyUpdate = true;
    /*====================================================*/
    /*====================================================*/
        
	
    /*====================================================*/
    /*======= COMMENT UPDATE =============================*/
    /*====================================================*/
    std::string comment = m_pMonRate->counterDescription(m_rateId);
    
    /* if comment changed (visible part only)
     */
    if(strncmp(m_data.comment, comment.c_str(), s_maxRateCommentSize-1) != 0)
    { 
      m_commentSize = min(comment.length()+1, s_maxRateCommentSize);
      strncpy(m_data.comment, comment.c_str(), m_commentSize);
      anyUpdate = true;
    }
    /*====================================================*/
    /*====================================================*/
    
    /*====================================================*/
    /*======= SERVICE UPDATE =============================*/
    /*====================================================*/
    if(anyUpdate)
    {
      if(m_structuredService)
        m_structuredService->updateService((void*)&m_data, sentDataSize());
      else
        success = false;
    }
    /*====================================================*/
    /*====================================================*/
  }
  catch(const std::exception & e)
  {
    /* possible exception in 
       m_counterNewValue = m_pMonRate->counter(m_rateId);
     * or in
       m_rateValue /= (m_newTime - m_oldTime);
     */
    success = false;
  }
  
  return success;
}
