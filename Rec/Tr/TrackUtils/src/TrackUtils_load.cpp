#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackUtils ) {

   DECLARE_ALGORITHM( TrackEventCloneKiller );
   DECLARE_ALGORITHM( TrackContainerCleaner );
   DECLARE_ALGORITHM( TTrackFromLong        );
   DECLARE_ALGORITHM( TrackContainerCopy    );
   DECLARE_ALGORITHM( TrackPrepareVelo      );
   DECLARE_ALGORITHM( TrackToDST            );
   DECLARE_ALGORITHM( TrackFromDST          );

}

