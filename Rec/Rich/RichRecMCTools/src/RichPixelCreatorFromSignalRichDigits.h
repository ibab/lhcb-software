
/** @file RichPixelCreatorFromSignalRichDigits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromSignalRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRichDigits.h,v 1.3 2004-07-27 16:14:11 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"

/** @class RichPixelCreatorFromSignalRichDigits RichPixelCreatorFromSignalRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation but then refers to the
 *  MCRichOpticalPhoton objects select on the true Cherenkov hits.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */

class RichPixelCreatorFromSignalRichDigits : public RichRecToolBase,
                                             virtual public IRichPixelCreator,
                                             virtual public IIncidentListener {

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromSignalRichDigits( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromSignalRichDigits(){}

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

  /// Initialise for a new event
  void InitNewEvent();

private: // data

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// MC Truth tool
  IRichMCTruthTool * m_mcTool;

  /// Pointer to delegated pixel maker
  IRichPixelCreator * m_pixMaker;

  /// Nickname of RichPixel Creator to use
  std::string m_subPixelCreatorName;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

};

inline void RichPixelCreatorFromSignalRichDigits::InitNewEvent()
{
  // Initialise data for new event
  m_allDone = false;
  m_pixels  = 0;
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H
