
/** @file RichPixelCreatorFromRichDigits.h
 *
 *  Header file for tool : RichPixelCreatorFromRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigits.h,v 1.9 2004-10-13 09:52:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2004/07/27 20:15:32  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H 1

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

// Event
#include "Event/RichDigit.h"

/** @class RichPixelCreatorFromRichDigits RichPixelCreatorFromRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPixelCreatorFromRichDigits : public RichRecToolBase,
                                       virtual public IRichPixelCreator,
                                       virtual public IIncidentListener {

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

  // Implement the handle method for the Incident service.
  // This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

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

  /// Build a new RichRecPixel
  RichRecPixel * buildPixel ( const RichDigit * digit ) const;

private: // data

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Pointer to pixel map
  mutable std::map< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;
  mutable std::map< RichSmartID::KeyType, bool > m_pixelDone;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

};

inline void RichPixelCreatorFromRichDigits::InitNewEvent()
{
  // Initialise navigation data
  m_allDone = false;
  m_pixels  = 0;
  if ( m_bookKeep ) {
    m_pixelExists.clear();
    m_pixelDone.clear();
  }
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
