
//-----------------------------------------------------------------------------
/** @file RichRecTrack.cpp
 *
 *  Implementation file for class : RichRecTrack
 *
 *  CVS Log :-
 *  $Id: RichRecTrack.cpp,v 1.5 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecTrack.h"

// Reset information to force recalculation
void RichRecTrack::reset()
{
  m_nEmittedPhotons.resetData(0);
  m_nDetectablePhotons.resetData(0);
  m_nSignalPhotons.resetData(0);
  m_nObservableSignalPhotons.resetData(0);
  m_nScatteredPhotons.resetData(0);
  m_nObservableScatteredPhotons.resetData(0);
  setCurrentHypothesis( Rich::Pion );
}
