// $Id: RichRecTrack.cpp,v 1.2 2003-08-06 09:55:32 jonrob Exp $
// Include files 

// local
#include "Event/RichRecTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecTrack
//
// 2003-05-10 : Chris Jones
//-----------------------------------------------------------------------------

// Reset information to force recalculation
void RichRecTrack::reset() {
 
  m_nEmittedPhotons.resetData();
  m_nDetectablePhotons.resetData();
  m_nSignalPhotons.resetData();
  m_nObservableSignalPhotons.resetData();
  m_nScatteredPhotons.resetData();
  m_nObservableScatteredPhotons.resetData();

  setCurrentHypothesis( Rich::Pion );

}
