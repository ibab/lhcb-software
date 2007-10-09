// $Id: SeedPnt.cpp,v 1.3 2007-10-09 17:56:26 smenzeme Exp $
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
