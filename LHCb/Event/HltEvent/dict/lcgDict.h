// $Id: lcgDict.h,v 1.3 2009-06-03 20:47:23 jpalac Exp $
#ifndef DICT_LCGDICT_H 
#define DICT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Event/HltObjectSummary.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    GaudiUtils::VectorMap< std::string, LHCb::HltDecReport > _i0;
    std::vector<const LHCb::HltObjectSummary*>               _i1;
    std::vector<LHCb::HltObjectSummary*>                     _i2; 
    LHCb::HltObjectSummary::Info                             _i3;
    // end instantiations
  };
}



#endif // DICT_LCGDICT_H
