
//-----------------------------------------------------------------------------
/** @file MCRichDigitHistoryCode.cpp
 *
 *  Implementation file for class : MCRichDigitHistoryCode
 *
 *  CVS Log :-
 *  $Id: MCRichDigitHistoryCode.cpp,v 1.1 2006-09-01 10:32:38 jonrob Exp $
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
  if ( signalEvent()   ) s << " :SignalEvent";
  if ( prevEvent()     ) s << " :PrevEvent";
  if ( prevPrevEvent() ) s << " :PrevPrevEvent";
  if ( nextEvent()     ) s << " :NextEvent";
  if ( nextNextEvent() ) s << " :NextNextEvent";
  
  // signal types
  if ( aerogelHit() ) s << " AeroCK";
  if ( c4f10Hit() )   s << " R1GasCK";
  if ( cf4Hit() )     s << " R2GasCK";

  // background history
  if ( scatteredHit() )   s << " Scattered";
  if ( chargedTrack() )   s << " ChargedTrack";
  if ( gasQuartzCK() )    s << " GasQuartzCK";
  if ( hpdQuartzCK() )    s << " HPDQuartzCK";
  if ( nitrogenCK() )     s << " N2CK";
  if ( aeroFilterCK() )   s << " AeroFilterCK";
  if ( darkHit() )        s << " DarkHit";
  if ( chargeShareHit() ) s << " ChargeShare";

  s << " ]";
  return s;
}
