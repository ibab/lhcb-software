// $Id: RichDetParameters.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
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

public:

  /// Standard constructor
  RichDetParameters( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  /// Destructor
  virtual ~RichDetParameters( ) {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns the maximum photon energy for a given radiator
  double maxPhotonEnergy( const Rich::RadiatorType rad ) const;

  /// Returns the minimum photon energy for a given radiator
  double minPhotonEnergy( const Rich::RadiatorType rad ) const;

  /// Returns the mean photon energy for a given radiator
  double meanPhotonEnergy( const Rich::RadiatorType rad ) const;

private:

  // Photon energy ranges
  std::vector<double> m_maxPhotEn;
  std::vector<double> m_minPhotEn;
  std::vector<double> m_meanPhotEn;

};
#endif // RICHDETPARAMETERS_H
