// $Id: lcgStl.h,v 1.2 2005-02-03 10:21:32 cattanem Exp $
#ifndef DAQEVENT_LCGSTL_H 
#define DAQEVENT_LCGSTL_H 1

// Dummy use of complex STL classes used in LHCb event model
// See also similar file in LHCbKernel
// For lcg dictionary generation

// Include files
#include <vector>
#include <map>
#include "Event/L1Bank.h"
#include "Event/RawBank.h"
#include "Event/L1Buffer.h"
#include "Event/RawBuffer.h"

namespace {
  std::map<int,std::vector<L1Bank> >  a;
  std::map<int,std::vector<RawBank> > b;
  std::vector<L1Bank>                 c;
  std::vector<RawBank>                d;
  BufferSerializer<RawBuffer>         e;
  BufferSerializer<L1Buffer>          f;
}
#endif // DAQEVENT_LCGSTL_H
