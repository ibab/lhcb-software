// $Id: RichDetParameters.h,v 1.2 2004-07-15 15:44:40 jonrob Exp $
#ifndef RICHDETPARAMETERS_H 
#define RICHDETPARAMETERS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichDetParameters.h"

/** @class RichDetParameters RichDetParameters.h
 *  
 *  Tool to provide access to useful detector parameters
 * 
 *  @author Chris Jones
 *  @date   2004-03-29
 */

class RichDetParameters : public RichToolBase,
                          virtual public IRichDetParameters {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichDetParameters( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichDetParameters( ) {};

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Calculates the maximum observable photon energy for a given radiator medium
  double maxPhotonEnergy ( const Rich::RadiatorType rad ) const;

  // Calculates the minimum observable photon energy for a given radiator medium
  double minPhotonEnergy ( const Rich::RadiatorType rad ) const;

  // Calculates the mean observable photon energy for a given radiator medium
  double meanPhotonEnergy ( const Rich::RadiatorType rad ) const;

private:

  std::vector<double> m_maxPhotEn;  ///< The maximum photon energies
  std::vector<double> m_minPhotEn;  ///< The minimum photon energies
  std::vector<double> m_meanPhotEn; ///< The mean photon energies

};
#endif // RICHDETPARAMETERS_H
