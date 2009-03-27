
//-----------------------------------------------------------------------------
/** @file RichGeomPhoton.cpp
 *
 *  Implementation file for class : RichGeomPhoton
 *
 *  CVS Log :-
 *  $Id: RichGeomPhoton.cpp,v 1.6 2009-03-27 14:06:43 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @author  Antonis Papanestis
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichGeomPhoton.h"

// DetElems
#include "RichDet/DeRichSphMirror.h"

std::ostream & LHCb::RichGeomPhoton::fillStream ( std::ostream& s ) const
{
  s << "{ "
    << " cherenkovTheta:\t"        << CherenkovTheta()         << std::endl
    << " cherenkovPhi:\t"          << CherenkovPhi()           << std::endl
    << " emissionPoint:\t"         << emissionPoint()          << std::endl
    << " detectionPoint:\t"        << detectionPoint()         << std::endl
    << " sphMirReflectionPoint:\t" << sphMirReflectionPoint()  << std::endl
    << " flatMirReflectionPoint:\t"<< flatMirReflectionPoint() << std::endl
    << " primaryMirror:\t";
  if ( primaryMirror()   ) { s << primaryMirror()->mirrorNumber(); }
  else                     { s << "Undefined"; }
  s << std::endl;
  s << " secondaryMirror:\t";
  if ( secondaryMirror() ) { s << secondaryMirror()->mirrorNumber(); }
  else                     { s << "Undefined"; }
  s << std::endl;
  s << " smartID:\t"               << smartID()                << std::endl
    << " activeSegmentFraction:\t" << activeSegmentFraction()  << " } ";
  return s;
}
