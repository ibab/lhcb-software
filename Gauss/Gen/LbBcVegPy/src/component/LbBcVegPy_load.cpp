// $Id: LbBcVegPy_load.cpp,v 1.3 2007-05-08 13:45:03 robbep Exp $
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
 
  // Cut Tools
  DECLARE_TOOL( BcDaughtersInLHCb ) ;
}
