// $Id: RichParticleIDType.cpp,v 1.2 2003-04-01 11:22:58 jonrob Exp $
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
