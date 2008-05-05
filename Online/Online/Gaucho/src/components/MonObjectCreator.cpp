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

MonObjectCreator::MonObjectCreator()
{
}

MonObjectCreator::~MonObjectCreator(){
}


MonObject* MonObjectCreator::createMonObject(const std::string &monObjectTypeName, IMessageSvc*  msgSvc, const std::string &source){
  MonObject* monObject=0;
  if (monObjectTypeName == s_monObject) monObject = new MonObject(msgSvc, source);
  if (monObjectTypeName == s_monH1F) monObject = new MonH1F(msgSvc, source);
  if (monObjectTypeName == s_monH1D) monObject = new MonH1D(msgSvc, source);
  if (monObjectTypeName == s_monH2F) monObject = new MonH2F(msgSvc, source);
  if (monObjectTypeName == s_monH2D) monObject = new MonH2D(msgSvc, source);
  if (monObjectTypeName == s_monProfile) monObject = new MonProfile(msgSvc, source);
  if (monObjectTypeName == s_monInt) monObject = new MonInt(msgSvc, source);
  if (monObjectTypeName == s_monDouble) monObject = new MonDouble(msgSvc, source);
  if (monObjectTypeName == s_monFloat) monObject = new MonFloat(msgSvc, source);
  if (monObjectTypeName == s_monString) monObject = new MonString(msgSvc, source);
  if (monObjectTypeName == s_monLong) monObject = new MonLong(msgSvc, source);
  if (monObjectTypeName == s_monBool) monObject = new MonBool(msgSvc, source);
  if (monObjectTypeName == s_monPairDD) monObject = new MonPairDD(msgSvc, source);
  if (monObjectTypeName == s_monPairII) monObject = new MonPairII(msgSvc, source);
  if (monObjectTypeName == s_monPairDI) monObject = new MonPairDI(msgSvc, source);
  //  if (monObjectTypeName == s_monHitMap2D) monObject = new MonHitMap2D(msgSvc, source);
  if (monObjectTypeName == s_monStatEntity) monObject = new MonStatEntity(msgSvc, source);
  if (monObjectTypeName == s_monVectorI) monObject = new MonVectorI(msgSvc, source);
  if (monObjectTypeName == s_monVectorD) monObject = new MonVectorD(msgSvc, source);
  return monObject;
}

int MonObjectCreator::monObjectSize(MonObject* monObject){
  std::stringstream m_ss;
  m_ss.str("");
  boost::archive::binary_oarchive oa(m_ss);
  monObject->save(oa, monObject->version());
  return m_ss.str().length();
}

