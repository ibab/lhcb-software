// $Id: IRichPhotonCreator.h,v 1.3 2004-02-02 14:23:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONCREATOR_H
#define RICHRECTOOLS_IRICHPHOTONCREATOR_H 1

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichRecSegment.h"

/** @class IRichPhotonCreator IRichPhotonCreator.h
 *
 *  Interface for tool which performs the association between RichRecSegments
 *  and RichRecPixels to form RichRecPhotons.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPhotonCreator( "IRichPhotonCreator" , 1 , 0 );

class IRichPhotonCreator : public virtual IAlgTool {

public:

  /// return unique interface identifier
  static const InterfaceID& interfaceID() { return IID_IRichPhotonCreator; }

  /// Return Pointer to the container of RichRecPhotons
  virtual RichRecPhotons * richPhotons() const = 0;

  /// Form a RichRecPhoton candidate from a given segment and a pixel.
  virtual RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                             RichRecPixel * pixel ) const = 0;

  /// Form all photon candidates for a given track and pixel.
  virtual RichRecTrack::Photons
  reconstructPhotons( RichRecTrack * track, RichRecPixel * pixel ) const = 0;

  /// Form all photon candidates for a given track, with all possible pixels.
  virtual const RichRecTrack::Photons &
  reconstructPhotons( RichRecTrack * track ) const = 0;

  /// Form all photon candidates for a given pixel, with all possible tracks.
  virtual const RichRecPixel::Photons &
  reconstructPhotons( RichRecPixel * pixel ) const = 0;

  /// Form all photon candidates for a given segment, with all possible pixels.
  virtual const RichRecSegment::Photons &
  reconstructPhotons( RichRecSegment * segment ) const = 0;

  /// Method to perform the reconstruction of all tracks and pixels. 
  /// The most efficient methods to create all possible photon candidates.
  virtual void reconstructPhotons() const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONCREATOR_H
