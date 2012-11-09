// $Id: RichG4HpdReflectionFlag.cpp,v 1.3 2008-01-28 17:10:29 jonrob Exp $
// Include files 



// local
#include "GaussRICH/RichG4HpdReflectionFlag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HpdReflectionFlag
//
// 2007-10-30 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HpdReflectionFlag* RichG4HpdReflectionFlag::m_RichG4HpdReflectionFlagInstance=0;


RichG4HpdReflectionFlag::RichG4HpdReflectionFlag(  ) {


  m_HpdQwPCRefl=1;   // reflection at hpd Quartz-Photocathode
  m_HpdChromiumRefl=2; // reflection at hpd  Chromium
  m_HpdAirQwRefl=3;  // reflection at hpd  air-quartz
  m_HpdAirPCRefl=4;  //reflection at hpd air-photocathode
  m_HpdSiliconRefl=5; //reflection at hpd silicon surface
  m_HpdKovarRefl=6;  // reflection at hpd kovar
  m_HpdKaptonRefl=7; //  reflection at hpd Kapton

  m_HpdPCQwRefl=8;   // reflection at hpd Photocathode- Quartz

  m_MaxNumHpdReflFlag=8; // max number of flags.//rwl edit 22.01.08
  
}

RichG4HpdReflectionFlag*  RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance() {

  if(m_RichG4HpdReflectionFlagInstance == 0 ) {   
    m_RichG4HpdReflectionFlagInstance=new RichG4HpdReflectionFlag(); 
  }
  return m_RichG4HpdReflectionFlagInstance;
}
//=============================================================================
// Destructor
//=============================================================================
RichG4HpdReflectionFlag::~RichG4HpdReflectionFlag() { } 

//=============================================================================
