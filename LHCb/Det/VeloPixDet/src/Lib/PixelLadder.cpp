// $Id: PixelLadder.cpp,v 1.3 2009-10-21 11:19:29 cocov Exp $
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

//========================== Access to the positions of the readout edge (-1 or +1)
// for a chip having its readout edge on the left side it will be -1 for an orientation along X
// for a chip having its readout edge on the top side it will be +1 for an orientation along Y
// in a ladder we do the assumption that all orientation are the same

std::vector<int> PixelLadder::edgesOrientation() const
{
  return m_edgesOrientation;
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

//========================== Set the positions of the readout edge
void PixelLadder::setEdgesOrientation( std::vector< int > orientation )
{
  m_edgesOrientation.clear();
  for (unsigned int i = 0 ; i < orientation.size() ; i++ )
  {
    m_edgesOrientation.push_back(orientation[i]);
  }
}
