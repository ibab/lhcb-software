// $Id: GaussMonitor_load.cpp,v 1.6 2007-05-16 17:33:29 gcorti Exp $
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

DECLARE_FACTORY_ENTRIES(GaussMonitor) {
 
  DECLARE_ALGORITHM( MCTruthFullMonitor );
  DECLARE_ALGORITHM( MCTruthMonitor );
  DECLARE_ALGORITHM( MCDecayCounter );
  DECLARE_ALGORITHM( GeneratorFullMonitor );
  DECLARE_ALGORITHM( GenMonitorAlg );
  DECLARE_ALGORITHM( GeneratorAnalysis );
  DECLARE_ALGORITHM( ProductionAnalysis );
  DECLARE_ALGORITHM( ProductionAsymmetry );
   
}
