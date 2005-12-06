
//-----------------------------------------------------------------------------
/** @file MCRichTrack.cpp
 *
 *  Implementation file for class : MCRichTrack
 *
 *  $Id: MCRichTrack.cpp,v 1.1 2005-12-06 13:53:35 jonrob Exp $
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
  for ( SmartRefVector<MCRichSegment>::const_iterator iSeg = mcSegments().begin();
        iSeg != mcSegments().end(); ++iSeg ) {
    if ( *iSeg && rad == (*iSeg)->radiator() ) return *iSeg;
  }
  return NULL;
}
