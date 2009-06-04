// $Id: lcgDict.h,v 1.4 2009-06-04 20:06:25 tskwarni Exp $
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
    GaudiUtils::VectorMap< std::string, LHCb::HltVertexReports::HltVertexReport > _i1;
    GaudiUtils::VectorMap< std::string, LHCb::HltObjectSummary > _i2;
    std::vector<const LHCb::HltObjectSummary*>               _i3;
    std::vector<LHCb::HltObjectSummary*>                     _i4; 
    LHCb::HltObjectSummary::Info                             _i5;
    // end instantiations
  };
}



#endif // DICT_LCGDICT_H
