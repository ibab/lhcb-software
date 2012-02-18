// $Id: lcgDict.h,v 1.2 2008-02-20 14:13:18 ibelyaev Exp $
#ifndef TRACKEVENT_LCGDICT_H 
#define TRACKEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Event/Track.h"

// end include files

namespace 
{
  struct _Instantiations 
  {
    // begin instantiations
    std::vector<const LHCb::Track*>       _i1;
    std::vector<LHCb::Track*>             _i2;
    std::pair<const LHCb::Track*,float>   _i3;
    std::vector<std::pair<const LHCb::Track*,float> > _i4;
    // end instantiations
  };
}

#endif // TRACKEVENT_LCGDICT_H
