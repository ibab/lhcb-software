// $Id: IRichRecPhotonTool.h,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECPHOTONTOOL_H 
#define RICHRECTOOLS_IRICHRECPHOTONTOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// RichKernel
#include "RichKernel/RichDefinitions.h"

// RichRecEvent
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichRecSegment.h"

/** @class RichRecPhotonTool RichRecPhotonTool.h
 *  
 *  Interface for tool which performs the association between RichRecTracks
 *  and RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecPhotonTool( "IRichRecPhotonTool" , 1 , 0 );

class IRichRecPhotonTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecPhotonTool; }

  /// Form a Photon candidate from a Segment and a pixel.
  virtual RichRecPhoton*
  reconstructPhoton( const SmartRef<RichRecSegment>& segment,
                     const SmartRef<RichRecPixel>& pixel) = 0;
  
  /// Form all photon candidates for a given track and pixel.
  virtual SmartRefVector<RichRecPhoton>
  reconstructPhotons(const SmartRef<RichRecTrack>& track,
                     const SmartRef<RichRecPixel>& pixel) = 0;

  /// Form all photon candidates for a given track, with all possible pixels.
  virtual SmartRefVector<RichRecPhoton>
  reconstructPhotons(const SmartRef<RichRecTrack>& track) = 0;

  /// Form all photon candidates for a given pixel, with all possible tracks.
  virtual SmartRefVector<RichRecPhoton>
  reconstructPhotons(const SmartRef<RichRecPixel>& pixel) = 0;

  /// Method to perform the asssociation of all tracks and pixels
  virtual void reconstructPhotons() = 0;

protected:

private:

};
#endif // RICHRECTOOLS_IRICHRECPHOTONTOOL_H 
