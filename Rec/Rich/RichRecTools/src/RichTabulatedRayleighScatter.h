// $Id: RichTabulatedRayleighScatter.h,v 1.1 2003-08-26 14:40:21 jonrob Exp $
#ifndef RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

/** @class RichTabulatedRayleighScatter RichTabulatedRayleighScatter.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering using
 *  tabulated quantities in the XML
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedRayleighScatter : public RichRecToolBase,
                                     virtual public IRichRayleighScatter {

public:

  /// Standard constructor
  RichTabulatedRayleighScatter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedRayleighScatter() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonScatteredProb( const RichRecSegment * segment,
                              const double energy );

private:  // Private data

  /// Tabulated Rayleigh scattering length
  Rich1DTabProperty * m_rayScatL;

  /// Location of tabulated  rayleigh scattering length in XML
  std::string m_rayScatLoc;

};

#endif // RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
