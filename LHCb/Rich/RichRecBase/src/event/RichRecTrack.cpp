// $Id: RichRecTrack.cpp,v 1.1 2003-06-30 15:11:58 jonrob Exp $
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

  std::vector<float> resetVector1( Rich::NParticleTypes, -1.0 );
  setNEmittedPhotons             ( resetVector1 );
  setNDetectablePhotons          ( resetVector1 );
  setNSignalPhotons              ( resetVector1 );
  setNObservableSignalPhotons    ( resetVector1 );
  setNScatteredPhotons           ( resetVector1 );
  setNObservableScatteredPhotons ( resetVector1 );

  setCurrentHypothesis           ( Rich::Pion );

}
