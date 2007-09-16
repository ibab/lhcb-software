// $Id: SeedHit.cpp,v 1.2 2007-09-16 12:21:09 krinnert Exp $
// Include files 

// local
#include "TsaKernel_Tf/SeedHit.h"

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
