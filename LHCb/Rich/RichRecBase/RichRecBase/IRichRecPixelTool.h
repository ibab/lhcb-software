// $Id: IRichRecPixelTool.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECPIXELTOOL_H
#define RICHRECTOOLS_IRICHRECPIXELTOOL_H 1

// GaudiKernel
//#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/SmartRef.h"
//#include "GaudiKernel/SmartRefVector.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichDigit.h"

/** @class RichRecPixelTool RichRecPixelTool.h
 *
 *  Tool which performs useful methods on RichRecPixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecPixelTool( "IRichRecPixelTool" , 1 , 0 );

class IRichRecPixelTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichRecPixelTool;}

  /// Locates parent RichDigit
  virtual RichDigit * parentRichDigit ( const RichRecPixel * pixel ) = 0;

  /// Return Pointer to RichRecPixels
  virtual RichRecPixels * richPixels() = 0;

  /// Forms a new RichRecPixel object from a RichDigit
  virtual RichRecPixel * newPixel( RichDigit * digit ) = 0;

  /// Form all pixels for input RichDigits
  virtual StatusCode newPixels() = 0;

};

#endif // RICHRECTOOLS_IRICHRECPIXELTOOL_H
