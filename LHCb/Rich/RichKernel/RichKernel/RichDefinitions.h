// $Id: RichDefinitions.h,v 1.6 2002-06-18 16:39:21 jonesc Exp $
#ifndef RICHKERNEL_RICHDEFINITIONS_H
#define RICHKERNEL_RICHDEFINITIONS_H 1

// std include
#include <vector>
#include <list>

// from gaudi
#include "GaudiKernel/StreamBuffer.h"

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
    Unknown = -1,
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

  /// Enumeration for Rich detectors 1 and 2
  enum Detector {
    Rich1 = 0,
    Rich2
  };

  /// Enumeration for Rich1 detector splits
  enum Rich1Side {
    top = 0,
    bottom
  };

  /// Enumeration for Rich1 detector splits
  enum Rich2Side {
    left = 0,
    right
  };

  /// Text conversion for ParticleIDType enumeration
  std::string text( const Rich::ParticleIDType& particle ) ;

  /// Text conversion for RadiatorType enumeration
  std::string text( const Rich::RadiatorType& radiator ) ;

  /// Text conversion for Detector enumeration
  std::string text( const Rich::Detector& detector ) ;

  /// Text conversion for Rich1Side enumeration
  std::string text( const Rich::Rich1Side& side ) ;

  /// Text conversion for Rich2Side enumeration
  std::string text( const Rich::Rich2Side& side ) ;

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

#endif // RICHKERNEL_RICHDEFINITIONS_H
