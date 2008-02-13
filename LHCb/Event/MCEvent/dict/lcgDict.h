// $Id: lcgDict.h,v 1.8 2008-02-13 15:56:17 cattanem Exp $
#ifndef MCEVENT_LCGDICT_H 
#define MCEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Event/MCTrackInfo.h"
#include "Event/MCParticle.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::vector<const LHCb::MCParticle*> _i1;
    std::pair<LHCb::MCParticle*,double>  _i2;
    // end instantiations
  };
}

#endif // MCEVENT_LCGDICT_H
