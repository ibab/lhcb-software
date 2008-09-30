// $Id: lcg_selection.h,v 1.3 2008-09-30 08:05:05 cattanem Exp $
#ifndef L0EVENT_LCGDICT_H 
#define L0EVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <map>
#include <string>
#include <utility>
#include "Event/L0DUChannel.h"
// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::map<std::string,LHCb::L0DUChannel*> l0_1;
    std::map<std::string,LHCb::L0DUChannel*>::iterator l0_2;
    std::pair<const std::string,LHCb::L0DUChannel*>  l0_3;
    // end instantiations
  };
}

#endif // L0EVENT_LCGDICT_H
