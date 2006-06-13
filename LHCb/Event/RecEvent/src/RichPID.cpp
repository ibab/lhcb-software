
//-----------------------------------------------------------------------------
/** @file RichPID.cpp
 *
 *  Implementation file for class : RichPID
 *
 *  CVS Log :-
 *  $Id: RichPID.cpp,v 1.2 2006-06-13 13:14:46 jonrob Exp $
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
  if ( this->global()     ) { hist+=(cnt>0?"+":""); hist+="Global";     ++cnt; }
  if ( this->local()      ) { hist+=(cnt>0?"+":""); hist+="Local";      ++cnt; }
  if ( this->ringRefit()  ) { hist+=(cnt>0?"+":""); hist+="RingRefit";  ++cnt; }
  if ( this->trigGlobal() ) { hist+=(cnt>0?"+":""); hist+="TrigGlobal"; ++cnt; }
  if ( this->trigLocal()  ) { hist+=(cnt>0?"+":""); hist+="TrigLocal";  ++cnt; }
  return hist;
}
