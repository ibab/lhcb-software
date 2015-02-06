
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
    for ( const auto& P : richRecPixels() ) { prob += P.associationProb(); }
    prob /= (double)richRecPixels().size();
  }
  return prob;
}
