#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonProfile.h"

MonObjectCreator::MonObjectCreator()
{
}

MonObjectCreator::~MonObjectCreator(){
}


MonObject* MonObjectCreator::createMonObject(const std::string &monObjectTypeName, 
                                             IMessageSvc*  msgSvc, const std::string &source){
  MonObject* monObject=0;
  if (monObjectTypeName.compare(s_monObject) == 0) monObject = new MonObject(msgSvc, source);
  if (monObjectTypeName.compare(s_monH1F) == 0) monObject = new MonH1F(msgSvc, source);
  if (monObjectTypeName.compare(s_monH1D) == 0) monObject = new MonH1D(msgSvc, source);
  if (monObjectTypeName.compare(s_monH2F) == 0) monObject = new MonH2F(msgSvc, source);
  if (monObjectTypeName.compare(s_monH2D) == 0) monObject = new MonH2D(msgSvc, source);
  if (monObjectTypeName.compare(s_monProfile) == 0) monObject = new MonProfile(msgSvc, source);
  return monObject;
}

MonObject* MonObjectCreator::createMonObjectWithPrefix(const std::string &monObjectPrefix, 
                                                       IMessageSvc*  msgSvc, const std::string &source){
  MonObject* monObject=0;
  if (monObjectPrefix.compare(s_pfixMonObject) == 0) monObject = new MonObject(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH1F) == 0) monObject = new MonH1F(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH1D) == 0) monObject = new MonH1D(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH2F) == 0) monObject = new MonH2F(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH2D) == 0) monObject = new MonH2D(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonProfile) == 0) monObject = new MonProfile(msgSvc, source);
  return monObject;
}

int MonObjectCreator::monObjectSize(MonObject* monObject){
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  monObject->save(oa, monObject->mon_version());
  return m_ss.str().length();
}

