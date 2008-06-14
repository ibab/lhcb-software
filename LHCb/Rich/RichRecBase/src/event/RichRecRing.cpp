//-----------------------------------------------------------------------------
/** @file RichRecRing.cpp
 *
 *  Implementation file for class : LHCb::RichRecRing
 *
 *  CVS Log :-
 *  $Id: RichRecRing.cpp,v 1.4 2008-06-14 10:39:46 jonrob Exp $
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
    for ( RichRecPixelOnRing::Vector::const_iterator iP = richRecPixels().begin();
          iP != richRecPixels().end(); ++iP )
    {
      prob += (*iP).associationProb();
    }
    prob /= (double)richRecPixels().size();
  }
  return prob;
}
