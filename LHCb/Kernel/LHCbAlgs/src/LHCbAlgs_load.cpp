// $Id: LHCbAlgs_load.cpp,v 1.6 2004-09-17 07:24:20 cattanem Exp $
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

  DECLARE_ALGORITHM( LbAppInit );
  DECLARE_ALGORITHM( ProcessPhase );
  DECLARE_ALGORITHM( TESFilter );
  DECLARE_ALGORITHM( TESCheck );
}
