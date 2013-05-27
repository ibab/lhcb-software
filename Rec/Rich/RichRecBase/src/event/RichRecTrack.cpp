
//-----------------------------------------------------------------------------
/** @file RichRecTrack.cpp
 *
 *  Implementation file for class : RichRecTrack
 *
 *  CVS Log :-
 *  $Id: RichRecTrack.cpp,v 1.7 2006-12-02 15:24:33 jonrob Exp $
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

  if ( m_nEmittedPhotons.objectExists()      ) nEmittedPhotons().resetData(0);
  if ( m_nDetectablePhotons.objectExists()   ) nDetectablePhotons().resetData(0);
  if ( m_nSignalPhotons.objectExists()       ) nSignalPhotons().resetData(0);
  if ( m_nScatteredPhotons.objectExists()    ) nScatteredPhotons().resetData(0);
  if ( m_nObservableSignalPhotons.objectExists()    ) nObservableSignalPhotons().resetData(0);
  if ( m_nObservableScatteredPhotons.objectExists() ) nObservableScatteredPhotons().resetData(0);

  setCurrentHypothesis( Rich::Pion );
}

LHCb::RichRecSegment* 
LHCb::RichRecTrack::segmentInRad(const Rich::RadiatorType rad) const
{
  LHCb::RichRecSegment* segment = NULL;
  for ( Segments::const_iterator iS = richRecSegments().begin();
        iS != richRecSegments().end(); ++iS )
  {
    if ( *iS && (*iS)->trackSegment().radiator() == rad ) { segment = *iS; break; }
  }
  return segment;
}
