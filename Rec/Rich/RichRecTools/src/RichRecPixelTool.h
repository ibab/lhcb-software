// $Id: RichRecPixelTool.h,v 1.1 2003-04-01 14:33:22 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECPIXELTOOL_H
#define RICHRECTOOLS_RICHRECPIXELTOOL_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// interface
#include "RichRecBase/IRichRecPixelTool.h"

// Detector
#include "RichDetTools/IRichDetInterface.h"

// Temporary SICB geometry package
#include "RiSicbGeom/PixelFinder.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecPixelTool RichRecPixelTool.h
 *
 *  Tool which performs useful methods on RichRecPixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecPixelTool : public AlgTool,
                         virtual public IRichRecPixelTool,
                         virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecPixelTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// Destructor
  virtual ~RichRecPixelTool(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Locates parent RichDigit
  RichDigit * parentRichDigit ( const RichRecPixel * pixel );

  /// Return Pointer to RichRecPixels
  RichRecPixels * richPixels();

  /// Forms a new RichRecPixel object from a RichDigit
  RichRecPixel * newPixel( RichDigit * digit );

  /// Form all pixels for input RichDigits
  StatusCode newPixels();

private:

  /// Pointer to RichRecPixels
  RichRecPixels * m_pixels;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to PixelFinder tool (temporary for SICB pixels)
  IPixelFinder* m_pixelFinder;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag for indicating digits are from SICB processing
  bool m_sicbDigits;

  /// Flag to signify all pixels have been formed
  bool m_allDone;

  /// Pointer to pixel map
  std::map< int, RichRecPixel* > m_pixelExists;
  std::map< int, bool > m_pixelDone;

};

#endif // RICHRECTOOLS_RICHRECPIXELTOOL_H
