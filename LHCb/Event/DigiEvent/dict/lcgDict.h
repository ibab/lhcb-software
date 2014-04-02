#ifndef DIGIEVENT_LCGDICT_H 
#define DIGIEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include "Kernel/FastClusterContainer.h"
#include "Event/VeloLiteCluster.h"
#include "Event/STLiteCluster.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    FastClusterContainer<LHCb::VeloLiteCluster, int> _i1;
    FastClusterContainer<LHCb::STLiteCluster, int>   _i2;
    // end instantiations
  };
}

#endif // DIGIEVENT_LCGDICT_H
