// $Id: IRichPhotonCreator.h,v 1.2 2003-08-06 09:55:31 jonrob Exp $
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
  virtual RichRecPhotons *& richPhotons() = 0;

  /// Form a RichRecPhoton candidate from a given segment and a pixel.
  virtual RichRecPhoton * reconstructPhoton( RichRecSegment * segment,
                                             RichRecPixel * pixel ) = 0;

  /// Form all photon candidates for a given track and pixel.
  virtual RichRecTrack::Photons
  reconstructPhotons( RichRecTrack * track, RichRecPixel * pixel ) = 0;

  /// Form all photon candidates for a given track, with all possible pixels.
  virtual RichRecTrack::Photons &
  reconstructPhotons( RichRecTrack * track ) = 0;

  /// Form all photon candidates for a given pixel, with all possible tracks.
  virtual RichRecPixel::Photons &
  reconstructPhotons( RichRecPixel * pixel ) = 0;

  /// Form all photon candidates for a given segment, with all possible pixels.
  virtual RichRecSegment::Photons &
  reconstructPhotons( RichRecSegment * segment ) = 0;

  /// Method to perform the reconstruction of all tracks and pixels. 
  /// The most efficient methods to create all possible photon candidates.
  virtual void reconstructPhotons() = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONCREATOR_H
