// $Id: SeedTrack.cpp,v 1.3 2007-10-09 17:56:26 smenzeme Exp $
// Include files 

// local
#include "TsaKernel/SeedTrack.h"

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
