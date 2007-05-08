#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( AlignTrTools ) {
  DECLARE_ALGORITHM( AlignTrackMonitor );
  DECLARE_TOOL( AlignSelTool );
  DECLARE_TOOL( TAConfig );
  DECLARE_TOOL( Derivatives );
  DECLARE_ALGORITHM( ATrackSelector );
}

