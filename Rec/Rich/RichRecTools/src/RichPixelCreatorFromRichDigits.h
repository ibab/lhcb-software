
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigits.h
 *
 *  Header file for tool : RichPixelCreatorFromRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigits.h,v 1.15 2005-06-23 15:17:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

// Event
#include "Event/RichDigit.h"

//-----------------------------------------------------------------------------
/** @class RichPixelCreatorFromRichDigits RichPixelCreatorFromRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichPixelCreatorFromRichDigits : public RichPixelCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPixelCreatorFromRichDigits( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRichDigits(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

private: // methods

  /// Build a new RichRecPixel
  RichRecPixel * buildPixel ( const RichDigit * digit ) const;

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_idTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
