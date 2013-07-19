// $Id: RichG4HitCoordResult.cpp,v 1.1 2006-02-21 17:05:27 seaso Exp $
// Include files 



// local
#include "GaussRICH/RichG4HitCoordResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4HitCoordResult
//
// 2006-02-17 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4HitCoordResult::RichG4HitCoordResult(  ) 

{
  m_radiatorNum =-1;
  m_DetPtFromPixelNum = Gaudi::XYZPoint (0,0,0);
  m_DetPtFromTrueLocalHit =  Gaudi::XYZPoint (0,0,0);
  m_DetPtFromGlobalPhCathode = Gaudi::XYZPoint (0,0,0);
  m_DetPtInPhDetFromPixelNum = Gaudi::XYZPoint (0,0,0);
  m_DetPtInPhDetFromTrueLocalHit = Gaudi::XYZPoint (0,0,0);
  m_DetPtInPhDetFromGlobalPhCathode = Gaudi::XYZPoint (0,0,0);
  m_SphReflPtD2E1 = Gaudi::XYZPoint (0,0,0);
  m_SphReflPtD3E1 = Gaudi::XYZPoint (0,0,0);


}
//=============================================================================
// Destructor
//=============================================================================
RichG4HitCoordResult::~RichG4HitCoordResult() {} 

//=============================================================================
