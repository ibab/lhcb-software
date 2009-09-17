// $Id: PixelLadder.cpp,v 1.1.1.1 2009-09-17 16:07:20 cocov Exp $
// Include files
// local
#define  VELOPIXDET_PIXELLADDER_CPP 1

#include "VeloPixDet/PixelLadder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PixelLadder
//
// 2009-07-13 : Coco Victor
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

PixelLadder::PixelLadder(Gaudi::XYZPoint pRef ,int n)
{
   m_pRef = pRef;
   m_NChip = n;
}

//=============================================================================
// Destructor
//=============================================================================
PixelLadder::~PixelLadder() {}

//=============================================================================

//========================== Access to the reference point coordinates
Gaudi::XYZPoint PixelLadder::ReferencePoint() const
{
  return m_pRef;
}

//========================== Access to the reference number of chip in the ladder
int PixelLadder::nChip() const
{
  return m_NChip;
}


//========================== Set the reference point coordinates
void PixelLadder::setReferencePoint( Gaudi::XYZPoint p)
{
  m_pRef = p;
}

//========================== Set the number of chip in the ladder
void PixelLadder::setNChip( int n )
{
  m_NChip = n;
}
