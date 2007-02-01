
//-----------------------------------------------------------------------------
/** @file RichDetParameters.h
 *
 *  Header file for tool : Rich::DetParameters
 *
 *  CVS Log :-
 *  $Id: RichDetParameters.h,v 1.8 2007-02-01 17:51:10 jonrob Exp $
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
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class DetParameters RichDetParameters.h
   *
   *  Tool to provide access to useful detector parameters
   *
   *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
   *  @date   2004-03-29
   *
   *  @todo Replace hardcoded data with access to XML/D-Base
   */
  //-----------------------------------------------------------------------------

  class DetParameters : public Rich::ToolBase,
                        virtual public IDetParameters
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    DetParameters( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

    /// Destructor
    virtual ~DetParameters( ) {};

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
    const IDetParameters::RadLimits & AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const;

  private:

    std::vector<double> m_maxPhotEn;  ///< The maximum photon energies
    std::vector<double> m_minPhotEn;  ///< The minimum photon energies
    std::vector<double> m_meanPhotEn; ///< The mean photon energies

    /// The radiator acceptance limits
    boost::array< IDetParameters::RadLimits, Rich::NRadiatorTypes > m_radOutLimLoc;

  };

}

#endif // RICHTOOLS_RICHDETPARAMETERS_H
