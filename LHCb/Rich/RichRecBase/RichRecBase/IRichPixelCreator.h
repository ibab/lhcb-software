// $Id: IRichPixelCreator.h,v 1.3 2004-02-02 14:23:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHPIXELCREATOR_H
#define RICHRECTOOLS_IRICHPIXELCREATOR_H 1

// Event
#include "Event/RichRecPixel.h"
class RichDigit;

/** @class IRichPixelCreator IRichPixelCreator.h
 *
 *  Interface for tool for the creation and book-keeping of RichRecPixel objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPixelCreator( "IRichPixelCreator" , 1 , 0 );

class IRichPixelCreator : public virtual IAlgTool {

public:

  /// Returns the unique interface identifier
  static const InterfaceID& interfaceID() {return IID_IRichPixelCreator;}

  /// Returns a RichRecPixel object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  virtual RichRecPixel * newPixel( const ContainedObject * obj ) const = 0;

  /// Form all possible RichRecPixels from input Objects.
  /// The most efficient way to make all RichRecTrack objects in the event.
  virtual StatusCode newPixels() const = 0;

  /// Returns a pointer to the RichRecPixels
  virtual RichRecPixels * richPixels() const = 0;

};

#endif // RICHRECTOOLS_IRICHPIXELCREATOR_H
