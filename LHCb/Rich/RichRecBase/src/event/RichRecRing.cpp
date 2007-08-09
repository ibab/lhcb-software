
//-----------------------------------------------------------------------------
/** @file RichRecRing.cpp
 *
 *  Implementation file for class : LHCb::RichRecRing
 *
 *  CVS Log :-
 *  $Id: RichRecRing.cpp,v 1.1 2007-08-09 15:51:12 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   23/07/2007
 */
//-----------------------------------------------------------------------------

// STL
#include <cmath>
#include <iostream>

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// Event model
#include "Event/RichRecRing.h"

//-------------------------------------------------------------------------------------

const LHCb::RichRecPointOnRing*
LHCb::RichRecRing::getPointClosestInAzimuth(const double angle) const
{
  const LHCb::RichRecPointOnRing* point =  NULL;
  if ( !ringPoints().empty() )
  {

    // Increment in phi
    const double incPhi = Gaudi::Units::twopi / static_cast<double>(ringPoints().size());

    // which point
    unsigned int iPoint = (int)((angle/incPhi) - 0.5);
    if ( iPoint >= ringPoints().size() ) iPoint = ringPoints().size()-1;

    // set the point
    point = &ringPoints()[iPoint];

    /*

    // loop over the ring points and find nearest
    // could probably do this quicker ??
    double phiDiff(999999);
    for ( LHCb::RichRecPointOnRing::Vector::const_iterator iP = ringPoints().begin();
          iP != ringPoints().end(); ++iP )
    {
      const double tmpAngle = Gaudi::Units::pi + 
                              atan2( centrePointLocal().y() - (*iP).localPosition().y(),
                                     centrePointLocal().x() - (*iP).localPosition().x() );
      const double diff =
        fabs( angle - tmpAngle );
      if ( diff < phiDiff )
      {
        phiDiff = diff;
        point = &(*iP);
      }
      else if ( point ) { break; }
    }

    */

  }
  return point;
}

//=====================================================================================
