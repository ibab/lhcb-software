// $Id: SeedPnt.cpp,v 1.2 2007-09-16 09:44:46 mneedham Exp $
// Include files 

// local
#include "TsaKernel_Tf/SeedPnt.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SeedPnt
//
// 2007-07-18 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& Tf::Tsa::SeedPnt::fillStream(std::ostream& s) const
{
  return s << "{ "
           << " sign=" << sign()
           << " coord=" << coord()
           << " z=" << z()
           << " skip=" << skip()
           << " " << *hit()
           << " }";
}
