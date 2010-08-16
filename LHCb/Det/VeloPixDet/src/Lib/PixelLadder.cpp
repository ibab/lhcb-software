// $Id: PixelLadder.cpp,v 1.4 2009-10-28 13:02:28 cocov Exp $
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

PixelLadder::PixelLadder(Gaudi::XYZPoint pRef ,int n , bool isHoriz )
{
   m_pRef = pRef;
   m_NChip = n;
   m_horizontalChip = isHoriz;
   for (int i = 0 ; i < n ; i++){
     m_edgesOrientation.push_back(0);
   }
}

//=============================================================================
// Destructor
//=============================================================================
PixelLadder::~PixelLadder() {}

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

//========================== Set the orientation of the ladder 
//horizontal means the chips are aligned along the x axis, vertical means the chips are along the y axis
void PixelLadder::setIsHorizontal( bool isHoriz  )
{
  m_horizontalChip = isHoriz;
}

//========================== Set the positions of the readout edge
void PixelLadder::setEdgesOrientation( std::vector< int > orientation )
{
  for (unsigned int i = 0 ; i < orientation.size() ; i++ )
  {
    m_edgesOrientation[i] = orientation[i] ;
  }
}
