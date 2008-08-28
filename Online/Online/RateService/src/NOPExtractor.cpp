#include "NOPExtractor.h"
#include "Gaucho/MonRate.h"

#include <iostream>


NOPExtractor::NOPExtractor(MonRate * pMonRate)
     : m_pMonRate(pMonRate)
{
  setComment("Number of processes publishing via this MonRate");
}

NOPExtractor::~NOPExtractor()
{
}

int NOPExtractor::getNOPFromMonRate()
{
  if(!m_pMonRate)
    return -1;
  TProfile * profile = m_pMonRate->profile();
  
  return profile->GetBinEntries(4);
}


std::string NOPExtractor::makeServiceName(std::string nameHeader)
{
  return nameHeader+"/"+s_NOPServiceNameEnd;
}

void NOPExtractor::publishService(std::string nameHeader)
{
  std::string serviceName = makeServiceName(nameHeader);
  
  RatePublisher::publishService(serviceName);
}

bool NOPExtractor::extractData()
{
  int NOP = getNOPFromMonRate();
  if( NOP  == -1)
    return false;
  
  setValue(NOP);
  
  updateService();
  
  return true;
}
