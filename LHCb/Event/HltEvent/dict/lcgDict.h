// $Id: lcgDict.h,v 1.2 2009-02-27 01:09:13 tskwarni Exp $
#ifndef DICT_LCGDICT_H 
#define DICT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Event/HltObjectSummary.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::vector<const LHCb::HltObjectSummary*> _i1;
    std::vector<LHCb::HltObjectSummary*>       _i2; 
    LHCb::HltObjectSummary::Info _i3;
    // end instantiations
  };
}



#endif // DICT_LCGDICT_H
