
//-----------------------------------------------------------------------------
/** @file MCRichTrack.cpp
 *
 *  Implementation file for class : MCRichTrack
 *
 *  $Id: MCRichTrack.cpp,v 1.2 2006-03-01 09:38:42 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2004-01-22
 */
//-----------------------------------------------------------------------------

// local
#include "Event/MCRichTrack.h"
#include "Event/MCRichSegment.h"

const LHCb::MCRichSegment * 
LHCb::MCRichTrack::segmentInRad( const Rich::RadiatorType rad ) const
{
  const LHCb::MCRichSegment * segment = 0;
  for ( SmartRefVector<MCRichSegment>::const_iterator iSeg = mcSegments().begin();
        iSeg != mcSegments().end(); ++iSeg ) 
  {
    if ( *iSeg && rad == (*iSeg)->radiator() ) 
    {
      if (segment != 0) 
      { std::cout << "WARNING : MCRichTrack has multiple segments for the same radiator !!" << std::endl; }
      segment = *iSeg;
    }
  }
  return segment;
}
