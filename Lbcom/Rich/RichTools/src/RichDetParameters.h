
//-----------------------------------------------------------------------------
/** @file RichDetParameters.h
 *
 *  Header file for tool : Rich::DetParameters
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHDETPARAMETERS_H
#define RICHTOOLS_RICHDETPARAMETERS_H 1

// STL
#include <vector>

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichDetParameters.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class DetParameters RichDetParameters.h
   *
   *  Tool to provide access to useful detector parameters, that cannot be easily
   *  calculated on the fly. Some could eventually be moved to either the LHCbCond 
   *  or DDDB databases. 
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
    virtual ~DetParameters( ) {}

    // Initialization of the tool after creation
    StatusCode initialize();

  public: // methods (and doxygen comments) inherited from interface

    // Calculates the maximum observable photon energy for a given radiator medium
    double maxPhotonEnergy ( const Rich::RadiatorType rad ) const;

    // Calculates the minimum observable photon energy for a given radiator medium
    double minPhotonEnergy ( const Rich::RadiatorType rad ) const;

    // Calculates the mean observable photon energy for a given radiator medium
    double meanPhotonEnergy ( const Rich::RadiatorType rad ) const;

    // Calculate the standard deviation of n-1 distribution for observed photons
    double refIndexSD ( const Rich::RadiatorType rad ) const;

    // Returns the average acceptance outer limits in local HPD coordinates
    // for the given radiator type
    const IDetParameters::RadLimits & AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const;

  private:

    std::vector<double> m_maxPhotEn;  ///< The maximum photon energies
    std::vector<double> m_minPhotEn;  ///< The minimum photon energies
    std::vector<double> m_meanPhotEn; ///< The mean photon energies

    std::vector<double> m_refSD; ///< Standard deviation of n-1 distribution for observed photons

    /// The radiator acceptance limits
    std::vector< IDetParameters::RadLimits > m_radOutLimLoc;

  };

}

#endif // RICHTOOLS_RICHDETPARAMETERS_H
