// $Id: RichRayleighScatter.h,v 1.1 2003-08-12 13:35:44 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHRAYLEIGHSCATTER_H 1

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

/** @class RichRayleighScatter RichRayleighScatter.h
 *
 *  Tool to calculate quantities related to Rayleigh scattering
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRayleighScatter : public RichRecToolBase,
                            virtual public IRichRayleighScatter {

public:

  /// Standard constructor
  RichRayleighScatter( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichRayleighScatter() {};

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

#endif // RICHRECTOOLS_RICHRAYLEIGHSCATTER_H
