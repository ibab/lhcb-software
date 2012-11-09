// $Id: RichInfo.cpp,v 1.2 2003-07-28 10:27:11 witoldp Exp $
// Include files 



// local
#include "GaussRICH/RichInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RICHInfo
//
// 2003-04-16 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichInfo::RichInfo(  ):m_RichPhotInformation(0),m_RichPEInformation(0),
       m_HasUserPhotInfo(false),m_HasUserPEInfo(false) {

}
RichInfo::~RichInfo( ) 
{
  delete m_RichPEInformation;
  delete m_RichPhotInformation;
}
RichInfo::RichInfo(RichPhotInfo* aRichPhotInfo):m_RichPEInformation(0),
  m_HasUserPEInfo(false)
{
  
  m_RichPhotInformation= aRichPhotInfo;
  m_HasUserPhotInfo= true;
  
  
}
RichInfo::RichInfo(RichPEInfo* aRichPEInfo):m_RichPhotInformation(0),
  m_HasUserPhotInfo(false) 
{
  m_RichPEInformation= aRichPEInfo;
  m_HasUserPEInfo= true; 
   
}


//=============================================================================
