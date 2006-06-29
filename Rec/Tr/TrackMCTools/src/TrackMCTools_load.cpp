#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( TrackMCTools ) {

  DECLARE_TOOL( TrackAcceptance );
  DECLARE_TOOL( TrackCriteriaSelector );
  DECLARE_TOOL( IdealStateCreator );
  DECLARE_TOOL( CleanPatTrackSelector );
}
