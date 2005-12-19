// $Id: STTools_load.cpp,v 1.1.1.1 2005-12-19 15:58:58 mneedham Exp $
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

DECLARE_FACTORY_ENTRIES(STTools) {
  DECLARE_TOOL( STSignalToNoiseTool );
  DECLARE_TOOL( STOfflinePosition );
  DECLARE_TOOL( STOnlinePosition );
  
  DECLARE_TOOL( DotDump ) ;

}
