// $Id: RichParticleIDType.h,v 1.8 2003-11-03 08:36:39 cattanem Exp $
#ifndef RICHKERNEL_PARTICLEIDTYPE_H
#define RICHKERNEL_PARTICLEIDTYPE_H 1

// std include
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/// namespace for RICH detector specifics
namespace Rich {

  /// Number of RICH particle types
  static const int NParticleTypes = 5;

  /// Number of RICH particle types, kept for backward compatibility
  static const int NRichParticleTypes = 5;

  /** @enum Rich::ParticleIDType
   *  Enumeration for RICH particle ID mass hypotheses
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2002-06-19
   */
  enum ParticleIDType {
    Unknown = -1,
    Electron,
    Muon,
    Pion,
    Kaon,
    Proton,
    BelowThreshold
  };

  /// Text conversion for Rich::ParticleIDType enumeration
  std::string text( const Rich::ParticleIDType& particle );

  /// Std Vector typedef
  typedef std::vector<ParticleIDType> ParticleIDTypeVector;

}

/// Implement StreamBuffer >> method for Rich::ParticleIDType enumeration
inline StreamBuffer& operator >> ( StreamBuffer& s,
                                   Rich::ParticleIDType& particle ) {
  int intType;
  s >> intType;
  particle = (Rich::ParticleIDType)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::ParticleIDType enumeration
inline StreamBuffer& operator << ( StreamBuffer& s,
                                   const Rich::ParticleIDType& particle ) {
  s << (int)particle;
  return s;
}

/// Implement textual ostream << method for Rich::ParticleIDType enumeration
inline std::ostream& operator << ( std::ostream& s,
                                   const Rich::ParticleIDType& particle ) {
  s << Rich::text( particle );
  return s;
}

/// Implement textual MsgStream << method for Rich::ParticleIDType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::ParticleIDType& particle ) {
  s << Rich::text( particle );
  return s;
}

#endif // RICHKERNEL_PARTICLEIDTYPE_H
