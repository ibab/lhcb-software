#include "RichG4HitCollName.h"


RichG4HitCollName::RichG4HitCollName() {

  m_NumberOfHCollectionInRICH=4;
  m_RichHCName.reserve(m_NumberOfHCollectionInRICH);
  m_RichHCName.push_back("Rich1TopHC");
  m_RichHCName.push_back("Rich1BotHC");
  m_RichHCName.push_back("Rich2TopHC");
  m_RichHCName.push_back("Rich2BotHC");

}
RichG4HitCollName::~RichG4HitCollName() {; }


