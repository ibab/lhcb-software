
//-----------------------------------------------------------------------------
/** @file RichPID.cpp
 *
 *  Implementation file for class : LHCb::RichPID
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// RichEvent includes
#include "Event/RichPID.h"

// Boost
#include "boost/format.hpp"

std::string LHCb::RichPID::pidType() const
{
  std::string hist;
  int cnt = 0;
  if ( this->offlineGlobal() ) {                       hist+="OfflineGlobal"; ++cnt; }
  if ( this->offlineLocal()  ) { hist+=(cnt>0?"+":""); hist+="OfflineLocal";  ++cnt; }
  if ( this->ringRefit()     ) { hist+=(cnt>0?"+":""); hist+="RingRefit";     ++cnt; }
  if ( this->hltGlobal()     ) { hist+=(cnt>0?"+":""); hist+="HltGlobal";     ++cnt; }
  if ( this->hltLocal()      ) { hist+=(cnt>0?"+":""); hist+="HltLocal";      ++cnt; }
  return hist;
}

bool LHCb::RichPID::isAboveThreshold( const Rich::ParticleIDType type ) const
{
  return ( Rich::Electron == type ? this->electronHypoAboveThres() :
           Rich::Muon     == type ? this->muonHypoAboveThres()     :
           Rich::Pion     == type ? this->pionHypoAboveThres()     :
           Rich::Kaon     == type ? this->kaonHypoAboveThres()     :
           Rich::Proton   == type ? this->protonHypoAboveThres()   :
           Rich::Deuteron == type ? this->deuteronHypoAboveThres() :
           false );
}

void LHCb::RichPID::setAboveThreshold( const Rich::ParticleIDType type,
                                       const bool flag )
{
  if      ( Rich::Electron == type ) { this->setElectronHypoAboveThres(flag); }
  else if ( Rich::Muon     == type ) { this->setMuonHypoAboveThres(flag);     }
  else if ( Rich::Pion     == type ) { this->setPionHypoAboveThres(flag);     }
  else if ( Rich::Kaon     == type ) { this->setKaonHypoAboveThres(flag);     }
  else if ( Rich::Proton   == type ) { this->setProtonHypoAboveThres(flag);   }
  else if ( Rich::Deuteron == type ) { this->setDeuteronHypoAboveThres(flag); }
}

bool LHCb::RichPID::traversedRadiator(const Rich::RadiatorType radiator) const
{
  return ( Rich::Aerogel  == radiator ? this->usedAerogel()  :
           Rich::Rich1Gas == radiator ? this->usedRich1Gas() :
           Rich::Rich2Gas == radiator ? this->usedRich2Gas() :
           false );
}

std::ostream& LHCb::RichPID::fillStream( std::ostream& s ) const
{
  s << "[ ";

  // Formatting strings
  const std::string sF = "%6.2f";

  // PID type
  s << "Key=" << key() << " " << pidType();

  // Track info
  if ( track() )
  {
    s << " | Tk " << track()->key() << " " << track()->type()
      << " " << boost::format(sF) % (track()->p()/Gaudi::Units::GeV)
      << " GeV";
  }
  else
  {
    s << " | NO ASSOCIATED TRACK";
  }

  // Active radiators
  s << " |";
  if ( usedAerogel()  ) { s << " " << Rich::text(Rich::Aerogel);  }
  if ( usedRich1Gas() ) { s << " " << Rich::text(Rich::Rich1Gas); }
  if ( usedRich2Gas() ) { s << " " << Rich::text(Rich::Rich2Gas); }

  // Mass thresholds
  s << " | Thres ";
  for ( const auto pid : Rich::particles() )
  {
    s << ( isAboveThreshold(pid) ? "T" : "F" );
  }

  // DLL values
  s << " | DLLs ";
  for ( const auto pid : Rich::particles() )
  {
    s << " " << boost::format(sF) % particleDeltaLL(pid);
  }

  // Best ID
  s << " | " << bestParticleID();

  // return message stream
  return s << " ]";
}
