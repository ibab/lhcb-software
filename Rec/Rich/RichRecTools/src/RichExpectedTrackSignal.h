
//-----------------------------------------------------------------------------
/** @file RichExpectedTrackSignal.h
 *
 *  Header file for tool : RichExpectedTrackSignal
 *
 *  CVS Log :-
 *  $Id: RichExpectedTrackSignal.h,v 1.10 2005-06-23 15:17:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

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
#include "RichRecBase/IRichGasQuartzWindowAbs.h"

//-----------------------------------------------------------------------------
/** @class RichExpectedTrackSignal RichExpectedTrackSignal.h
 *
 *  Tool to calculate the expected signals for given 
 *  RichRecSegments and RichRecTracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichExpectedTrackSignal : public RichRecToolBase,
                                virtual public IRichExpectedTrackSignal {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichExpectedTrackSignal( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  /// Destructor
  virtual ~RichExpectedTrackSignal() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Expected number of observable signal photons for given track and hypothesis
  double nSignalPhotons ( RichRecTrack * track,
                          const Rich::ParticleIDType id ) const;

  // Expected number of signal photons for given segment and hypothesis
  double nSignalPhotons ( RichRecSegment * segment,
                          const Rich::ParticleIDType id ) const;

  // Expected number of observable signal photons for given track and hypothesis
  double nObservableSignalPhotons ( RichRecTrack * track,
                                    const Rich::ParticleIDType id ) const;

  // Expected number of observable signal photons for given segment and hypothesis
  double nObservableSignalPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const;

  // Expected number of scattered photons for given track and hypothesis
  double nScatteredPhotons ( RichRecTrack * track,
                             const Rich::ParticleIDType id ) const;

  // Expected number of scattered photons for given segment and hypothesis
  double nScatteredPhotons ( RichRecSegment * segment,
                             const Rich::ParticleIDType id ) const;

  // Expected number of observable scattered photons for given track and hypothesis
  double nObservableScatteredPhotons ( RichRecTrack * track,
                                       const Rich::ParticleIDType id ) const;

  // Expected number of observable scattered photons for given segment and hypothesis
  double nObservableScatteredPhotons ( RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const;

  // Expected number of observable signal and scattered photons for
  //  the given track and hypothesis 
  double nTotalObservablePhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id ) const;

  // Expected number of observable signal+scattered photons for given segment and hypothesis
  double nTotalObservablePhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) const;

  // Expected number of emitted photons for given track and hypothesis
  double nEmittedPhotons ( RichRecTrack * track,
                           const Rich::ParticleIDType id ) const;

  // Expected number of emitted photons for given segment and hypothesis
  double nEmittedPhotons ( RichRecSegment * segment,
                           const Rich::ParticleIDType id ) const;

  // Expected number of emitted photons for given segment and hypothesis,
  //  scaled by the HPD quantum efficiency 
  double nDetectablePhotons ( RichRecTrack * track,
                              const Rich::ParticleIDType id ) const;

  //  Expected number of emitted photons for given segment and hypothesis,
  //  scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

  // The average energy of signal Cherenkov photons
  double avgSignalPhotEnergy( RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

  // The average energy of emitted Cherenkov photons
  double avgEmitPhotEnergy( RichRecSegment * segment,
                            const Rich::ParticleIDType id ) const;
  
  // Is it possible for this track to give Rich information
  bool hasRichInfo( RichRecTrack * track ) const;

  // Is it geometrically possible for this segment to give Rich information
  bool hasRichInfo( RichRecSegment * segment ) const;

  // Is this track above threshold for a given particle type
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type ) const;

  // Is this track above threshold for a given particle type in a given radiator
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type,
                       const Rich::RadiatorType radiator ) const;

  // Is this segment above threshold for a given particle hypothesis
  bool aboveThreshold( RichRecSegment * segment,
                       const Rich::ParticleIDType type ) const;

  // Set the threshold information in a RichPID object for given track
  void setThresholdInfo( RichRecTrack * track, RichPID * pid ) const;

  // Set the threshold information in a RichPID object for given segment
  void setThresholdInfo( RichRecSegment * segment, RichPID * pid ) const;

  // Is this track active in this radiator medium for given particle id
  bool activeInRadiator( RichRecTrack * track,
                         const Rich::RadiatorType rad,
                         const Rich::ParticleIDType id = Rich::Electron ) const;

private:  // Private data

  // Pointers to tool instances
  const IRichGeomEff * m_geomEff;                  ///< Geometrical efficiency tool
  const IRichSellmeirFunc * m_sellmeir;            ///< Sellmeir function
  const IRichSignalDetectionEff * m_sigDetEff;     ///< signal detection efficiency
  const IRichParticleProperties * m_richPartProp;  ///< General particle properties
  const IRichRayleighScatter * m_rayScat;          ///< Rayleigh scattering function
  const IRichGasQuartzWindowAbs * m_gasQuartzWin;  ///< Absorption at the quartz window

};

#endif // RICHRECTOOLS_RICHEXPECTEDTRACKSIGNAL_H
