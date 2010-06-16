
//-----------------------------------------------------------------------------
/** @file RichPID.cpp
 *
 *  Implementation file for class : RichPID
 *
 *  CVS Log :-
 *  $Id: RichPID.cpp,v 1.6 2010-03-09 18:39:17 jonrob Exp $
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
  if ( this->offlineGlobal() ) { hist+=(cnt>0?"+":""); hist+="OfflineGlobal"; ++cnt; }
  if ( this->offlineLocal()  ) { hist+=(cnt>0?"+":""); hist+="OfflineLocal";  ++cnt; }
  if ( this->ringRefit()     ) { hist+=(cnt>0?"+":""); hist+="RingRefit";     ++cnt; }
  if ( this->hltGlobal()     ) { hist+=(cnt>0?"+":""); hist+="HltGlobal";     ++cnt; }
  if ( this->hltLocal()      ) { hist+=(cnt>0?"+":""); hist+="HltLocal";      ++cnt; }
  return hist;
}

bool LHCb::RichPID::isAboveThreshold(const Rich::ParticleIDType type) const
{
  switch ( type )
  {
  case Rich::Pion           :  return this->pionHypoAboveThres();
  case Rich::Electron       :  return this->electronHypoAboveThres();
  case Rich::Muon           :  return this->muonHypoAboveThres();
  case Rich::Kaon           :  return this->kaonHypoAboveThres();
  case Rich::Proton         :  return this->protonHypoAboveThres();
  case Rich::BelowThreshold :  return false;
  default                   :  return false;
  }
}

void LHCb::RichPID::setAboveThreshold(const Rich::ParticleIDType type,
                                      const bool flag)
{
  switch ( type )
  {
  case Rich::Pion        :  this->setPionHypoAboveThres(flag);
  case Rich::Electron    :  this->setElectronHypoAboveThres(flag);
  case Rich::Muon        :  this->setMuonHypoAboveThres(flag);
  case Rich::Kaon        :  this->setKaonHypoAboveThres(flag);
  case Rich::Proton      :  this->setProtonHypoAboveThres(flag);
  default                :  return; // Covers Rich::BelowThreshold
  }
}

bool LHCb::RichPID::traversedRadiator(const Rich::RadiatorType radiator) const
{
  switch ( radiator )
  {
  case Rich::Aerogel  : return this->usedAerogel();
  case Rich::Rich1Gas : return this->usedRich1Gas();
  case Rich::Rich2Gas : return this->usedRich2Gas();
  default             : return false;
  }
}

std::ostream& LHCb::RichPID::fillStream(std::ostream& s) const
{
  s << "[ ";

  // Formatting strings
  const std::string sF = "%7.3f";

  // PID type
  s << "Key " << key() << " (" << pidType() << ")";

  // Track info
  if ( track() )
  {
    const LHCb::State* state = &(track())->firstState();
    const double tkPtot      = ( state ? state->p()/Gaudi::Units::GeV : 0.0 );
    s << " | Track " << track()->key() << " type=" << track()->type()
      << " Ptot=" << boost::format(sF)%tkPtot << " GeV/c";
  }
  else
  {
    s << " | NO ASSOCIATED TRACK";
  }

  // Active radaitors
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
  s << " | Dlls =";
  for ( Rich::Particles::const_iterator ipid = Rich::particles().begin();
        ipid != Rich::particles().end(); ++ipid )
  {
    s << " " << boost::format(sF)%(particleDeltaLL(*ipid));
  }

  // return message stream
  return s << " ]";
}
