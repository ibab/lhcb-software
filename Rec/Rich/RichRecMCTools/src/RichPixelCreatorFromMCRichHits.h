// $Id: RichPixelCreatorFromMCRichHits.h,v 1.1.1.1 2003-10-13 16:21:50 jonesc Exp $
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
#include "RichDetTools/IRichDetInterface.h"

// Event
#include "Event/MCRichHit.h"

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

public:

  /// Standard constructor
  RichPixelCreatorFromMCRichHits( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromMCRichHits(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a RichRecPixel object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj );

  /// Form all possible RichRecPixels from input RichDigits.
  /// The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels();

  /// Returns a pointer to the RichRecPixels
  RichRecPixels *& richPixels();

private:

  /// Pointer to RichRecPixels
  RichRecPixels * m_pixels;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInt;

  /// String containing input MCRichHits location in TES
  std::string m_mcHitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  bool m_allDone;

  /// Pointer to pixel map
  std::map< long int, RichRecPixel* > m_pixelExists;
  std::map< long int, bool > m_pixelDone;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMMCRICHHITS_H
