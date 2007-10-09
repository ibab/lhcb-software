// $Id: SeedHit.cpp,v 1.3 2007-10-09 17:56:26 smenzeme Exp $
// Include files 

// local
#include "TsaKernel/SeedHit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SeedHit
//
// 2007-07-17 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& Tf::Tsa::SeedHit::fillStream(std::ostream& s) const
{
  return s << "{"
    //<< " Key=" << this->key()
           << " " << lhcbID()
           << " x,y,z,r=" << z() << "," << y() << "," << z() << "," << r()
           << " x(Max,Min)=" << xMax() << "," << xMin() 
           << " y(Max,Min)=" << yMax() << "," << yMin()
           << " }";
}
