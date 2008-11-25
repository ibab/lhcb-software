#include "Gaucho/MonObjectCreator.h"
#include "Gaucho/MonH1F.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2F.h"
#include "Gaucho/MonH2D.h"
#include "Gaucho/MonBool.h"
#include "Gaucho/MonInt.h"
#include "Gaucho/MonLong.h"
#include "Gaucho/MonFloat.h"
#include "Gaucho/MonString.h"
#include "Gaucho/MonDouble.h"
#include "Gaucho/MonPairDD.h"
#include "Gaucho/MonPairII.h"
#include "Gaucho/MonPairDI.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonHitMap2D.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/MonVectorI.h"
#include "Gaucho/MonVectorD.h"
#include "Gaucho/MonRate.h"

MonObjectCreator::MonObjectCreator()
{
}

MonObjectCreator::~MonObjectCreator(){
}


MonObject* MonObjectCreator::createMonObject(const std::string &monObjectTypeName, IMessageSvc*  msgSvc, const std::string &source){
  MonObject* monObject=0;
  if (monObjectTypeName.compare(s_monObject) == 0) monObject = new MonObject(msgSvc, source);
  if (monObjectTypeName.compare(s_monH1F) == 0) monObject = new MonH1F(msgSvc, source);
  if (monObjectTypeName.compare(s_monH1D) == 0) monObject = new MonH1D(msgSvc, source);
  if (monObjectTypeName.compare(s_monH2F) == 0) monObject = new MonH2F(msgSvc, source);
  if (monObjectTypeName.compare(s_monH2D) == 0) monObject = new MonH2D(msgSvc, source);
  if (monObjectTypeName.compare(s_monProfile) == 0) monObject = new MonProfile(msgSvc, source);
  if (monObjectTypeName.compare(s_monInt) == 0) monObject = new MonInt(msgSvc, source);
  if (monObjectTypeName.compare(s_monDouble) == 0) monObject = new MonDouble(msgSvc, source);
  if (monObjectTypeName.compare(s_monFloat) == 0) monObject = new MonFloat(msgSvc, source);
  if (monObjectTypeName.compare(s_monString) == 0) monObject = new MonString(msgSvc, source);
  if (monObjectTypeName.compare(s_monLong) == 0) monObject = new MonLong(msgSvc, source);
  if (monObjectTypeName.compare(s_monBool) == 0) monObject = new MonBool(msgSvc, source);
  if (monObjectTypeName.compare(s_monPairDD) == 0) monObject = new MonPairDD(msgSvc, source);
  if (monObjectTypeName.compare(s_monPairII) == 0) monObject = new MonPairII(msgSvc, source);
  if (monObjectTypeName.compare(s_monPairDI) == 0) monObject = new MonPairDI(msgSvc, source);
  //  if (monObjectTypeName.compare(s_monHitMap2D) == 0) monObject = new MonHitMap2D(msgSvc, source);
  if (monObjectTypeName.compare(s_monStatEntity) == 0) monObject = new MonStatEntity(msgSvc, source);
  if (monObjectTypeName.compare(s_monVectorI) == 0) monObject = new MonVectorI(msgSvc, source);
  if (monObjectTypeName.compare(s_monVectorD) == 0) monObject = new MonVectorD(msgSvc, source);
  if (monObjectTypeName.compare(s_monRate) == 0) monObject = new MonRate(msgSvc, source);
  return monObject;
}

MonObject* MonObjectCreator::createMonObjectWithPrefix(const std::string &monObjectPrefix, IMessageSvc*  msgSvc, const std::string &source){
  MonObject* monObject=0;
  if (monObjectPrefix.compare(s_pfixMonObject) == 0) monObject = new MonObject(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH1F) == 0) monObject = new MonH1F(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH1D) == 0) monObject = new MonH1D(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH2F) == 0) monObject = new MonH2F(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonH2D) == 0) monObject = new MonH2D(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonProfile) == 0) monObject = new MonProfile(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonInt) == 0) monObject = new MonInt(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonDouble) == 0) monObject = new MonDouble(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonFloat) == 0) monObject = new MonFloat(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonString) == 0) monObject = new MonString(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonLong) == 0) monObject = new MonLong(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonBool) == 0) monObject = new MonBool(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonPairDD) == 0) monObject = new MonPairDD(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonPairII) == 0) monObject = new MonPairII(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonPairDI) == 0) monObject = new MonPairDI(msgSvc, source);
  //  if (monObjectPrefix.compare(s_pfixMonHitMap2D) == 0) monObject = new MonHitMap2D(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonStatEntity) == 0) monObject = new MonStatEntity(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonVectorI) == 0) monObject = new MonVectorI(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonVectorD) == 0) monObject = new MonVectorD(msgSvc, source);
  if (monObjectPrefix.compare(s_pfixMonRate) == 0) monObject = new MonRate(msgSvc, source);
  return monObject;
}

int MonObjectCreator::monObjectSize(MonObject* monObject){
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  monObject->save(oa, monObject->version());
  return m_ss.str().length();
}

