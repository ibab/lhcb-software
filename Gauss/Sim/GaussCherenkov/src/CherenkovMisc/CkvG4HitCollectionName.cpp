#include "GaussCherenkov/CkvG4HitCollName.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"


CkvG4HitCollName::CkvG4HitCollName() {

  m_NumberOfHCollectionInRICH=4;
  m_NumberOfStdHCollectionInRICH=m_NumberOfHCollectionInRICH;

  m_RichHCName.reserve(m_NumberOfHCollectionInRICH);
  m_RichHCName.push_back("Rich1TopHC");
  m_RichHCName.push_back("Rich1BotHC");
  m_RichHCName.push_back("Rich2LeftHC");
  m_RichHCName.push_back("Rich2RightHC");
  m_InitCollectListNumForOutput=0;
  m_FinalCollectListNumForOutput=m_NumberOfHCollectionInRICH-1;  
  m_InitCollectListNumForStdPmtOutput=0;
  m_FinalCollectListNumForStdPmtOutput=m_NumberOfHCollectionInRICH-1;

   CkvGeometrySetupUtil * aCkvGeometrySetup= CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
   bool aRich2UseGrandPmt= aCkvGeometrySetup ->Rich2_UseGrandPmt();
   if(aRich2UseGrandPmt)setCollConfigWithMixedPmtSet();
   

}
void CkvG4HitCollName::setCollConfigWithMixedPmtSet(){
   m_NumberOfHCollectionInRICH=6;
   m_NumberOfStdHCollectionInRICH=4;
   m_NumberOfGrandHCollectionInRICH=2;
   m_RichHCName.clear();
   m_RichHCName.reserve(m_NumberOfHCollectionInRICH);
   m_RichHCName.push_back("Rich1TopHC");
   m_RichHCName.push_back("Rich1BotHC");
   m_RichHCName.push_back("Rich2LeftHC");
   m_RichHCName.push_back("Rich2RightHC");
 
   m_RichHCName.push_back("Rich2LeftGrandHC");
   m_RichHCName.push_back("Rich2RightGrandHC");

   m_InitCollectListNumForOutput=0;
   m_FinalCollectListNumForOutput=m_NumberOfHCollectionInRICH-1; 
   m_InitCollectListNumForStdPmtOutput=0;
   m_FinalCollectListNumForStdPmtOutput=3;
   m_InitCollectListNumForGrandPmtOutput=4;
   m_FinalCollectListNumForGrandPmtOutput=5;
   
 
   UseGrandPmtFlag(true);
   
 
}

CkvG4HitCollName::~CkvG4HitCollName() {; }


