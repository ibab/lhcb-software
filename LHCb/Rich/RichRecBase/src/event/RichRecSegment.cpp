// $Id: RichRecSegment.cpp,v 1.5 2004-06-03 16:19:42 jonesc Exp $
// Include files 

// local
#include "Event/RichRecSegment.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecSegment
//
// 2003-05-10 : Chris Jones
//-----------------------------------------------------------------------------

// Reset information to force recalculation
void RichRecSegment::reset() 
{
  setHitDetectorRegions(0);

  m_nEmittedPhotons.resetData(0);
  m_nDetectablePhotons.resetData(0);
  m_nSignalPhotons.resetData(0);
  m_nScatteredPhotons.resetData(0);
  m_geomEfficiency.resetData(0);
  m_averageCKTheta.resetData(0);
  m_geomEfficiencyScat.resetData(0);

  PDGeomEffsVector resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerPD( resetVector3 );

  m_emittedPhotonSpectra.reset();
  m_detectablePhotonSpectra.reset();
  m_signalPhotonSpectra.reset();

  richRecTrack()->reset();
}
