// $Id: RichFunctionalRayleighScatterSICB.h,v 1.1 2003-11-25 14:06:39 jonrob Exp $
#ifndef RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTERSICB_H
#define RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTERSICB_H 1

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

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

/** @class RichFunctionalRayleighScatterSICB RichFunctionalRayleighScatterSICB.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering using a
 *  analytic functional form
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichFunctionalRayleighScatterSICB : public RichRecToolBase,
                                          virtual public IRichRayleighScatter {

public:

  /// Standard constructor
  RichFunctionalRayleighScatterSICB( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent );

  /// Destructor
  virtual ~RichFunctionalRayleighScatterSICB() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonScatteredProb( const RichRecSegment * segment,
                              const double energy );

private:  // Private data

  // Rayleigh sattering parameters
  double m_eVToMicron;  ///< To converter between energy in eV to wavelenght in micrometers
  double m_AeroClarity; ///< The areogel clarity

};

#endif // RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTERSICB_H
