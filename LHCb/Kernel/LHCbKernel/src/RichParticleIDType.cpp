
//-----------------------------------------------------------------------------
/** @file RichParticleIDType.cpp
 *
 *  Implementation file for enumeration : Rich::ParticleIDType
 *
 *  CVS Log :-
 *  $Id: RichParticleIDType.cpp,v 1.1 2006-01-17 13:58:09 jonrob Exp $
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
  case Rich::BelowThreshold: return "below threshold";
  case Rich::Unknown:        return "unknown";
  default:                   return "?"; // should never happen
  }
}
