// $Id: RichG4ReconResult.cpp,v 1.2 2004-02-10 14:24:14 jonesc Exp $
// Include files

// local
#include "GaussRICH/RichG4ReconResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4ReconResult
//
// 2003-10-28 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4ReconResult::RichG4ReconResult(  ) {

  InitReconResult();

}

RichG4ReconResult::~RichG4ReconResult(  ) {}

void RichG4ReconResult::InitReconResult()
{

  m_radiatorForCkv=-1;
  m_ckvAngleD1E1 = -0.1;
  m_ckvAngleD2E1 = -0.1;
  m_ckvAngleD3E1= -0.1;
  m_ckvAngleD3E2= -0.1;
  m_ckvAngleD1E2= -0.1;
  m_ckvAngleD1E3= -0.1;
  m_ckvAngleD2E3= -0.1;
  m_ckvAngleD3E3= -0.1;
  m_ckvAngleD4E1= -0.1;
  m_ckvAngleD4E3= -0.1;
  m_ckvAngleD4E2= -0.1;
  m_ckvAngleD4E4= -0.1;

   m_ckvAngleD5E4= -0.1;
   m_ckvAngleD5E1= -0.1;
   m_ckvAngleD6E1= -0.1;
   m_ckvAngleD7E1= -0.1;
  
   m_ckvAngleD5E3= -0.1;
   m_ckvAngleD6E3= -0.1;
   m_ckvAngleD7E3= -0.1;

}

//=============================================================================
