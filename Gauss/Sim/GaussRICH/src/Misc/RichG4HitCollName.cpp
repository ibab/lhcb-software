#include "GaussRICH/RichG4HitCollName.h"


RichG4HitCollName::RichG4HitCollName() {

  m_NumberOfHCollectionInRICH=4;
  m_RichHCName.reserve(m_NumberOfHCollectionInRICH);
  m_RichHCName.push_back("Rich1TopHC");
  m_RichHCName.push_back("Rich1BotHC");
  m_RichHCName.push_back("Rich2LeftHC");
  m_RichHCName.push_back("Rich2RightHC");
  m_InitCollectListNumForOutput=0;
  m_FinalCollectListNumForOutput=m_NumberOfHCollectionInRICH-1;  
}
void RichG4HitCollName::setCollConfigWithSuperRich(){
   m_NumberOfHCollectionInRICH=2;
   m_RichHCName.clear();
   m_RichHCName.reserve(m_NumberOfHCollectionInRICH);
   m_RichHCName.push_back("SuperRichLeftHC");
   m_RichHCName.push_back("SuperRichRightHC");
   m_InitCollectListNumForOutput=0;
   m_FinalCollectListNumForOutput=m_NumberOfHCollectionInRICH-1;  
   setSuperRichFlag(true);
   
 
}

RichG4HitCollName::~RichG4HitCollName() {; }


