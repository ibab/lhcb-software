// $Id: LbPythia8_load.cpp,v 1.1.1.1 2007-07-31 17:02:19 robbep Exp $
// Include files 

//
//  Package    : Gen/LbPythia8
//
//  Description: Declaration of the components (factory entries)
//               specified by this component library.
//

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

// Declaration of the Pythia8Production tool factory: here for Gaudi v19r3 and
// compatibility with Gaudi v18
#include "Pythia8Production.h"
DECLARE_TOOL_FACTORY( Pythia8Production );

DECLARE_FACTORY_ENTRIES(LbPythia8) { 
  // Production tools
  DECLARE_TOOL( Pythia8Production      ) ;
}
