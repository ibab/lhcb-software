
//-----------------------------------------------------------------------------
/** @file RichRecSegment.cpp
 *
 *  Implementation file for class : RichRecSegment
 *
 *  CVS Log :-
 *  $Id: RichRecSegment.cpp,v 1.8 2005-02-02 10:01:20 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2004/07/26 18:00:58  jonrob
 *  Various improvements to the doxygen comments
 *
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

  m_nEmittedPhotons.resetData(0);
  m_nDetectablePhotons.resetData(0);
  m_nSignalPhotons.resetData(0);
  m_nScatteredPhotons.resetData(0);
  m_geomEfficiency.resetData(0);
  m_averageCKTheta.resetData(0);
  m_averageCKRadiusLocal.resetData(0);
  m_geomEfficiencyScat.resetData(0);
  m_avSaturatedRadiusLocal = -999;

  PDGeomEffsVector resetVector3( Rich::NParticleTypes );
  setGeomEfficiencyPerPD( resetVector3 );

  m_emittedPhotonSpectra.reset();
  m_detectablePhotonSpectra.reset();
  m_signalPhotonSpectra.reset();

  richRecTrack()->reset();
}
