#include "RatePublisher.h"

#include <iostream>

int min(int x, int y)
{
  if(x < y) return x;
  return y;
}

void RatePublisher::printData()
{
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
  std::cout << "comment = " << m_comment << std::endl;
  std::cout << "value = " << m_value << std::endl;
  std::cout << "in struct:" << std::endl;
  std::cout << "m_rateData.comment = " << m_rateData.comment << std::endl;
  std::cout << "m_rateData.value = " << m_rateData.value << std::endl;
  std::cout << std::endl;
  std::cout << "sent data size = " << sentDataSize() << std::endl;
  std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
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
  std::cout << "RatePublisher::updateService()" << std::endl;
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
  std::cout << "RatePublisher::publishService(std::string serviceName)" << std::endl;
  m_serviceName = serviceName;
    
  char * tmpFormat = new char[s_rateServiceFormat.length()+1];
  strcpy(tmpFormat, s_rateServiceFormat.c_str());

  std::cout << "CREATING SERVICE " << serviceName << " FORMAT = " << tmpFormat << std::endl;
  
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
  return sizeof(double) + (min(MAX_CAR, m_comment.length())) * sizeof(char);
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
