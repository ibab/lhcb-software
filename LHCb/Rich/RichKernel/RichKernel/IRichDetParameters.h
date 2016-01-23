
//-----------------------------------------------------------------------------
/** @file IRichDetParameters.h
 *
 *  Header file for tool interface : Rich::IDetParameters
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHDETPARAMETERS_H
#define RICHKERNEL_IRICHDETPARAMETERS_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichDetParameters( "Rich::IDetParameters", 1, 0 );

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class IDetParameters IRichDetParameters.h RichKernel/IRichDetParameters.h
   *
   *  Interface for tools providing access to useful detector parameters, that 
   *  cannot be easily calculated on the fly. Some could eventually be moved to 
   *  either the LHCbCond or DDDB databases. 
   *
   *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
   *  @date   2004-03-29
   */
  //-----------------------------------------------------------------------------

  class IDetParameters : public virtual IAlgTool
  {

  public:

    /** @class RadLimits IRichDetParameters.h RichKernel/IRichDetParameters.h
     *
     *  Helper class for IRichDetParameters to contain HPD acceptance data
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   2005-01-29
     */
    class RadLimits 
    {

    public:

      /// Default Constructor
      RadLimits() = default;

      /** Constructor from limit data
       *
       *  Limits are unsigned, and correspond to a single HPD panel.
       *
       *  @param minX The minimum x edge of acceptance
       *  @param maxX The maximum x edge of acceptance
       *  @param minY The minimum y edge of acceptance
       *  @param maxY The maximum y edge of acceptance
       */
      RadLimits( const double minX, const double maxX,
                 const double minY, const double maxY )
        : m_maxX(maxX), m_minX(minX), m_maxY(maxY), m_minY(minY) { }

      /// Access the minimum x limit
      inline double minX() const { return m_minX; }
      /// Access the maximum x limit
      inline double maxX() const { return m_maxX; }
      /// Access the minimum y limit
      inline double minY() const { return m_minY; }
      /// Access the maximum y limit
      inline double maxY() const { return m_maxY; }

    private:

      double m_maxX {0}; ///< Maximum X limit
      double m_minX {0}; ///< Minimum X limit
      double m_maxY {0}; ///< Maximum Y limit
      double m_minY {0}; ///< Minimum Y limit

    };

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichDetParameters; }

    /** Calculates the maximum observable photon energy for a given radiator medium
     *
     *  @param rad  The radiator type
     *
     *  @return The value of the maximum photon energy for the given radiator
     */
    virtual double maxPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;

    /** Calculates the minimum observable photon energy for a given radiator medium
     *
     *  @param rad  The radiator type
     *
     *  @return The value of the minimum photon energy for the given radiator
     */
    virtual double minPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;

    /** Calculate the mean observable photon energy for a given radiator medium
     *
     *  @param rad  The radiator type
     *
     * @return The value of the mean photon energy for the given radiator
     */
    virtual double meanPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;

    /** Calculate the standard deviation of n-1 distribution for observed photons
     *
     *  @param rad  The radiator type
     *
     * @return The standard deviation of n-1 distribution for observed photons for the given radiator
     */
    virtual double refIndexSD ( const Rich::RadiatorType rad ) const = 0;

    /** Returns the average acceptance outer limits in local HPD coordinates
     *  for the given radiator type
     *
     *  @param rad The radiator type
     *
     *  @return The average (x,y) outer acceptance limits
     *          .first  Gives the x limit
     *          .second Gives the y limit
     */
    virtual const IDetParameters::RadLimits &
    AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHDETPARAMETERS_H
