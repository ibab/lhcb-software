// $Id: lcgStl.h,v 1.1 2004-06-10 08:19:52 cattanem Exp $
#ifndef EVENT_LCGSTL_H 
#define EVENT_LCGSTL_H 1

// Dummy use of complex STL classes used in LHCb event model
// See also similar file in LHCbKernel
// For lcg dictionary generation

// Include files
#include <vector>
#include <map>
#include "Event/L1Bank.h"
#include "Event/RawBank.h"

namespace {
  std::map<int,std::vector<L1Bank> >  a;
  std::map<int,std::vector<RawBank> > b;
  std::vector<L1Bank>                 c;
  std::vector<RawBank>                d;
}
#endif // EVENT_LCGSTL_H
