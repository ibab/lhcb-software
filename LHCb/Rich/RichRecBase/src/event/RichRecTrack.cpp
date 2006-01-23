
//-----------------------------------------------------------------------------
/** @file RichRecTrack.cpp
 *
 *  Implementation file for class : RichRecTrack
 *
 *  CVS Log :-
 *  $Id: RichRecTrack.cpp,v 1.6 2006-01-23 14:08:55 jonrob Exp $
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
