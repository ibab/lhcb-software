// $Id: RichG4Counters.cpp,v 1.1 2003-06-19 08:17:52 seaso Exp $
// Include files 



// local
#include "RichG4Counters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4Counters
//
// 2003-06-07 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4Counters* RichG4Counters::RichG4CountersInstance=0;

RichG4Counters::RichG4Counters(  ) {

}
RichG4Counters::~RichG4Counters(  ) {
}
RichG4Counters* RichG4Counters::getInstance()
{
  if(RichG4CountersInstance == 0 ){
    RichG4CountersInstance= new RichG4Counters();
  }

  return  RichG4CountersInstance;
  

}
void RichG4Counters::InitRichEventCounters()
{
   m_NumPhotProdRich1Gas = 0 ;
   m_NumPhotGasOnRich1Mirror1 = 0 ;
   m_NumPhotGasOnRich1Mirror2 = 0;
   m_NumPhotGasOnRich1GasQW = 0;
   m_NumPhotGasOnRich1HpdQW = 0;
   m_NumPhotGasRich1BeforeQE = 0;
   m_NumPhotGasRich1AfterQE = 0;
   m_NumPhotGasRich1SiDet=0;

   m_NumPhotProdRich1Agel =0;   
   m_NumPhotAgelAtAgelDownstrZ=0;   
   m_NumPhotAgelOnRich1Mirror1=0;   
   m_NumPhotAgelOnRich1Mirror2=0;
   m_NumPhotAgelOnRich1GasQW =0;
   m_NumPhotAgelOnRich1HpdQW =0;
   m_NumPhotAgelRich1BeforeQE =0;
   m_NumPhotAgelRich1AfterQE = 0;
   m_NumPhotAgelRich1SiDet =0;
   

   m_NumHitTotRich1Gas =0;   
   m_NumHitTotRich1Agel =0;   
   m_NumHitSaturatedRich1Gas =0;   
   m_NumHitSaturatedRich1Agel =0;
   m_NumHitPartGunPrimaryPartRich1Gas=0;
   m_NumHitPartGunPrimaryPartRich1Agel=0;
   m_NumHitTotRich1All =0;
   
 
   
}


//=============================================================================
