// $Id: RichRecRecoPixels.h,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECRECOPIXELS_H 
#define RICHRECTOOLS_RICHRECRECOPIXELS_H 1

// Fram Gaudi framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// Temporary SICB geometry package
#include "RiSicbGeom/SicbGeom.h"
#include "RiSicbGeom/PixelFinder.h"

// From Rich event model
#include "Event/RichRecPixel.h"
#include "Event/RichDigit.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecRecoPixels RichRecRecoPixels.h 
 *
 *  Algorithm for forming RichRecPixels from reconstructed digits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichRecRecoPixels : public Algorithm {

public:

  /// Standard algorithm constructor
  RichRecRecoPixels (const std::string &name,
                     ISvcLocator *svcLoc);

  /// Default destructor
  ~RichRecRecoPixels();

  /// Gaudi framework initialize method
  virtual StatusCode initialize ();

  /// Gaudi framework execute method
  virtual StatusCode execute ();

  /// Gaudi framework finalize method
  virtual StatusCode finalize ();

private:

  /// Default constructor
  RichRecRecoPixels();

  /// Default copy constructor
  RichRecRecoPixels(const RichRecRecoPixels &right);

  /// Default copy constructor
  RichRecRecoPixels & operator=(const RichRecRecoPixels &right);

  /// String containing input reconstructed pixel location in TDS
  std::string m_recoDigitsLocation;

  /// String containing output RichRecPixel location in TDS
  std::string m_richRecPixelsLocation;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

  /// Pointer to PixelFinder tool (temporary for SICB pixels)
  IPixelFinder* m_pixelFinder;

};

#endif // RICHRECTOOLS_RICHRECRECOPIXELS_H 
