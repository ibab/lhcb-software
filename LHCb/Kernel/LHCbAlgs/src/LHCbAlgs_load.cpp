// $Id: LHCbAlgs_load.cpp,v 1.9 2005-12-14 13:22:21 cattanem Exp $
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
  DECLARE_TOOL( InitRndmTool );
  DECLARE_TOOL( MeomoryTool );

  DECLARE_ALGORITHM( ProcessPhase );
  DECLARE_ALGORITHM( TESCheck );
  DECLARE_ALGORITHM( EventNodeKiller );
}
