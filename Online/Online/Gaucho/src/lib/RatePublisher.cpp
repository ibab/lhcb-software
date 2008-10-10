#include "Gaucho/RatePublisher.h"

#include <iostream>

int min(int x, int y)
{
  if(x < y) return x;
  return y;
}

void RatePublisher::printData()
{
  COUT_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  COUT_DEBUG("comment = " << m_comment);
  COUT_DEBUG("value = " << m_value)
  COUT_DEBUG("in struct:")
  COUT_DEBUG("m_rateData.comment = " << m_rateData.comment);
  COUT_DEBUG("m_rateData.value = " << m_rateData.value);
  COUT_DEBUG("");
  COUT_DEBUG("sent data size = " << sentDataSize());
  COUT_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

RatePublisher::RatePublisher()
{
  strcpy(m_rateData.comment, "\0");
  m_comment = "";
}

RatePublisher::~RatePublisher()
{
  clearRateData();
  if(m_structuredService) delete m_structuredService;
}


void RatePublisher::updateService()
{
  COUT_DEBUG("RatePublisher::updateService()");
  if(!m_structuredService)
    return;
  
  fillRateData();
  
 // printData();
  
  m_structuredService->updateService((void*)&m_rateData, sentDataSize());
}

void RatePublisher::updateService(double value, std::string comment)
{
  setValue(value);
  setComment(comment);
  
  updateService();
}

void RatePublisher::updateService(double value)
{
  setValue(value);
  
  updateService();
}

void RatePublisher::publishService(std::string serviceName)
{
  COUT_DEBUG("RatePublisher::publishService(std::string serviceName)");
  m_serviceName = serviceName;
    
  char * tmpFormat = new char[s_rateServiceFormat.length()+1];
  strcpy(tmpFormat, s_rateServiceFormat.c_str());

  COUT_DEBUG("CREATING SERVICE " << serviceName << " FORMAT = " << tmpFormat);
  
  fillRateData();
  
  //printData();
  
  m_structuredService = new DimService(serviceName.c_str(),
                                       tmpFormat,
				       (void*)&m_rateData,
				       sentDataSize());
  delete tmpFormat;
}

int RatePublisher::sentDataSize()
{
  return sizeof(double) + (min(MAX_CAR, m_comment.length()+1)) * sizeof(char);
}

void RatePublisher::fillRateData()
{
  clearRateData();
  
  m_rateData.value = m_value;
  int commentSize = min(MAX_CAR, m_comment.length()+1);
    
  strncpy(m_rateData.comment, m_comment.c_str(), commentSize);
  
 /* std::cout << " ---------------------- >>>> copied comment in struct = " <<
               m_rateData.comment << std::endl;*/
}

void RatePublisher::clearRateData()
{
  strcpy(m_rateData.comment, "\0");
  m_rateData.value = 0;
}
