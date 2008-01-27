
//-----------------------------------------------------------------------------
/** @file MCRichHit.cpp
 *
 *  Implementation file for class : MCRichHit
 *
 *  CVS Log :-
 *  $Id: MCRichHit.cpp,v 1.1 2008-01-27 09:57:08 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    26/01/2008
 */
//-----------------------------------------------------------------------------

// local
#include "Event/MCRichHit.h"

LHCb::MCRichDigitHistoryCode LHCb::MCRichHit::mcRichDigitHistoryCode() const
{

  // make a default history code object
  LHCb::MCRichDigitHistoryCode hist;

  // set signal flags
  hist.setAerogelHit ( Rich::Aerogel == radiator() );
  hist.setC4f10Hit   ( Rich::C4F10   == radiator() );
  hist.setCf4Hit     ( Rich::CF4     == radiator() );
 
  // set background flags
  hist.setScatteredHit(scatteredPhoton());
  hist.setChargedTrack(chargedTrack());
  hist.setGasQuartzCK(gasQuartzCK());
  hist.setHpdQuartzCK(hpdQuartzCK());
  hist.setNitrogenCK(nitrogenCK());
  hist.setAeroFilterCK(aeroFilterCK());
  hist.setHpdSiBackscatter(hpdSiBackscatter());
  hist.setHpdReflQWPC(hpdReflQWPC());
  hist.setHpdReflChr(hpdReflChr());
  hist.setHpdReflAirQW(hpdReflAirQW());
  hist.setHpdReflAirPC(hpdReflAirPC());
  hist.setHpdReflSi(hpdReflSi());
  hist.setHpdReflKovar(hpdReflKovar());
  hist.setHpdReflKapton(hpdReflKapton());
  hist.setHpdReflPCQW(hpdReflPCQW());

  // return final flag
  return hist;
}
