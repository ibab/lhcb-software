
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromMCRichHits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromMCRichHits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromMCRichHits.h,v 1.11 2005-06-23 15:14:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

//--------------------------------------------------------------------------------------
/** @class RichPixelCreatorFromMCRichHits RichPixelCreatorFromMCRichHits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses MCRichHits as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//--------------------------------------------------------------------------------------

class RichPixelCreatorFromMCRichHits : public RichPixelCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromMCRichHits( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromMCRichHits(){}

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

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// MC Truth tool
  const IRichMCTruthTool * m_mcTool;

  /// String containing input MCRichHits location in TES
  std::string m_mcHitsLocation;

  /// Flags for which radiators to create pixels for
  std::vector<bool> m_usedRads;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H
