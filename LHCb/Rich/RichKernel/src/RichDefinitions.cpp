// $Id: RichDefinitions.cpp,v 1.1 2002-06-18 16:43:34 jonrob Exp $
// Include files

// local
#include "RichKernel/RichDefinitions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDefinitions
//
// 2002-06-14 : Chris Jones
//-----------------------------------------------------------------------------

// Text conversion for ParticleIDType enumeration
std::string Rich::text( const Rich::ParticleIDType& particle ) {
  switch( particle ) {
  case Rich::Electron:       return "electron";
  case Rich::Muon:           return "muon";
  case Rich::Pion:           return "pion";
  case Rich::Kaon:           return "kaon";
  case Rich::Proton:         return "proton";
  case Rich::BelowThreshold: return "below threshold";
  case Rich::Unknown:        return "unknown";
  default:                   return "?"; // should never happen
  }
}

// Text conversion for RadiatorType enumeration
std::string Rich::text( const Rich::RadiatorType& radiator ) {
  switch( radiator ) {
  case Rich::Aerogel: return "aerogel";
  case Rich::C4F10:   return "C4F10";
  case Rich::CF4:     return "CF4";
  default:            return "?"; // should never happen
  }
}

// Text conversion for Detector enumeration
std::string Rich::text( const Rich::Detector& detector ) {
  switch( detector ) {
  case Rich::Rich1:   return "Rich1";
  case Rich::Rich2:   return "Rich2";
  default:            return "?"; // should never happen
  }
}

// Text conversion for Rich1Side enumeration
std::string Rich::text( const Rich::Rich1Side& side ) {
  switch( side ) {
  case Rich::top:     return "top";
  case Rich::bottom:  return "bottom";
  default:            return "?"; // should never happen
  }
}

// Text conversion for Rich2Side enumeration
std::string Rich::text( const Rich::Rich2Side& side ) {
  switch( side ) {
  case Rich::left:    return "left";
  case Rich::right:   return "right";
  default:            return "?"; // should never happen
  }
}
