// $Id: IRichRecSegmentTool.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_IRICHRECSEGMENTTOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// RichKernel
#include "RichKernel/RichDefinitions.h"

// RichRecEvent
#include "Event/RichRecSegment.h"

/** @class RichRecSegmentTool RichRecSegmentTool.h
 *
 *  Interface for tool which performs the association between RichRecTracks
 *  and RichRecPixels to form RichRecSegments
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecSegmentTool( "IRichRecSegmentTool" , 
                                                  1 , 0 );

class IRichRecSegmentTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichRecSegmentTool;}

  /// Obtain geometrical efficiency for this track and hypothesis
  virtual double geomEfficiency ( SmartRef<RichRecSegment>& segment,
                                  const Rich::ParticleIDType id ) = 0;

  /// Expected photon signal for given segment and hypothesis
  virtual double expObsPhotonSignal ( SmartRef<RichRecSegment>& segment,
                                      const Rich::ParticleIDType id ) = 0;

  /// Expected emitted photons for given segment and hypothesis
  virtual double expPhotons ( SmartRef<RichRecSegment>& segment,
                              const Rich::ParticleIDType id ) = 0;

protected:

private:

};
#endif // RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
