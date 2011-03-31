
//===============================================================================
/** @file RichTraceMode.cpp
 *
 *  Implementation file for utility class : RichTraceMode
 *
 *  @author Chris Jones Christopher.Rob.Jones@cern.ch
 *  @date   05/08/2006
 */
//===============================================================================

// Include files

// local
#include "Kernel/RichTraceMode.h"

std::ostream& LHCb::RichTraceMode::fillStream(std::ostream& os) const
{
  os << "RichTraceMode[ DetectionPrecision=" << detPrecision()
     << " DetPlaneBoundaries=" << detPlaneBound();
  if ( forcedSide() )         { os << " [Will force given RICH side]"; }
  if ( outMirrorBoundary() )  { os << " [Will Respect Outside Mirror Boundaries]"; }
  if ( mirrorSegBoundary() )  { os << " [Will Respect Mirror Seg Boundaries]"; }
  if ( beamPipeIntersects() ) { os << " [Will Test for intersections with beampipe]"; }
  if ( hpdKaptonShadowing() ) { os << " [Will check for intersections with HPD kapton]"; }
  if ( aeroRefraction()     ) { os << " [Will correct for refraction at Aerogel/Rich1Gas boundary]"; }
  return os << " ]";
}

//-----------------------------------------------------------------------------
