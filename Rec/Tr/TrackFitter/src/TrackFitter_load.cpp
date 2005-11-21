#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackFitter ) {

   DECLARE_ALGORITHM( TrackEventFitter );

   DECLARE_TOOL( TrackKalmanFilter );
   DECLARE_TOOL( TrackOldKalmanFilter );
   DECLARE_TOOL( TrackMasterFitter );

}

