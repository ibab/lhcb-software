// $Id: IRichRecTrackTool.h,v 1.1.1.1 2002-07-28 10:46:23 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECTRACKTOOL_H
#define RICHRECTOOLS_IRICHRECTRACKTOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From event models
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// Rich
#include "RichKernel/RichDefinitions.h"
#include "Event/RichRecTrack.h"

/** @class RichRecTrackTool RichRecTrackTool.h
 *
 *  Tool which performs useful methods on RichRecTracks
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecTrackTool( "IRichRecTrackTool" , 1 , 0 );

class IRichRecTrackTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichRecTrackTool;}

  /// Locates parent MCParticle
  virtual void parent ( const SmartRef<RichRecTrack>& track, 
                        SmartRef<MCParticle>& pParent ) = 0;

  /// Locates parent TrStoredTrack
  virtual void parent ( const SmartRef<RichRecTrack>& track,
                        SmartRef<TrStoredTrack>& pParent ) = 0;

  /// Expected photon signal for given track and hypothesis
  virtual double expObsPhotonSignal ( const SmartRef<RichRecTrack>& track,
                                      const Rich::ParticleIDType id ) = 0;

  /// Expected emitted photons for given track and hypothesis
  virtual double expPhotons ( const SmartRef<RichRecTrack>& track,
                              const Rich::ParticleIDType id ) = 0;

protected:

private:

};
#endif // RICHRECTOOLS_IRICHRECTRACKTOOL_H
