// $Id: IRichTrackProperties.h,v 1.1 2003-06-30 15:11:56 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHTRACKPROPERTIES_H
#define RICHRECTOOLS_IRICHTRACKPROPERTIES_H 1

// Event
class RichRecTrack;
class RichPID;

// RichKernel
#include "RichKernel/RichParticleIDType.h"
#include "RichKernel/RichRadiatorType.h"

/** @class IRichTrackProperties IRichTrackProperties.h
 *
 *  Interface to tools providing a collection of useful methods based around 
 *  the RichRecTrack objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *  @todo Review this class and move methods to more logical and sef-contained tools
 */

static const InterfaceID IID_IRichTrackProperties( "IRichTrackProperties" , 1 , 0 );

class IRichTrackProperties : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichTrackProperties; }

  /// Expected emitted photons for given track and hypothesis
  virtual double nEmittedPhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecTrack * track,
                                      const Rich::ParticleIDType id ) = 0;

  /// Expected number of signal photons for given track and hypothesis
  virtual double nSignalPhotons ( RichRecTrack * track,
                                  const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal photons for given track and hypothesis
  virtual double nObservableSignalPhotons ( RichRecTrack * track,
                                            const Rich::ParticleIDType id ) = 0;

  /// Expected number of scattered photons for given track and hypothesis
  virtual double nScatteredPhotons ( RichRecTrack * track,
                                     const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable scattered photons for given track and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecTrack * track,
                                               const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal+scattered photons for given track and hypothesis
  virtual double nTotalObservablePhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType id ) = 0;

  /// Is this track active in given radiator for given particle id
  virtual bool activeInRadiator( RichRecTrack * track,
                                 const Rich::RadiatorType rad,
                                 const Rich::ParticleIDType id =
                                 Rich::Electron ) = 0;

  /// Is it possible for this track to give Rich information
  virtual bool hasRichInfo( RichRecTrack * track ) = 0;

  /// Is this track above threshold for a given particle type in any radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type ) = 0;

  /// Is this track above threshold for a given particle type in a given radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type,
                               const Rich::RadiatorType radiator ) = 0;

  /// Set the threshold information in a RichPID object for given track
  virtual void setThresholdInfo( RichRecTrack * track,
                                 RichPID * pid ) = 0;

};

#endif // RICHRECTOOLS_IRICHTRACKPROPERTIES_H
