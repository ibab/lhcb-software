
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRichDigits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromCheatedRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRichDigits.h,v 1.11 2005-05-28 16:45:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
#define RICHRECMCTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"

//--------------------------------------------------------------------------------------
/** @class RichPixelCreatorFromCheatedRichDigits RichPixelCreatorFromCheatedRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation but then refers to the
 *  MCRichOpticalPhoton objects to use the true MC hit positions.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//--------------------------------------------------------------------------------------

class RichPixelCreatorFromCheatedRichDigits : public RichPixelCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromCheatedRichDigits( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromCheatedRichDigits(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // Public interface methods

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

private: // methods

  /** Create a new RichRecPixel for a given RichDigit and MCRichHit
   *  @return Pointer to the new RichRecPixel
   */
  RichRecPixel * newPixelFromHit( const RichDigit * digit, ///< Pointer to a RichDigit
                                  const MCRichHit * hit    ///< Pointer to an MCRichHit
                                  ) const;

private: // data

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// MC Truth tool
  IRichMCTruthTool * m_mcTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Flags for which radiators to create pixels for
  std::vector<bool> m_usedRads;

};

#endif // RICHRECMCTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
