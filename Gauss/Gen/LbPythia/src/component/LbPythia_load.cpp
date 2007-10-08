// $Id: LbPythia_load.cpp,v 1.7 2007-10-08 16:52:00 gcorti Exp $
// Include files 

//
//  Package    : Gen/LbPythia
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

// Declaration of the PythiaProduction tool factory: here for Gaudi v19r3 and
// compatibility with Gaudi v18
#include "LbPythia/PythiaProduction.h"
DECLARE_TOOL_FACTORY( PythiaProduction );

DECLARE_FACTORY_ENTRIES(LbPythia) {
  // New structure:
 
  // Production tools
  DECLARE_TOOL( PythiaProduction      ) ;
  DECLARE_TOOL( ReadLHEfileProduction ) ;
  DECLARE_TOOL( JetProduction ) ;
 
  // Cut tools
  DECLARE_TOOL( PythiaHiggs ) ;
  DECLARE_TOOL( PythiaHiggsType ) ;
}
