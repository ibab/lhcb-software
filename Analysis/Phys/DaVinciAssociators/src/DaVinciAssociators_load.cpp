// $Id: DaVinciAssociators_load.cpp,v 1.1.1.1 2002-04-25 15:57:35 phicharp Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "DaVinciAssociators/Part2MCPartAsct.h"

static const ToolFactory<Part2MCPartAsct>     s_Part2MCPartAsctFactory; 
const       IToolFactory& Part2MCPartAsctFactory = s_Part2MCPartAsctFactory;

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( PhysAssociators ) {

  DECLARE_TOOL( Part2MCPartAsct );
  
  DECLARE_ALGORITHM( Part2MCPartChi2Alg );

}
