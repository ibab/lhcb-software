
//-----------------------------------------------------------------------------
/** @file RichDetParameters.h
 *
 *  Header file for tool : RichDetParameters
 *
 *  CVS Log :-
 *  $Id: RichDetParameters.h,v 1.7 2006-12-01 13:13:13 cattanem Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHDETPARAMETERS_H
#define RICHTOOLS_RICHDETPARAMETERS_H 1

// from Gaudi
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// Rich Kernel
#include "RichKernel/BoostArray.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichDetParameters.h"

//-----------------------------------------------------------------------------
/** @class RichDetParameters RichDetParameters.h
 *
 *  Tool to provide access to useful detector parameters
 *
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 *
 *  @todo Replace hardcoded data with access to XML/D-Base
 */
//-----------------------------------------------------------------------------

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

  // Returns the average acceptance outer limits in local HPD coordinates
  // for the given radiator type
  const IRichDetParameters::RadLimits & AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const;

private:

  std::vector<double> m_maxPhotEn;  ///< The maximum photon energies
  std::vector<double> m_minPhotEn;  ///< The minimum photon energies
  std::vector<double> m_meanPhotEn; ///< The mean photon energies

  /// The radiator acceptance limits
  boost::array< IRichDetParameters::RadLimits, Rich::NRadiatorTypes > m_radOutLimLoc;

};
#endif // RICHTOOLS_RICHDETPARAMETERS_H
