#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( AlignTrTools ) {
  DECLARE_ALGORITHM( AlignTrackMonitor );
  DECLARE_TOOL( AlignSelTool );
  DECLARE_ALGORITHM( ATrackSelector );
}

