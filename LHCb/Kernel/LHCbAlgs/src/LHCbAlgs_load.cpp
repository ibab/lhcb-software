// $Id: LHCbAlgs_load.cpp,v 1.12 2006-06-29 12:47:04 mneedham Exp $
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
  DECLARE_TOOL( TrajPoca );

  DECLARE_ALGORITHM( ProcessPhase );
  DECLARE_ALGORITHM( TESCheck );
  DECLARE_ALGORITHM( EventNodeKiller );
}
