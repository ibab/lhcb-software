// $Id: $
// Include files 



// local
#include "GaussCherenkov/CherenkovG4Counters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CherenkovG4Counters
//
// 2011-04-17 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CherenkovG4Counters* CherenkovG4Counters::CherenkovG4CountersInstance=0;

CherenkovG4Counters::CherenkovG4Counters(): 
m_RichPmtHitArray(std::vector<int> (302*16)),
m_NumPmtsRich1(120*16), 
m_NumPmtsRich2(182*16),
m_NumPmtInModuleRich(16),
m_NumPmtsSuperRich(3000) {
  // the dummy values above adjusted from DB 
}
//=============================================================================
// Destructor
//=============================================================================
CherenkovG4Counters::~CherenkovG4Counters() {} 

//=============================================================================
CherenkovG4Counters* CherenkovG4Counters::getInstance()
{
  if(  CherenkovG4CountersInstance == 0 ) {
    CherenkovG4CountersInstance = new  CherenkovG4Counters();
    
  }
  
  return CherenkovG4CountersInstance;
}

void CherenkovG4Counters::InitCherenkovG4Counters() 
{
  m_RichPmtHitArray.clear();
  m_RichPmtHitArray.resize(m_NumPmtsRich1+m_NumPmtsRich2);
  
}

void CherenkovG4Counters::InitCherenkovG4CountersSR() 
{
  m_RichPmtHitArray.clear();
  m_RichPmtHitArray.resize(m_NumPmtsSuperRich);
  
}

