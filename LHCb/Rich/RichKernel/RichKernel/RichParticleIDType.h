// $Id: RichParticleIDType.h,v 1.6 2003-04-01 11:22:56 jonrob Exp $
#ifndef RICHKERNEL_PARTICLEIDTYPE_H
#define RICHKERNEL_PARTICLEIDTYPE_H 1

// std include
#include <string>
#include <vector>
#include <iostream>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"

/** @enumeration ParticleIDType ParticleIDType.h RichKernel/ParticleIDType.h
 *
 *  Enumeration for particle ID types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Number of Rich particle types
  static const int NParticleTypes = 5;
  static const int NRichParticleTypes = 5; // keep for backward compatibility

  /// Enumeration for particle ID types
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
