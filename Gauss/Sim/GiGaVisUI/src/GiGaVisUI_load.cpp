// $Id: GiGaVisUI_load.cpp,v 1.3 2007-01-12 15:48:40 ranjard Exp $ 

#include "GaudiKernel/DeclareFactoryEntries.h" 

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(Test) {

  // UI & Vis 
  DECLARE_TOOL( GiGaUIsession );
  DECLARE_TOOL( GiGaVisManager );
  DECLARE_TOOL( GiGaSetVisAttributes );
  
}
