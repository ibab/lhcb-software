// Include files 
#include "GaudiKernel/ContainerFactoryDefs.h"

//-----------------------------------------------------------------------------
// Dummy file to please Windows. Does not contain factory implementations
// because TrFitEvent classes don't have a classID
//
// 2005-05-25: Marco Cattaneo
//-----------------------------------------------------------------------------

/// ====================================================================

void TrackFitEvent_load() {
}

extern "C" void TrackFitEvent_loadRef()  {
  TrackFitEvent_load();
}
