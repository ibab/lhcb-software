// $Id: SimComponents_load.cpp,v 1.1.1.1 2005-12-15 14:29:30 gcorti Exp $
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

DECLARE_FACTORY_ENTRIES(SimComponents) {

  DECLARE_ALGORITHM( DumpMCEventAlg );
  DECLARE_ALGORITHM( EvtTypeChecker );
  DECLARE_ALGORITHM( MCHitMonitor );

  DECLARE_SERVICE( EvtTypeSvc );

  DECLARE_TOOL( MCDecayFinder );
   
}
