// $Id: LHCbAlgs_load.cpp,v 1.11 2006-03-09 12:24:33 cattanem Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( LHCbAlgs ) {
  DECLARE_TOOL( TimingTool );
  DECLARE_TOOL( MemoryTool );

  DECLARE_ALGORITHM( ProcessPhase );
  DECLARE_ALGORITHM( TESCheck );
  DECLARE_ALGORITHM( EventNodeKiller );
}
