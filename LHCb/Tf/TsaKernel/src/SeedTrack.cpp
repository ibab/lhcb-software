// $Id: SeedTrack.cpp,v 1.2 2007-09-16 09:44:46 mneedham Exp $
// Include files 

// local
#include "TsaKernel_Tf/SeedTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SeedTrack
//
// 2007-07-18 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& Tf::Tsa::SeedTrack::fillStream(std::ostream& s) const
{
  return s << "{"
           << " select=" << select()
           << " live=" << live()
           << " nx=" << nx()
           << " nx=" << nx()
           << " nHit=" << nHit()
           << " x0=" << x0()
           << " y0=" << y0()
           << " sx=" << sx()
           << " sy=" << sy()
           << " tx=" << tx()
           << " xChis2=" << xChi2()
           << " yChis2=" << yChi2()
           << " sector=" << sector()
           << " dth=" << dth()
           << " lik=" << lik() 
           << " }";
}
