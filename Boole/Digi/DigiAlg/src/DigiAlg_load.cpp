// $Id: DigiAlg_load.cpp,v 1.2 2003-10-06 16:16:40 cattanem Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(DigiAlg) {
  DECLARE_TOOL( LumiTool );
  DECLARE_ALGORITHM( BooleInit );
  DECLARE_ALGORITHM( MergeEventAlg );
  DECLARE_ALGORITHM( CreateL1Event );
}
