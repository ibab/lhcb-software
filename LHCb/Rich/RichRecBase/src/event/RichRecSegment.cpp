// $Id: RichRecSegment.cpp,v 1.3 2003-10-13 16:10:55 jonrob Exp $
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

  m_nEmittedPhotons.resetData();
  m_nDetectablePhotons.resetData();
  m_nSignalPhotons.resetData();
  m_nScatteredPhotons.resetData();
  m_geomEfficiency.resetData();
  m_averageCKTheta.resetData();
  m_geomEfficiencyScat.resetData();

  std::vector< RichMap<int,double> > resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerPD( resetVector3 );

  m_emittedPhotonSpectra.reset();
  m_detectablePhotonSpectra.reset();
  m_signalPhotonSpectra.reset();

  // reset parent RichRecTrack
  richRecTrack()->reset();

}
