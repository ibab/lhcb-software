// $Id: DaVinciAssociators_load.cpp,v 1.2 2002-05-10 15:08:21 phicharp Exp $
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
  
  DECLARE_ALGORITHM( Part2MCChi2 );
  DECLARE_ALGORITHM( Part2MCLink );

}
