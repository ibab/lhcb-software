// $Id: RichDefinitions.h,v 1.3 2002-04-05 06:36:56 cattanem Exp $
#ifndef RICHKERNEL_RICHDEFINITIONS_H
#define RICHKERNEL_RICHDEFINITIONS_H 1

#include "GaudiKernel/StreamBuffer.h"
#include <vector>
#include <list>

namespace Rich {

  /// Enumeration for particle ID algorithm types
  enum AlgorithmType {
    SummaryAlgorithm = 0,
    GlobalAlgorithm,
    LocalAlgorithm,
    RingFinderAlgorithm
  };

  /// Number of Rich particle types 
  static const int NRichParticleTypes = 5;

  /// Enumeration for particle ID types
  enum ParticleIDType {
    UnknownType = -1,
    Electron,
    Muon,
    Pion,
    Kaon,
    Proton,
    BelowThreshold
  };

  /// Enumeration for radiator types
  enum RadiatorType {
    Aerogel = 0,
    C4F10,
    CF4
  };

  /// Enumeration for Riches
  enum Detector {
    Rich1 = 0,
    Rich2
  };

}

/// Implement StreamBuffer >> method for Rich::ParticleIDType enumeration
inline StreamBuffer& operator >> (StreamBuffer& s, 
                                  Rich::ParticleIDType& particle) {
  int intType;
  s >> intType;
  particle = (Rich::ParticleIDType)intType;
  return s;
}

/// Implement StreamBuffer << method for Rich::ParticleIDType enumeration
inline StreamBuffer& operator << (StreamBuffer& s, 
                                  const Rich::ParticleIDType& particle) {
  s << (int)particle;
  return s;
}

#endif // RICHKERNEL_RICHDEFINITIONS_H
