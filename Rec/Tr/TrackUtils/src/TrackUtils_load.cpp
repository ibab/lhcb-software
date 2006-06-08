#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackUtils ) {

   DECLARE_ALGORITHM( TrackEventCloneKiller );
   DECLARE_ALGORITHM( TrackContainerCleaner );
   DECLARE_ALGORITHM( TTrackFromLong );
}

