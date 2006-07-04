// $Id: LbBcVegPy_load.cpp,v 1.2 2006-07-04 11:59:40 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(LbBcVegPy) {
  // New structure:
 
  // Production tools
  DECLARE_TOOL( BcVegPyProduction ) ;
 
}
