// $Id: RichParticleIDType.cpp,v 1.1 2002-06-21 15:34:08 jonesc Exp $
// Include files

// local
#include "RichKernel/RichParticleIDType.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::ParticleIDType
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
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

// Implementation of next function
Rich::ParticleIDType Rich::next( const Rich::ParticleIDType& particle ) {
  switch( particle ) {
  case Rich::Unknown:         return Rich::Electron;
  case Rich::Electron:        return Rich::Muon;
  case Rich::Muon:            return Rich::Pion;
  case Rich::Pion:            return Rich::Kaon;
  case Rich::Kaon:            return Rich::Proton;
  case Rich::Proton:          return Rich::BelowThreshold;
  case Rich::BelowThreshold:  return Rich::Unknown;
  default:                    return Rich::Unknown; // should never happen
  }
}

// Implementation of next function
Rich::ParticleIDType Rich::previous( const Rich::ParticleIDType& particle ) {
  switch( particle ) {
  case Rich::Unknown:         return Rich::BelowThreshold;
  case Rich::Electron:        return Rich::Unknown;
  case Rich::Muon:            return Rich::Electron;
  case Rich::Pion:            return Rich::Muon;
  case Rich::Kaon:            return Rich::Pion;
  case Rich::Proton:          return Rich::Kaon;
  case Rich::BelowThreshold:  return Rich::Proton;
  default:                    return Rich::Unknown; // should never happen
  }
}
