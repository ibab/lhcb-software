
//-----------------------------------------------------------------------------
/** @file IRichExpectedTrackSignal.h
 *
 *  Header file for RICH reconstruction tool interface : IRichExpectedTrackSignal
 *
 *  CVS Log :-
 *  $Id: IRichExpectedTrackSignal.h,v 1.5 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H 1

// Event model
class RichRecSegment;
class RichRecTrack;
class RichPID;

// Kernel
#include "RichKernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichExpectedTrackSignal( "IRichExpectedTrackSignal", 1, 0 );

/** @class IRichExpectedTrackSignal IRichExpectedTrackSignal.h
 *
 *  Interface for tool to calculate the expected signals for given
 *  RichRecSegments and RichRecTracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichExpectedTrackSignal : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichExpectedTrackSignal;}

  /// Expected number of signal photons for given track and hypothesis
  virtual double nSignalPhotons ( RichRecTrack * track,
                                  const Rich::ParticleIDType id ) const = 0;

  /// Expected number of signal photons for given segment and hypothesis
  virtual double nSignalPhotons ( RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable signal photons for given track and hypothesis
  virtual double nObservableSignalPhotons ( RichRecTrack * track,
                                            const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable signal photons for given segment and hypothesis
  virtual double nObservableSignalPhotons ( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) const = 0;

  /// Expected number of scattered photons for given track and hypothesis
  virtual double nScatteredPhotons ( RichRecTrack * track,
                                     const Rich::ParticleIDType id ) const = 0;

  /// Expected number of scattered photons for given segment and hypothesis
  virtual double nScatteredPhotons ( RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable scattered photons for given track and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecTrack * track,
                                               const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable scattered photons for given segment and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable signal+scattered photons for given track and hypothesis
  virtual double nTotalObservablePhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType id ) const = 0;

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  virtual double nTotalObservablePhotons ( RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const = 0;

  /// Expected emitted photons for given track and hypothesis
  virtual double nEmittedPhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id ) const = 0;

  /// Expected number of emitted photons for given segment and hypothesis
  virtual double nEmittedPhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) const = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecTrack * track,
                                      const Rich::ParticleIDType id ) const = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;


  /// The average energy of signal Cherenkov photons
  virtual double avgSignalPhotEnergy( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) const = 0;

  /// The average energy of emitted Cherenkov photons
  virtual double avgEmitPhotEnergy( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const = 0;

  /// Is it possible for this track to give Rich information
  virtual bool hasRichInfo( RichRecTrack * track ) const = 0;

  /// Is it geometrically possible for this segment to give Rich information
  virtual bool hasRichInfo( RichRecSegment * segment ) const = 0;


  /// Is this track above threshold for a given particle type in any radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type ) const = 0;

  /// Is this track above threshold for a given particle type in a given radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type,
                               const Rich::RadiatorType radiator ) const = 0;

  /// Is this segment above threshold for a given particle hypothesis
  virtual bool aboveThreshold( RichRecSegment * segment,
                               const Rich::ParticleIDType type ) const = 0;

  /// Set the threshold information in a RichPID object for given segment
  virtual void setThresholdInfo( RichRecSegment * segment,
                                 RichPID * pid ) const = 0;

  /// Set the threshold information in a RichPID object for given track
  virtual void setThresholdInfo( RichRecTrack * track,
                                 RichPID * pid ) const = 0;

  /// Is this track active in given radiator for given particle id
  virtual bool activeInRadiator( RichRecTrack * track,
                                 const Rich::RadiatorType rad,
                                 const Rich::ParticleIDType id = Rich::Electron ) const = 0;

};

#endif // RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
