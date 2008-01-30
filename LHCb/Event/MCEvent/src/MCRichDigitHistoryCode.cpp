
//-----------------------------------------------------------------------------
/** @file MCRichDigitHistoryCode.cpp
 *
 *  Implementation file for class : MCRichDigitHistoryCode
 *
 *  CVS Log :-
 *  $Id: MCRichDigitHistoryCode.cpp,v 1.3 2008-01-30 10:58:35 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    01/09/2006
 */
//-----------------------------------------------------------------------------

#include "Event/MCRichDigitHistoryCode.h"

std::ostream& LHCb::MCRichDigitHistoryCode::fillStream(std::ostream& s) const
{
  s << "[";

  // event history
  if ( signalEvent()   ) s << " SignalEvent";
  if ( prevEvent()     ) s << " PrevEvent";
  if ( prevPrevEvent() ) s << " PrevPrevEvent";
  if ( nextEvent()     ) s << " NextEvent";
  if ( nextNextEvent() ) s << " NextNextEvent";
  
  // signal radiators
  if ( aerogelHit() ) s << " AeroCK";
  if ( c4f10Hit()   ) s << " R1GasCK";
  if ( cf4Hit()     ) s << " R2GasCK";

  // background radiators
  if ( gasQuartzCK()  ) s << " GasQuartzCK";
  if ( hpdQuartzCK()  ) s << " HPDQuartzCK";
  if ( nitrogenCK()   ) s << " N2CK";
  if ( aeroFilterCK() ) s << " AeroFilterCK";

  // background history
  if ( scatteredHit()     ) s << " RayleighScattered";
  if ( chargedTrack()     ) s << " TrackHitHPD";
  if ( darkHit()          ) s << " DarkHit";
  if ( chargeShareHit()   ) s << " ChargeShare";
  if ( hpdSiBackscatter() ) s << " hpdSiBackscatter";
  if ( hpdReflQWPC()      ) s << " hpdReflQWPC";
  if ( hpdReflChr()       ) s << " hpdReflChr";
  if ( hpdReflAirQW()     ) s << " hpdReflAirQW";
  if ( hpdReflAirPC()     ) s << " hpdReflAirPC";
  if ( hpdReflSi()        ) s << " hpdReflSi";
  if ( hpdReflKovar()     ) s << " hpdReflKovar";
  if ( hpdReflKapton()    ) s << " hpdReflKapton";
  if ( hpdReflPCQW()      ) s << " hpdReflPCQW";

  return s << " ]";
}
