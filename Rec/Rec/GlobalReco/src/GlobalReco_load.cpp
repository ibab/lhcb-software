// $Id: GlobalReco_load.cpp,v 1.2 2002-07-11 16:24:20 gcorti Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(GlobalReco) {
  
  DECLARE_ALGORITHM( ChargedProtoPAlg );

  DECLARE_ALGORITHM(PrimaryVertexFinderAlg);
  DECLARE_TOOL(PrimaryVertexFitterTool);

}
