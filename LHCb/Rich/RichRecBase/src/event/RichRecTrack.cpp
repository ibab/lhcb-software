// $Id: RichRecTrack.cpp,v 1.4 2004-06-03 16:19:42 jonesc Exp $
// Include files 

// local
#include "Event/RichRecTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecTrack
//
// 2003-05-10 : Chris Jones
//-----------------------------------------------------------------------------

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
