
//-----------------------------------------------------------------------------
/** @file IRichPixelCreator.h
 *
 *  Header file for RICH reconstruction tool interface : IRichPixelCreator
 *
 *  CVS Log :-
 *  $Id: IRichPixelCreator.h,v 1.5 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHPIXELCREATOR_H
#define RICHRECTOOLS_IRICHPIXELCREATOR_H 1

// Event
#include "Event/RichRecPixel.h"
class RichDigit;

/// Static Interface Identification
static const InterfaceID IID_IRichPixelCreator( "IRichPixelCreator" , 1 , 0 );

/** @class IRichPixelCreator IRichPixelCreator.h
 *
 *  Interface for tool for the creation and book-keeping of RichRecPixel objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichPixelCreator : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichPixelCreator;}

  /** Creates and returns the RichRecPixel associated to the given data object.
   *
   *  @param obj The data object to use to build the RichRecPixel. The runtime
   *             type of obj must correspond to that expected by the particular
   *             implementation being used.
   *
   *  @return Pointer to the associated RichRecPixel object
   *  @retval NULL  Unable to build a RichRecPixel from the input data object
   *  @retval !NULL Object was successfully built
   */
  virtual RichRecPixel * newPixel( const ContainedObject * obj ) const = 0;

  /** Form all possible RichRecPixels from data objects at the configured
   *  input location in the TES.
   *  The most efficient way to make all RichRecPixel objects in the event.
   *
   * @return status code for the request
   * @retval StatusCode::SUCCESS All possible RichRecPixels were successfully built
   * @retval StatusCode::FAILURE A problem occurred whilst building the RichRecPixels
   */
  virtual StatusCode newPixels() const = 0;

  /** Access to all RichRecPixels currently created using this tool.
   *  Tool should never return a null pointer.
   *
   *  @return Pointer to the container of RichRecPixels
   */
  virtual RichRecPixels * richPixels() const = 0;

};

#endif // RICHRECTOOLS_IRICHPIXELCREATOR_H
