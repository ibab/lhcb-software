
//===============================================================================
/** @file RichTraceMode.cpp
 *
 *  Implementation file for utility class : RichTraceMode
 *
 *  CVS History :-
 *  $Id: RichTraceMode.cpp,v 1.1 2006-08-09 10:44:19 jonrob Exp $
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
  if ( forcedSide() )        { os << " [Will force given RICH side]"; }
  if ( outMirrorBoundary() ) { os << " [Will Respect Outside Mirror Boundaries]"; }
  if ( mirrorSegBoundary() ) { os << " [Will Respect Mirror Seg Boundaries]"; }
  return os << " ]";
}

//-----------------------------------------------------------------------------

