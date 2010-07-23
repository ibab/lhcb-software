// $Id: $
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

DECLARE_FACTORY_ENTRIES(MvaPreSelB2DH) {
  DECLARE_TOOL(B2DHMvaPreSelectionParamTool);
  DECLARE_TOOL(B2DHPreselMvaFisherDMonitorHistoTool);
  DECLARE_TOOL(B2DHPreselMvaUtilityTool);
  DECLARE_TOOL(B2DHMvaPreSelectionCutsTool);
  DECLARE_ALGORITHM(B2DHMvaPreSelectionAlg);


}
