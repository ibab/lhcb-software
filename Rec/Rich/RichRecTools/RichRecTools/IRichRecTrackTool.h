// $Id: IRichRecTrackTool.h,v 1.2 2002-11-14 13:54:21 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHRECTRACKTOOL_H
#define RICHRECTOOLS_IRICHRECTRACKTOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From event models
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// RichKernel
#include "RichKernel/RichDefinitions.h"

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

  /// Expected number of observable signal photons for given track 
  /// and hypothesis
  virtual double signalPhotons ( RichRecTrack * track,
                                 const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal+scattered photons for 
  /// given track and hypothesis
  virtual double allPhotons ( RichRecTrack * track,
                              const Rich::ParticleIDType& id ) = 0;

  /// Expected emitted photons for given track and hypothesis
  virtual double emittedPhotons ( RichRecTrack * track,
                                  const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable scattered photons for given track and hypothesis
  virtual double scatteredPhotons ( RichRecTrack * track,
                                    const Rich::ParticleIDType& id ) = 0;

  /// Is this track active in given radiator for given particle id
  virtual bool activeInRadiator( RichRecTrack * track,
                                 const Rich::RadiatorType rad,
                                 const Rich::ParticleIDType id = 
                                 Rich::Electron ) = 0;

  /// Is it geometrically possible for this track to give Rich information
  virtual bool hasRichInfo( RichRecTrack * track ) = 0;

  /// Form all tracks for input TrStoredTracks
  virtual StatusCode newTracks() = 0;

  /// Return Pointer to RichRecTracks
  virtual RichRecTracks * richTracks() = 0;

  /// Locates parent MCParticle
  virtual MCParticle * parentMCP ( const RichRecTrack * track ) = 0;
  
  /// Locates parent TrStoredTrack
  virtual TrStoredTrack * parentTrTrack ( const RichRecTrack * track ) = 0;

  /// Is this track above threshold for a given particle type in any radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               Rich::ParticleIDType & type ) = 0;

  /// Is this track above threshold for a given particle type in a given radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               Rich::ParticleIDType & type,
                               Rich::RadiatorType & radiator ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECTRACKTOOL_H
