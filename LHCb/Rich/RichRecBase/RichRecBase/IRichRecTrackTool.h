// $Id: IRichRecTrackTool.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECTRACKTOOL_H
#define RICHRECTOOLS_IRICHRECTRACKTOOL_H 1

// GaudiKernel
//#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/SmartRef.h"
//#include "GaudiKernel/SmartRefVector.h"

// From event models
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// Event
#include "Event/RichRecTrack.h"

/** @class RichRecTrackTool RichRecTrackTool.h
 *
 *  Tool which performs useful methods on RichRecTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecTrackTool( "IRichRecTrackTool" , 1 , 0 );

class IRichRecTrackTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichRecTrackTool;}

  /// Forms a new RichRecTrack object from a TrStoredTrack
  virtual RichRecTrack * newTrack ( TrStoredTrack * trTrack ) = 0;

 /// Expected emitted photons for given track and hypothesis
  virtual double nEmittedPhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType& id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecTrack * track,
                                      const Rich::ParticleIDType& id ) = 0;

  /// Expected number of signal photons for given track and hypothesis
  virtual double nSignalPhotons ( RichRecTrack * track,
                                  const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal photons for given track and hypothesis
  virtual double nObservableSignalPhotons ( RichRecTrack * track,
					    const Rich::ParticleIDType& id ) = 0;

  /// Expected number of scattered photons for given track and hypothesis
  virtual double nScatteredPhotons ( RichRecTrack * track,
                                     const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable scattered photons for given track and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecTrack * track,
					       const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal+scattered photons for
  /// given track and hypothesis
  virtual double nTotalObservablePhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType& id ) = 0;

  /// Is this track active in given radiator for given particle id
  virtual bool activeInRadiator( RichRecTrack * track,
                                 const Rich::RadiatorType rad,
                                 const Rich::ParticleIDType id =
                                 Rich::Electron ) = 0;

  /// Is it possible for this track to give Rich information
  virtual bool hasRichInfo( RichRecTrack * track ) = 0;

  /// Form all tracks from input TrStoredTracks
  virtual StatusCode newTracks() = 0;

  /// Return Pointer to RichRecTracks
  virtual RichRecTracks * richTracks() = 0;

  /// Locates parent MCParticle
  virtual MCParticle * parentMCP ( const RichRecTrack * track ) = 0;

  /// Locates parent TrStoredTrack
  virtual TrStoredTrack * parentTrTrack ( const RichRecTrack * track ) = 0;

  /// Is this track above threshold for a given particle type in any radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType & type ) = 0;

  /// Is this track above threshold for a given particle type in a given radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType & type,
                               const Rich::RadiatorType & radiator ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECTRACKTOOL_H
