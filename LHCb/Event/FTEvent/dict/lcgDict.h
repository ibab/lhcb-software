#ifndef FTEVENT_LCGDICT_H 
#define FTEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// Include files
#include "Kernel/FastClusterContainer.h"
#include "Event/FTRawCluster.h"
// end include files

/** @class lcgDict lcgDict.h dict/lcgDict.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2013-05-06
 */
namespace {
  struct _Instantiations {
    // begin instantiations
    FastClusterContainer<LHCb::FTRawCluster, int> _;
    // end instantiations
  };
}

#endif // FTEVENT_LCGDICT_H
