// $Id: lcgDict.h,v 1.7 2007-05-25 16:30:33 cattanem Exp $
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
    // end instantiations
  };
}

#endif // MCEVENT_LCGDICT_H
