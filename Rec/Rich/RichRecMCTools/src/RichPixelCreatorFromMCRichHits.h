
/** @file RichPixelCreatorFromMCRichHits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromMCRichHits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromMCRichHits.h,v 1.8 2005-02-02 10:01:48 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2005/01/13 14:39:01  jonrob
 *  Various updates
 *
 *  Revision 1.6  2004/07/27 16:14:11  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H 1

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

// RichKernel
#include "RichKernel/RichHashMap.h"

// Event
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

/** @class RichPixelCreatorFromMCRichHits RichPixelCreatorFromMCRichHits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses MCRichHits as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPixelCreatorFromMCRichHits : public RichRecToolBase,
                                       virtual public IRichPixelCreator,
                                       virtual public IIncidentListener {

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

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// MC Truth tool
  IRichMCTruthTool * m_mcTool;

  /// String containing input MCRichHits location in TES
  std::string m_mcHitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  // book keeping information
  mutable RichHashMap< long int, RichRecPixel* > m_pixelExists;
  mutable RichHashMap< long int, bool > m_pixelDone;

  /// Flags for which radiators to create pixels for
  std::vector<bool> m_usedRads;

};

inline void RichPixelCreatorFromMCRichHits::InitNewEvent()
{
  // Initialise navigation data
  m_allDone = false;
  m_pixelExists.clear();
  m_pixelDone.clear();
  m_pixels = 0;
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H
