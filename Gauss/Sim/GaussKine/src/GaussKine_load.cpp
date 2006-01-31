// $Id: GaussKine_load.cpp,v 1.1.1.1 2006-01-31 11:03:51 gcorti Exp $
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

DECLARE_FACTORY_ENTRIES(GaussKine) {
  
  DECLARE_ALGORITHM( G4HepMCToMCTruth );
  DECLARE_ALGORITHM( GeneratorToG4 );
  
}
