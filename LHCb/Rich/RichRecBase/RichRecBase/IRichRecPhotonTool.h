// $Id: IRichRecPhotonTool.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECPHOTONTOOL_H 
#define RICHRECTOOLS_IRICHRECPHOTONTOOL_H 1

// GaudiKernel
//#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/SmartRef.h"
//#include "GaudiKernel/SmartRefVector.h"

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichRecSegment.h"

// local
#include "RichRecBase/IRichRecTrackTool.h"
#include "RichRecBase/IRichRecSegmentTool.h"
#include "RichRecBase/IRichRecPixelTool.h"

/** @class RichRecPhotonTool RichRecPhotonTool.h
 *  
 *  Interface for tool which performs the association between RichRecTracks
 *  and RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecPhotonTool( "IRichRecPhotonTool" , 1 , 0 );

class IRichRecPhotonTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecPhotonTool; }

  /// Return Pointer to RichRecPhotons
  virtual RichRecPhotons * richPhotons() = 0;

  /// Form a Photon candidate from a Segment and a pixel. 
  virtual RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                            RichRecPixel * pixel ) = 0;
  
  /// Form all photon candidates for a given track and pixel.
  virtual SmartRefVector<RichRecPhoton> 
  reconstructPhotons( RichRecTrack * track, RichRecPixel * pixel ) = 0;

  /// Form all photon candidates for a given track, with all possible pixels.
  virtual SmartRefVector<RichRecPhoton>& 
  reconstructPhotons( RichRecTrack * track ) = 0;

  /// Form all photon candidates for a given pixel, with all possible tracks.
  virtual SmartRefVector<RichRecPhoton>& 
  reconstructPhotons( RichRecPixel * pixel ) = 0;

  /// Form all photon candidates for a given segment, with all possible pixels.
  virtual SmartRefVector<RichRecPhoton>& 
  reconstructPhotons( RichRecSegment * segment ) = 0;

  /// Method to perform the asssociation of all tracks and pixels
  virtual void reconstructPhotons() = 0;

  /// Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
  virtual double predictedPixelSignal( RichRecPhoton * photon,
                                       const Rich::ParticleIDType& id ) = 0;

  /// Returns square of distance seperating pixel and track hits on HPDs
  virtual double trackPixelHitSep2( const RichRecSegment * segment,
                                    const RichRecPixel * pixel ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECPHOTONTOOL_H 
