// $Id: RichRecSegment.cpp,v 1.1 2003-06-30 15:11:58 jonrob Exp $
// Include files 

// local
#include "Event/RichRecSegment.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecSegment
//
// 2003-05-10 : Chris Jones
//-----------------------------------------------------------------------------

// Reset information to force recalculation
void RichRecSegment::reset() {

  setHitDetectorRegions( 0 );

  std::vector<float> resetVector1( Rich::NParticleTypes, -1.0 );
  setNEmittedPhotons    ( resetVector1 );
  setNDetectablePhotons ( resetVector1 );
  setNSignalPhotons     ( resetVector1 );
  setNScatteredPhotons  ( resetVector1 );
  setGeomEfficiency     ( resetVector1 );
  setAverageCKTheta     ( resetVector1 );
  setGeomEfficiencyScat ( resetVector1 );

  std::vector< std::vector<float> > resetVector2( Rich::NParticleTypes );
  setNEmitPhotsPerEnBin( resetVector2 );

  std::vector< RichMap<int,float> > resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerHPD( resetVector3 );

  // reset parent RichRecTrack
  richRecTrack()->reset();

}
