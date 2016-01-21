
//-----------------------------------------------------------------------------
/** @file RichRecRing.cpp
 *
 *  Implementation file for class : LHCb::RichRecRing
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   12/06/2008
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecRing.h"

double LHCb::RichRecRing::averagePixelProb() const
{
  double prob(0);
  if ( !richRecPixels().empty() )
  {
    for ( const auto& pix : richRecPixels() ) { prob += pix.associationProb(); }
    prob /= (double)richRecPixels().size();
  }
  return prob;
}

LHCb::RichRecRing::ClosestPoints
LHCb::RichRecRing::getPointsClosestInAzimuth(const float angle) const
{
  // return object
  LHCb::RichRecRing::ClosestPoints points(nullptr,nullptr);

  // Do we have enough rings to work with ?
  const auto nPoints = ringPoints().size();
  if ( nPoints > 1 )
  {
    // cache 1 / 2pi
    constexpr auto oneOverTwoPi = 1.0 / Gaudi::Units::twopi;

    // compute index in array based on angle (assumes constant seperation).
    const auto iPoint
      = (unsigned int) ( ( angle * (float)(nPoints) * oneOverTwoPi ) - 0.5 );
    
    // Find the lower edge point
    points.first  = &ringPoints()[ iPoint < nPoints ? iPoint : nPoints-1 ];

    // ... and the next point on
    points.second = &ringPoints()[ iPoint < nPoints-1 ? iPoint+1 : 0 ];
  }

  // return found points
  return points;
}
