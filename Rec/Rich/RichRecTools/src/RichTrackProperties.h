// $Id: RichTrackProperties.h,v 1.1 2003-06-30 15:47:06 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTRACKPROPERTIES_H
#define RICHRECTOOLS_RICHTRACKPROPERTIES_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichTrackProperties.h"
#include "RichRecBase/IRichSegmentProperties.h"

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichPID.h"

/** @class RichTrackProperties RichTrackProperties.h
 *
 *  Tools providing a collection of useful methods based around 
 *  the RichRecTrack objects.
 *  the RichRecTrack objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *  @todo Review this class and move methods to more logical and sef-contained tools
 */

class RichTrackProperties : public RichRecToolBase,
                            virtual public IRichTrackProperties {
  
public:

  /// Standard constructor
  RichTrackProperties( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichTrackProperties() {}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Expected number of observable signal photons for given track
  /// and hypothesis
  double nSignalPhotons ( RichRecTrack * track,
                          const Rich::ParticleIDType id );

  /// Expected number of observable signal photons for given track and hypothesis
  double nObservableSignalPhotons ( RichRecTrack * track,
                                    const Rich::ParticleIDType id );

  /// Expected number of observable signal+scattered photons for
  /// given track and hypothesis
  double nTotalObservablePhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given track and hypothesis
  double nEmittedPhotons ( RichRecTrack * track,
                           const Rich::ParticleIDType id );

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  double nDetectablePhotons ( RichRecTrack * track,
                              const Rich::ParticleIDType id );

  /// Expected number of scattered photons for given track and hypothesis
  double nScatteredPhotons ( RichRecTrack * track,
                             const Rich::ParticleIDType id );

  /// Expected number of observable scattered photons for given track and hypothesis
  double nObservableScatteredPhotons ( RichRecTrack * track,
                                       const Rich::ParticleIDType id );

  /// Is this track active in this radiator medium for given particle id
  bool activeInRadiator( RichRecTrack * track,
                         const Rich::RadiatorType rad,
                         const Rich::ParticleIDType id = Rich::Electron );

  /// Is it possible for this track to give Rich information
  bool hasRichInfo( RichRecTrack * track );

  /// Is this track above threshold for a given particle type
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type );

  /// Is this track above threshold for a given particle type in a given radiator
  bool aboveThreshold( RichRecTrack * track,
                       const Rich::ParticleIDType type,
                       const Rich::RadiatorType radiator );

  /// Set the threshold information in a RichPID object for given track
  void setThresholdInfo( RichRecTrack * track, RichPID * pid );

private:

  // tool pointers
  IRichSegmentProperties * m_segProps;

};

#endif // RICHRECTOOLS_RICHTRACKPROPERTIES_H
