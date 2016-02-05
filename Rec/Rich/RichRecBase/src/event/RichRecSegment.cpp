
//-----------------------------------------------------------------------------
/** @file RichRecSegment.cpp
 *
 *  Implementation file for class : RichRecSegment
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecSegment.h"

// Reset information to force recalculation
void LHCb::RichRecSegment::reset()
{
  setTrackInfo(0);

  if ( m_nEmittedPhotons.objectExists()      ) nEmittedPhotons().resetData(0);
  if ( m_nDetectablePhotons.objectExists()   ) nDetectablePhotons().resetData(0);
  if ( m_nSignalPhotons.objectExists()       ) nSignalPhotons().resetData(0);
  if ( m_nScatteredPhotons.objectExists()    ) nScatteredPhotons().resetData(0);
  if ( m_geomEfficiency.objectExists()       ) geomEfficiency().resetData(0);
  if ( m_averageCKTheta.objectExists()       ) averageCKTheta().resetData(0);
  if ( m_averageCKRadiusLocal.objectExists() ) averageCKRadiusLocal().resetData(0);
  if ( m_geomEfficiencyScat.objectExists()   ) geomEfficiencyScat().resetData(0);
  m_avSaturatedRadiusLocal = -999;

  const PDGeomEffsVector resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerPD( resetVector3 );

  emittedPhotonSpectra().reset();
  detectablePhotonSpectra().reset();
  signalPhotonSpectra().reset();

  richRecTrack()->reset();
}
