
//=================================================================================
/** @file RichRadiatorType.h
 *
 *  Header file for RICH particle ID enumeration : RichRadiatorType
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//=================================================================================

#ifndef LHCBKERNEL_RICHRADIATORTYPE_H
#define LHCBKERNEL_RICHRADIATORTYPE_H 1

// STL
#include <string>
#include <vector>
#include <iostream>

// General namespace for RICH specific definitions documented in RichSide.h
namespace Rich
{

  /// Number of RICH radiators
  static const int NRadiatorTypes = 3;

  /** @enum Rich::RadiatorType
   *
   *  RICH radiator types
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  enum RadiatorType
    {
      InvalidRadiator = -1, ///< Unspecified radiator type
      Aerogel         = 0,  ///< Aerogel in RICH1
      Rich1Gas        = 1,  ///< Gaseous RICH1 radiator
      Rich2Gas        = 2,  ///< Gaseous RICH2 radiator
      C4F10           = 1,  ///< Gaseous RICH1 radiator (to be removed)
      CF4             = 2,  ///< Gaseous RICH2 radiator (to be removed)
      // background types
      GasQuartzWin    = 3,  ///< Quartz windows to the gas radiator volumes
      HPDQuartzWin    = 4,  ///< HPD Quartz windows
      Nitrogen        = 5,  ///< Nitrogen volume
      AerogelFilter   = 6,   ///< Aerogel filter material
      CO2             = 7,   ///< Carbon dioxide
      PMTQuartzWin    = 8    ///< MAPMT Quartz windows
    };

  /** Text conversion for RadiatorType enumeration
   *
   *  @param radiator Radiator type enumeration
   *  @return Radiator type as an std::string
   */
  std::string text( const Rich::RadiatorType radiator );

  /// Type for container of radiator types
  using Radiators = std::vector<Rich::RadiatorType>;

  /// Access a vector of all radiator types
  const Radiators & radiators() noexcept;

  /// Implement textual ostream << method for Rich::RadiatorType enumeration
  inline std::ostream& operator << ( std::ostream& s,
                                     const Rich::RadiatorType radiator )
  {
    return s << Rich::text( radiator );
  }

}

#endif // LHCBKERNEL_RICHRADIATORTYPE_H
