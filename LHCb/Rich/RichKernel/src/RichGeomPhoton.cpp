
//-----------------------------------------------------------------------------
/** @file RichGeomPhoton.cpp
 *
 *  Implementation file for class : RichGeomPhoton
 *
 *  CVS Log :-
 *  $Id: RichGeomPhoton.cpp,v 1.3 2007-04-23 12:44:04 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @author  Antonis Papanestis
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichGeomPhoton.h"

std::ostream & LHCb::RichGeomPhoton::fillStream ( std::ostream& s ) const
{
  s << "{ "
    << " cherenkovTheta:\t" << CherenkovTheta() << std::endl
    << " cherenkovPhi:\t" << CherenkovPhi() << std::endl
    << " emissionPoint:\t" << emissionPoint() << std::endl
    << " detectionPoint:\t" << detectionPoint() << std::endl
    << " sphMirReflectionPoint:\t"<<sphMirReflectionPoint()<<std::endl
    << " flatMirReflectionPoint:\t"<<flatMirReflectionPoint()<<std::endl
    << " sphMirNumber:\t" << sphMirrorNum()<<std::endl
    << " flatMirNumber:\t" << flatMirrorNum()<<std::endl
    << " pixelCluster:\t" << pixelCluster() << std::endl
    << " activeSegmentFraction:\t" << activeSegmentFraction() << " } ";
  return s;
}
