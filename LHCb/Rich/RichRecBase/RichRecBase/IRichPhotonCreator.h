// $Id: IRichPhotonCreator.h,v 1.1 2003-06-30 15:11:54 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONCREATOR_H
#define RICHRECTOOLS_IRICHPHOTONCREATOR_H 1

// Event
class RichRecTrack;
class RichRecPixel;
#include "Event/RichRecPhoton.h"
class RichRecSegment;

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

  /// Method to perform the reconstruction of all tracks and pixels. 
  /// The most efficient methods to create all possible photon candidates.
  virtual void reconstructPhotons() = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONCREATOR_H
