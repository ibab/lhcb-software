// $Id: BlsMoniSim_load.cpp,v 1.1.1.2 2010-03-10 17:38:47 vtalanov Exp $
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

DECLARE_FACTORY_ENTRIES ( BlsMoniSim ) {
  DECLARE_ALGORITHM ( BlsHitChecker )
}
