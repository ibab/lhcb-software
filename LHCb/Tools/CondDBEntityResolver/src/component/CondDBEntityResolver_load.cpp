// $Id: CondDBEntityResolver_load.cpp,v 1.1.1.1 2005-10-26 15:37:02 marcocle Exp $
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

DECLARE_FACTORY_ENTRIES(CondDBEntityResolver) {
  DECLARE_SERVICE ( CondDBEntityResolverSvc );
}
  
