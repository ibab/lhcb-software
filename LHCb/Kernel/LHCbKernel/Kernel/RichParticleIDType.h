
//--------------------------------------------------------------------------
/** @file RichParticleIDType.h
 *
 *  Header file for RICH particle ID enumeration : RichParticleIDType
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------

#ifndef RICHKERNEL_PARTICLEIDTYPE_H
#define RICHKERNEL_PARTICLEIDTYPE_H 1

// std include
#include <string>
#include <vector>
#include <iostream>

// General namespace for RICH specific definitions documented in RichSide.h
namespace Rich 
{

  /// Number of RICH particle types (Electron, Muon, Pion, Kaon, Proton and 'Below Threshold')
  static const int NParticleTypes = 6;

  //--------------------------------------------------------------------------
  /** @enum ParticleIDType
   *  Enumeration for the RICH particle ID mass hypotheses.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //--------------------------------------------------------------------------
  enum ParticleIDType
    {
      Unknown = -1,   ///< Unknown particle type
      Electron,       ///< Represents e+ or e-
      Muon,           ///< Represents mu+ or mu-
      Pion,           ///< Represents pi+ or pi-
      Kaon,           ///< Represents K+ or K-
      Proton,         ///< Represents Pr+ or Pr-
      BelowThreshold  ///< Particle type is below threshold
    };

  /// Text conversion for Rich::ParticleIDType enumeration
  std::string text( const Rich::ParticleIDType particle );

  /// Type for container of particle types
  typedef std::vector<ParticleIDType> Particles;

  /// Access a vector of valid particle ID types
  const Particles & particles() noexcept;

  /// Implement textual ostream << method for Rich::ParticleIDType enumeration
  inline std::ostream& operator << ( std::ostream& s,
                                     const Rich::ParticleIDType& particle ) 
  {
    return s << Rich::text(particle) ;
  }
  
}

#endif // RICHKERNEL_PARTICLEIDTYPE_H
