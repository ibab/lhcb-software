// $Id: RichExpectedTrackSignal.h,v 1.3 2003-08-26 14:40:18 jonrob Exp $
#ifndef RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
//#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/RichRecTrack.h"
#include "Event/RichPID.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichRecBase/IRichParticleProperties.h"
#include "RichRecBase/IRichRayleighScatter.h"

/** @class RichExpectedTrackSignal RichExpectedTrackSignal.h
 *
 *  Tool to calculate the expected signals for given 
 *  RichRecSegments and RichRecTracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichExpectedTrackSignal : public RichRecToolBase,
                                virtual public IRichExpectedTrackSignal {

public:

  /// Standard constructor
  RichExpectedTrackSignal( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  /// Destructor
  virtual ~RichExpectedTrackSignal() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Expected number of observable signal photons for given track and hypothesis
  double nSignalPhotons ( RichRecTrack * track,
                          const Rich::ParticleIDType id );

  /// Expected number of signal photons for given segment and hypothesis
  double nSignalPhotons ( RichRecSegment * segment,
                          const Rich::ParticleIDType id );

  /// Expected number of observable signal photons for given track and hypothesis
  double nObservableSignalPhotons ( RichRecTrack * track,
                                    const Rich::ParticleIDType id );

  /// Expected number of observable signal photons for given segment and hypothesis
  double nObservableSignalPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType id );

  /// Expected number of scattered photons for given track and hypothesis
  double nScatteredPhotons ( RichRecTrack * track,
                             const Rich::ParticleIDType id );

  /// Expected number of scattered photons for given segment and hypothesis
  double nScatteredPhotons ( RichRecSegment * segment,
                             const Rich::ParticleIDType id );

  /// Expected number of observable scattered photons for given track and hypothesis
  double nObservableScatteredPhotons ( RichRecTrack * track,
                                       const Rich::ParticleIDType id );

  /// Expected number of observable scattered photons for given segment and hypothesis
  double nObservableScatteredPhotons ( RichRecSegment * segment,
                                       const Rich::ParticleIDType id );

  /// Expected number of observable signal+scattered photons for
  /// given track and hypothesis
  double nTotalObservablePhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id );

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  double nTotalObservablePhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given track and hypothesis
  double nEmittedPhotons ( RichRecTrack * track,
                           const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis
  double nEmittedPhotons ( RichRecSegment * segment,
                           const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecTrack * track,
                              const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecSegment * segment,
                              const Rich::ParticleIDType id );

  /// Is it possible for this track to give Rich information
  bool hasRichInfo( RichRecTrack * track );

  /// Is it geometrically possible for this segment to give Rich information
  bool hasRichInfo( RichRecSegment * segment );

  /// Is this track above threshold for a given particle type
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type );

  /// Is this track above threshold for a given particle type in a given radiator
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type,
                       const Rich::RadiatorType radiator );

  /// Is this segment above threshold for a given particle hypothesis
  bool aboveThreshold( RichRecSegment * segment,
                       const Rich::ParticleIDType type );

  /// Set the threshold information in a RichPID object for given track
  void setThresholdInfo( RichRecTrack * track, RichPID * pid );

  /// Set the threshold information in a RichPID object for given segment
  void setThresholdInfo( RichRecSegment * segment, RichPID * pid );

  /// Is this track active in this radiator medium for given particle id
  bool activeInRadiator( RichRecTrack * track,
                         const Rich::RadiatorType rad,
                         const Rich::ParticleIDType id = Rich::Electron );

private:  // Private data

  /// Pointers to tool instances
  IRichGeomEff * m_geomEff;
  IRichSellmeirFunc * m_sellmeir;
  IRichSignalDetectionEff * m_sigDetEff;
  IRichParticleProperties * m_richPartProp;
  IRichRayleighScatter * m_rayScat;

};

#endif // RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
