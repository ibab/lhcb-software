// $Id: LHCbAlgs_load.cpp,v 1.3 2004-06-28 11:35:06 cattanem Exp $
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
  DECLARE_ALGORITHM( LbAppInit );
  DECLARE_ALGORITHM( ProcessPhase );
  DECLARE_ALGORITHM( TESFilter );
  DECLARE_ALGORITHM( StoreExplorer );
}
