
/** @file RichPixelCreatorFromCheatedRichDigits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromCheatedRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRichDigits.h,v 1.7 2004-07-27 16:14:11 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"

/** @class RichPixelCreatorFromCheatedRichDigits RichPixelCreatorFromCheatedRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation but then refers to the
 *  MCRichOpticalPhoton objects to use the true MC hit positions.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */

class RichPixelCreatorFromCheatedRichDigits : public RichRecToolBase,
                                              virtual public IRichPixelCreator,
                                              virtual public IIncidentListener {

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

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

public: // Public interface methods

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

  // Returns a pointer to the RichRecPixels
  RichRecPixels * richPixels() const;

private: // methods

  /** Create a new RichRecPixel for a given RichDigit and MCRichHit
   *  @return Pointer to the new RichRecPixel
   */
  RichRecPixel * newPixelFromHit( const RichDigit * digit, ///< Pointer to a RichDigit
                                  const MCRichHit * hit    ///< Pointer to an MCRichHit
                                  ) const;

  /// Initialise for a new event
  void InitNewEvent();

private: // data

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// MC Truth tool
  IRichMCTruthTool * m_mcTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  // Internal book keeping
  mutable std::map< unsigned, RichRecPixel* > m_pixelExists;
  mutable std::map< unsigned, bool > m_pixelDone;

};

inline void RichPixelCreatorFromCheatedRichDigits::InitNewEvent()
{
  // Initialise data for new event
  m_allDone = false;
  m_pixelExists.clear();
  m_pixelDone.clear();
  m_pixels = 0;
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
