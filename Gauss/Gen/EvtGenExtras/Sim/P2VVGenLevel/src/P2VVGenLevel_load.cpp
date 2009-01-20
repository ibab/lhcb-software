// $Id: P2VVGenLevel_load.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
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

DECLARE_FACTORY_ENTRIES(P2VVGenLevel) {
  DECLARE_ALGORITHM( P2VVTupleTool );
  DECLARE_TOOL( P2VVAngleCalculator );
}
