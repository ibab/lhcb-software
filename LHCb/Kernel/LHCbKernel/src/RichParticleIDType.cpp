
//-----------------------------------------------------------------------------
/** @file RichParticleIDType.cpp
 *
 *  Implementation file for enumeration : Rich::ParticleIDType
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichParticleIDType.h"

// Text conversion for ParticleIDType enumeration
std::string Rich::text( const Rich::ParticleIDType particle )
{
  switch( particle )
  {
  case Rich::Electron:       return "electron";
  case Rich::Muon:           return "muon";
  case Rich::Pion:           return "pion";
  case Rich::Kaon:           return "kaon";
  case Rich::Proton:         return "proton";
  case Rich::BelowThreshold: return "below_threshold";
  case Rich::Deuteron: 	     return "deuteron";
  case Rich::Unknown:        return "unknown";
  default:                   return "?"; // should never happen
  }
}

const Rich::Particles & Rich::particles() noexcept
{
  static Rich::Particles pids =
    { Rich::Electron, Rich::Muon, 
      Rich::Pion,     Rich::Kaon, 
      Rich::Proton,   Rich::Deuteron,
      Rich::BelowThreshold };
  return pids;
}
