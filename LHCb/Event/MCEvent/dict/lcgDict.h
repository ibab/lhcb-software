// $Id: lcgDict.h,v 1.9 2008-02-15 07:30:31 cattanem Exp $
#ifndef MCEVENT_LCGDICT_H 
#define MCEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <utility>
#include <vector>
#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::vector<const LHCb::MCParticle*> _i1;
    std::pair<LHCb::MCParticle*,double>  _i2;
    std::vector<const LHCb::MCVertex*>   _i3;
    // end instantiations
  };
}

#endif // MCEVENT_LCGDICT_H
