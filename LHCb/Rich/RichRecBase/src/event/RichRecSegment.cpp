
//-----------------------------------------------------------------------------
/** @file RichRecSegment.cpp
 *
 *  Implementation file for class : RichRecSegment
 *
 *  CVS Log :-
 *  $Id: RichRecSegment.cpp,v 1.9 2005-11-15 13:32:16 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecSegment.h"

// Reset information to force recalculation
void RichRecSegment::reset()
{
  setTrackInfo(0);

  nEmittedPhotons().resetData(0);
  nDetectablePhotons().resetData(0);
  nSignalPhotons().resetData(0);
  nScatteredPhotons().resetData(0);
  geomEfficiency().resetData(0);
  averageCKTheta().resetData(0);
  averageCKRadiusLocal().resetData(0);
  geomEfficiencyScat().resetData(0);
  m_avSaturatedRadiusLocal = -999;

  PDGeomEffsVector resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerPD( resetVector3 );

  emittedPhotonSpectra().reset();
  detectablePhotonSpectra().reset();
  signalPhotonSpectra().reset();

  richRecTrack()->reset();
}
