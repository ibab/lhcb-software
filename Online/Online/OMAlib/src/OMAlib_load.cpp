// $Id: OMAlib_load.cpp,v 1.1 2008-03-11 18:23:26 ggiacomo Exp $
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
DECLARE_FACTORY_ENTRIES( OMAlib ) {
  DECLARE_ALGORITHM( DBDrivenAnalysisTask );  
  DECLARE_ALGORITHM( ExampleAnalysisTask );  
}
