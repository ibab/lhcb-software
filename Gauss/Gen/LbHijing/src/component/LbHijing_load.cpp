// $Id: LbHijing_load.cpp,v 1.1.1.1 2006-07-03 17:23:35 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

//
// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(LbHijing) {

  DECLARE_TOOL( HijingProduction );
  
}
