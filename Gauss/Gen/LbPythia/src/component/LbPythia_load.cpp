// $Id: LbPythia_load.cpp,v 1.4 2006-10-04 20:03:03 robbep Exp $
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

DECLARE_FACTORY_ENTRIES(LbPythia) {
  // New structure:
 
  // Production tools
  DECLARE_TOOL( PythiaProduction      ) ;
  DECLARE_TOOL( ReadLHEfileProduction ) ;
 
  // Cut tools
  DECLARE_TOOL( PythiaHiggs ) ;
  
}
