
//-----------------------------------------------------------------------------
/** @file RichRecTrack.cpp
 *
 *  Implementation file for class : RichRecTrack
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecTrack.h"

// Reset information to force recalculation
void LHCb::RichRecTrack::reset()
{
  if ( m_nEmittedPhotons.objectExists()             ) nEmittedPhotons().resetData(0);
  if ( m_nDetectablePhotons.objectExists()          ) nDetectablePhotons().resetData(0);
  if ( m_nSignalPhotons.objectExists()              ) nSignalPhotons().resetData(0);
  if ( m_nScatteredPhotons.objectExists()           ) nScatteredPhotons().resetData(0);
  if ( m_nObservableSignalPhotons.objectExists()    ) nObservableSignalPhotons().resetData(0);
  if ( m_nObservableScatteredPhotons.objectExists() ) nObservableScatteredPhotons().resetData(0);
  setCurrentHypothesis( Rich::Pion );
}

LHCb::RichRecSegment* 
LHCb::RichRecTrack::segmentInRad(const Rich::RadiatorType rad) const
{
  LHCb::RichRecSegment* segment = NULL;
  for ( LHCb::RichRecSegment * S : richRecSegments() )
  {
    if ( S && S->trackSegment().radiator() == rad ) { segment = S; break; }
  }
  return segment;
}
