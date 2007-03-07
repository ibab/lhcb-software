// $Id: AlignSolvTools_load.cpp,v 1.2 2007-03-07 13:09:15 ahicheur Exp $
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

DECLARE_FACTORY_ENTRIES(AlignSolvTools) {
  DECLARE_TOOL(AlignSolvTool);
  //  DECLARE_ALGORITHM( SolvExample );
  
}
