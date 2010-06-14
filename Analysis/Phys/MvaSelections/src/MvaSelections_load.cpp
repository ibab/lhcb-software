// $Id: MvaSelections_load.cpp,v 1.2 2010/06/10 12:42:22 seaso Exp $
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

DECLARE_FACTORY_ENTRIES(MvaSelections) {

  DECLARE_TOOL(B2DHSelectionParamTool);
  DECLARE_TOOL(B2DHMvaFisherDMonitorHistoTool);
  DECLARE_TOOL(Bs2DsHMvaUtilityTool);
  DECLARE_TOOL(B2DHSelectionCutsTool);
  DECLARE_ALGORITHM(B2DHMvaSelectionAlg);
  
  
}
