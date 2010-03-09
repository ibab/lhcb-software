
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
  case Rich::Aerogel   : return this->usedAerogel();
  case Rich::Rich1Gas  : return this->usedRich1Gas();
  case Rich::Rich2Gas  : return this->usedRich2Gas();
  default              : return false;
  }
}
