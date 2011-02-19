
//-----------------------------------------------------------------------------
/** @file RichPID.cpp
 *
 *  Implementation file for class : LHCb::RichPID
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// Include to suppress "debug information too long" warning on Windows
#include "GaudiKernel/Kernel.h"

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
  const std::string sF = "%7.3f";

  // PID type
  s << "Key=" << key() << " " << pidType();

  // Track info
  if ( track() )
  {
    s << " | Track Key=" << track()->key() << " " << track()->type()
      << " Ptot = " << boost::format(sF) % (track()->p()/Gaudi::Units::GeV)
      << " GeV/c";
  }
  else
  {
    s << " | NO ASSOCIATED TRACK";
  }

  // Active radiators
  s << " | Active Rads =";
  if ( usedAerogel()  ) { s << " " << Rich::text(Rich::Aerogel);  }
  if ( usedRich1Gas() ) { s << " " << Rich::text(Rich::Rich1Gas); }
  if ( usedRich2Gas() ) { s << " " << Rich::text(Rich::Rich2Gas); }

  // Mass thresholds
  s << " | Thresholds = ";
  for ( Rich::Particles::const_iterator ipid = Rich::particles().begin();
        ipid != Rich::particles().end(); ++ipid )
  {
    const std::string T = isAboveThreshold(*ipid) ? "T" : "F";
    s << T << " ";
  }

  // DLL values
  s << " | DLLs(el,mu,pi,ka,pr,bt) =";
  for ( Rich::Particles::const_iterator ipid = Rich::particles().begin();
        ipid != Rich::particles().end(); ++ipid )
  {
    s << " " << boost::format(sF)%(particleDeltaLL(*ipid));
  }

  // return message stream
  return s << " ]";
}
