
//=================================================================================
/** @file RichRadiatorType.h
 *
 *  Header file for RICH particle ID enumeration : RichRadiatorType
 *
 *  CVS Log :-
 *  $Id: RichRadiatorType.h,v 1.8 2006-02-16 15:48:37 jonrob Exp $
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

// from Gaudi
#include "GaudiKernel/MsgStream.h"

//  General namespace for RICH specific definitions documented in RichSide.h
namespace Rich {

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
      C4F10           = 1,  ///< Gaseous RICH1 radiator (kept for backwards compatibility)
      CF4             = 2   ///< Gaseous RICH2 radiator (kept for backwards compatibility)
    };

  /** Text conversion for RadiatorType enumeration
   *
   *  @param radiator Radiator type enumeration
   *  @return Radiator type as an std::string
   */
  std::string text( const Rich::RadiatorType radiator );

  /// Std Vector typedef
  typedef std::vector<Rich::RadiatorType> RadiatorTypeVector;

  /// Std const Vector typedef
  typedef std::vector<const Rich::RadiatorType> RadiatorTypeConstVector;

  /// Std vector typedef
  typedef RadiatorTypeVector Radiators;

  /// Access a vector of all radiator types
  //const Rich::Radiators & radiators();

}

/// Implement textual ostream << method for Rich::RadiatorType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::RadiatorType & radiator ) 
{
  s << Rich::text( radiator );
  return s;
}

/// Implement textual MsgStream << method for Rich::RadiatorType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::RadiatorType & radiator ) 
{
  s << Rich::text( radiator );
  return s;
}

#endif // LHCBKERNEL_RICHRADIATORTYPE_H
