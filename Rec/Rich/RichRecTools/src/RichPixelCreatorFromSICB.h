// $Id: RichPixelCreatorFromSICB.h,v 1.1 2003-06-30 15:47:05 jonrob Exp $
#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMSICB_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMSICB_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interface
#include "RichRecBase/IRichPixelCreator.h"
#include "RiSicbGeom/PixelFinder.h"

// Event
#include "Event/RichDigit.h"

/** @class RichPixelCreatorFromSICB RichPixelCreatorFromSICB.h
 *
 *  Tool for the creation and manipulation of RichRecPixel objects.
 *  Uses RichDigits from the SICB digitisation as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichPixelCreatorFromSICB : public RichRecToolBase,
                                 virtual public IRichPixelCreator,
                                 virtual public IIncidentListener {

public:

  /// Standard constructor
  RichPixelCreatorFromSICB( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromSICB(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a RichRecPixel object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  RichRecPixel * newPixel( ContainedObject * obj );

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

  /// Pointer to PixelFinder tool
  IPixelFinder* m_pixelFinder;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  bool m_allDone;

  /// Pointer to pixel map
  std::map< int, RichRecPixel* > m_pixelExists;
  std::map< int, bool > m_pixelDone;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMSICB_H
