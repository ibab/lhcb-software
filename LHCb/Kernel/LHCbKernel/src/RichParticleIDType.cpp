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

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
// boost
#include "boost/assign/list_of.hpp"
#endif

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
  case Rich::Unknown:        return "unknown";
  default:                   return "?"; // should never happen
  }
}

const Rich::Particles & Rich::particles()
{
  static Rich::Particles pids =
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
    {Rich::Electron, Rich::Muon, Rich::Pion, Rich::Kaon, Rich::Proton, Rich::BelowThreshold};
#else
    boost::assign::list_of(Rich::Electron)(Rich::Muon)(Rich::Pion)(Rich::Kaon)(Rich::Proton)(Rich::BelowThreshold);
#endif
  return pids;
}
