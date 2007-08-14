// $Id: SeedPnt.cpp,v 1.1.1.1 2007-08-14 13:42:02 jonrob Exp $
// Include files 

// local
#include "TsaKernel/SeedPnt.h"

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
